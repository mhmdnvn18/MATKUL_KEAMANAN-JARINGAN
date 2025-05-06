# MATKUL KEAMANAN JARINGAN

Repositori ini berisi materi, kode, dan panduan praktikum untuk mata kuliah Keamanan Jaringan, khususnya implementasi sistem monitoring kesehatan berbasis ESP32.

---

## Langkah Praktikum

1. **Pelajari dan praktikkan folder `1.BASIC`**  
   Mulai dari dasar: instalasi library, wiring, dan pengujian sensor serta komunikasi BLE antara Node dan Gateway.  
   [Lihat Panduan & Kode →](https://github.com/mhmdnvn18/MATKUL_KEAMANAN-JARINGAN/tree/main/1.BASIC)

2. **Lanjutkan ke folder `2.BASIC+WIFI+WEBSITE`**  
   Setelah berhasil dengan BLE, lanjutkan ke integrasi WiFi, pengiriman data ke cloud (Supabase), dan visualisasi data di dashboard website.  
   [Lihat Panduan & Kode →](https://github.com/mhmdnvn18/MATKUL_KEAMANAN-JARINGAN/tree/main/2.BASIC%2BWIFI%2BWEBSITE)

---

## Struktur Folder

- **1.BASIC**  
  Materi dan kode dasar monitoring kesehatan berbasis ESP32:
  - `NODE.RAW`: Kode ESP32 Node (sensor suhu MLX90614 & MAX30105, BLE server).
  - `GATEWAY.RAW`: Kode ESP32 Gateway (BLE client, sensor gerak MPU6500, GPS Neo-6M).
  - `GATEWAY.NimBLE`: Alternatif Gateway dengan library NimBLE.
  - `GATEWAY.DUMMY`: Gateway dummy untuk simulasi/pengujian.

- **2.BASIC+WIFI+WEBSITE**  
  Pengembangan lanjut: integrasi WiFi, database Supabase, dan dashboard website.
  - Kode ESP32 untuk kirim data ke Supabase via WiFi.
  - Website dashboard real-time (HTML/JS).
  - File konfigurasi deploy (Vercel) & skema database.

- **LIBRARY**  
  Kumpulan library eksternal (jika dibutuhkan secara offline).

- **Wiring Diagram.jpeg**  
  Diagram koneksi hardware ESP32 Node & Gateway ke sensor.

---

## Gambaran Sistem

- **Node (ESP32):**  
  Membaca data kesehatan (suhu, BPM, SpO2) → kirim via BLE ke Gateway.
- **Gateway (ESP32):**  
  Terima data Node via BLE, baca sensor gerak & GPS → tampilkan ke serial monitor atau kirim ke cloud/website.

Lihat wiring pada file `Wiring Diagram.jpeg` dan detail langkah di README masing-masing folder.

---

## Referensi & Dokumentasi

- Panduan detail tiap langkah ada di README pada masing-masing subfolder.
- Untuk generate UUID BLE: [uuidgenerator.net](https://www.uuidgenerator.net/)
- Panduan deploy website: lihat `2.BASIC+WIFI+WEBSITE/README.md`

---

**Catatan:**  
Pastikan mengikuti urutan praktik dari folder 1 ke 2, dan wiring sesuai diagram agar sistem berjalan dengan baik.