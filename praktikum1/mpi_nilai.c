/*
 * ============================================================
 *  Program    : mpi_nilai.c
 *  Deskripsi  : Analisis Nilai Mahasiswa secara Paralel
 *               menggunakan OpenMPI (Master-Worker Pattern)
 * ============================================================
 *  Kompilasi  : mpicc -o mpi_nilai mpi_nilai.c
 *  Eksekusi   : mpirun --hostfile hosts.txt -np 5 ./mpi_nilai
 * ============================================================
 *  Alur:
 *    1. Master (Rank 0) membaca data_nilai.csv
 *    2. Master broadcast jumlah data ke semua proses
 *    3. Master scatter (bagi rata) data ke semua proses
 *    4. Setiap proses hitung max, min, sum lokal
 *    5. MPI_Reduce kumpulkan hasil global ke Master
 *    6. Master tampilkan hasil akhir
 * ============================================================
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

/* Ukuran buffer untuk membaca satu baris CSV */
#define LINE_BUF 256

/*
 * Fungsi: baca_csv
 * -----------------
 * Membaca file CSV dan mengekstrak kolom Nilai ke dalam array.
 *
 * Parameter:
 *   filename   - path ke file CSV
 *   nilai      - pointer ke array float (akan di-malloc di dalam fungsi)
 *   total_data - pointer untuk menyimpan jumlah baris data yang terbaca
 *
 * Return:
 *   0 jika berhasil, -1 jika gagal
 */
int baca_csv(const char *filename, float **nilai, int *total_data) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Tidak dapat membuka file '%s'\n", filename);
        return -1;
    }

    /* --- Fase 1: Hitung jumlah baris data (skip header) --- */
    char line[LINE_BUF];
    int count = 0;

    /* Skip baris header (NIM,Nilai) */
    if (fgets(line, LINE_BUF, fp) == NULL) {
        fprintf(stderr, "ERROR: File '%s' kosong\n", filename);
        fclose(fp);
        return -1;
    }

    /* Hitung baris data */
    while (fgets(line, LINE_BUF, fp) != NULL) {
        /* Abaikan baris kosong */
        if (strlen(line) > 1) {
            count++;
        }
    }

    if (count == 0) {
        fprintf(stderr, "ERROR: Tidak ada data di dalam file '%s'\n", filename);
        fclose(fp);
        return -1;
    }

    /* --- Fase 2: Alokasi memori dan baca ulang data --- */
    *nilai = (float *)malloc(count * sizeof(float));
    if (*nilai == NULL) {
        fprintf(stderr, "ERROR: Gagal mengalokasikan memori untuk %d data\n", count);
        fclose(fp);
        return -1;
    }

    /* Kembali ke awal file */
    rewind(fp);

    /* Skip header lagi */
    fgets(line, LINE_BUF, fp);

    int idx = 0;
    while (fgets(line, LINE_BUF, fp) != NULL && idx < count) {
        if (strlen(line) <= 1) continue;

        /* Parse: skip kolom NIM (sebelum koma), ambil kolom Nilai */
        char *token = strtok(line, ",");  /* Token pertama = NIM (dilewati) */
        token = strtok(NULL, ",\n\r");    /* Token kedua = Nilai */

        if (token != NULL) {
            (*nilai)[idx] = (float)atof(token);
            idx++;
        }
    }

    *total_data = idx;
    fclose(fp);
    return 0;
}

/*
 * ============================================================
 *  FUNGSI UTAMA
 * ============================================================
 */
int main(int argc, char **argv) {

    /* --------------------------------------------------------
     *  1. INISIALISASI MPI
     * -------------------------------------------------------- */
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Deteksi nama host tempat proses berjalan */
    char hostname[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(hostname, &name_len);

    float *all_nilai = NULL;   /* Array seluruh data (hanya di Master) */
    int    total_data = 0;     /* Jumlah total baris data              */

    /* --------------------------------------------------------
     *  2. MASTER: BACA FILE CSV
     * -------------------------------------------------------- */
    if (rank == 0) {
        printf("\n");
        printf("[Master @ %s] Membaca file data_nilai.csv ...\n", hostname);

        if (baca_csv("data_nilai.csv", &all_nilai, &total_data) != 0) {
            fprintf(stderr, "[Master] FATAL: Gagal membaca CSV. Program dihentikan.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }

        printf("[Master @ %s] Berhasil membaca %d data nilai.\n", hostname, total_data);

        /* Validasi: jumlah data harus habis dibagi jumlah proses */
        if (total_data % size != 0) {
            fprintf(stderr, "[Master] WARNING: Jumlah data (%d) tidak habis dibagi"
                    " jumlah proses (%d).\n", total_data, size);
            fprintf(stderr, "         Menambahkan padding agar data bisa dibagi rata.\n");

            /* Padding: tambah data dummy (0.0) agar habis dibagi */
            int padded_size = ((total_data / size) + 1) * size;
            float *padded = (float *)calloc(padded_size, sizeof(float));
            memcpy(padded, all_nilai, total_data * sizeof(float));
            /* Sisa diisi -1.0 sebagai marker (akan di-handle saat perhitungan) */
            for (int i = total_data; i < padded_size; i++) {
                padded[i] = -1.0f;  /* Marker data padding */
            }
            free(all_nilai);
            all_nilai = padded;

            /* Simpan jumlah data asli, update total untuk scatter */
            printf("[Master] Data di-pad dari %d menjadi %d elemen.\n",
                   total_data, padded_size);
            /* total_data tetap menyimpan jumlah data ASLI untuk perhitungan rata-rata */
        }
    }

    /* --------------------------------------------------------
     *  3. BROADCAST JUMLAH DATA KE SEMUA PROSES
     * --------------------------------------------------------
     *  Semua proses perlu tahu total_data agar bisa menghitung
     *  chunk_size masing-masing.
     * -------------------------------------------------------- */
    MPI_Bcast(&total_data, 1, MPI_INT, 0, MPI_COMM_WORLD);

    /* Hitung ukuran chunk per proses */
    int chunk_size = total_data / size;

    /* Jika ada sisa, Master sudah menambahkan padding */
    if (total_data % size != 0) {
        chunk_size = (total_data / size) + 1;
    }

    /* Alokasi memori untuk chunk lokal */
    float *local_nilai = (float *)malloc(chunk_size * sizeof(float));
    if (local_nilai == NULL) {
        fprintf(stderr, "[Rank %d] ERROR: Gagal alokasi memori lokal.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 1;
    }

    /* --------------------------------------------------------
     *  4. SCATTER: DISTRIBUSI DATA DARI MASTER KE SEMUA PROSES
     * --------------------------------------------------------
     *  MPI_Scatter membagi array all_nilai secara rata.
     *  Setiap proses menerima chunk_size elemen.
     * -------------------------------------------------------- */
    MPI_Scatter(all_nilai, chunk_size, MPI_FLOAT,
                local_nilai, chunk_size, MPI_FLOAT,
                0, MPI_COMM_WORLD);

    printf("[Rank %d @ %s] Menerima %d data untuk diproses.\n",
           rank, hostname, chunk_size);

    /* --------------------------------------------------------
     *  5. PERHITUNGAN LOKAL: MAX, MIN, SUM
     * --------------------------------------------------------
     *  Setiap proses menghitung dari chunk data-nya sendiri.
     *  Data padding (marker -1.0) diabaikan.
     * -------------------------------------------------------- */
    float local_max = -FLT_MAX;
    float local_min =  FLT_MAX;
    float local_sum =  0.0f;
    int   local_count = 0;

    for (int i = 0; i < chunk_size; i++) {
        /* Abaikan data padding (marker = -1.0) */
        if (local_nilai[i] < 0.0f) continue;

        if (local_nilai[i] > local_max) local_max = local_nilai[i];
        if (local_nilai[i] < local_min) local_min = local_nilai[i];
        local_sum += local_nilai[i];
        local_count++;
    }

    printf("[Rank %d @ %s] Hasil lokal -> Max: %.2f | Min: %.2f | Sum: %.2f (%d data valid)\n",
           rank, hostname, local_max, local_min, local_sum, local_count);

    /* --------------------------------------------------------
     *  6. REDUCE: KUMPULKAN HASIL GLOBAL KE MASTER
     * --------------------------------------------------------
     *  - MPI_MAX: ambil nilai terbesar dari semua local_max
     *  - MPI_MIN: ambil nilai terkecil dari semua local_min
     *  - MPI_SUM: jumlahkan semua local_sum
     * -------------------------------------------------------- */
    float global_max, global_min, global_sum;

    MPI_Reduce(&local_max, &global_max, 1, MPI_FLOAT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_min, &global_min, 1, MPI_FLOAT, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    /* --------------------------------------------------------
     *  7. MASTER: TAMPILKAN HASIL AKHIR
     * -------------------------------------------------------- */
    if (rank == 0) {
        float rata_rata = global_sum / (float)total_data;

        printf("\n");
        printf("===========================================\n");
        printf("HASIL ANALISIS NILAI MAHASISWA (MPI)\n");
        printf("===========================================\n");
        printf("Total Data Diproses : %d baris\n", total_data);
        printf("Nilai Tertinggi     : %.2f\n", global_max);
        printf("Nilai Terendah      : %.2f\n", global_min);
        printf("Rata-rata Kelas     : %.2f\n", rata_rata);
        printf("===========================================\n");
        printf("\n");

        /* Bersihkan memori Master */
        free(all_nilai);
    }

    /* Bersihkan memori lokal */
    free(local_nilai);

    /* --------------------------------------------------------
     *  8. FINALISASI MPI
     * -------------------------------------------------------- */
    MPI_Finalize();

    return 0;
}
