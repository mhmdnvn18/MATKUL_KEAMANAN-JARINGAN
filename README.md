# MATKUL KEAMANAN JARINGAN

Repositori ini berisi materi dan contoh praktikum untuk mata kuliah Keamanan Jaringan.

## Langkah Percobaan
1. Buka, pelajari dan praktikan secara langsung menggunakan perangkat anda sesuai dengan langkah-langkah pada folder `1.BASIC` 
- [LINK](https://github.com/mhmdnvn18/MATKUL_KEAMANAN-JARINGAN/tree/main/1.BASIC)

2. Jika sudah berhasil, silahkan buka, pelajari dan praktikan secara langsung menggunakan perangkat anda sesuai dengan langkah-langkah pada folder `2.BASIC+WIFI+WEBSITE` 
- [LINK](https://github.com/mhmdnvn18/MATKUL_KEAMANAN-JARINGAN/tree/main/2.BASIC%2BWIFI%2BWEBSITE)

## Struktur Folder

- **1.BASIC**  
  Berisi materi dasar dan contoh implementasi sistem monitoring kesehatan berbasis ESP32.  
  Subfolder penting:
  - `NODE.RAW`: Kode untuk ESP32 Node yang membaca sensor suhu (MLX90614) dan sensor detak jantung/SpO2 (MAX30105), lalu mengirim data via BLE.
  - `GATEWAY.RAW`: Kode untuk ESP32 Gateway yang menerima data dari Node via BLE, membaca sensor gerak (MPU6500), dan GPS (Neo-6M), serta menampilkan data ke serial monitor.
  - `GATEWAY.NimBLE`: Alternatif implementasi Gateway menggunakan library NimBLE.
  - `GATEWAY.DUMMY`: Contoh gateway dummy untuk pengujian.

- **2.BASIC+WIFI+WEBSITE**  
  Berisi pengembangan lanjutan dengan integrasi WiFi dan dashboard website:
  - Kode ESP32 untuk mengirim data ke database (Supabase) via WiFi.
  - Dashboard website (HTML/JS) untuk menampilkan data kesehatan secara real-time.
  - Contoh file konfigurasi deploy ke Vercel dan skema database.

- **LIBRARY**  
  Kumpulan library eksternal yang digunakan (jika diperlukan secara offline).

- **Wiring Diagram.jpeg**  
  Gambar wiring diagram ESP32 Node dan Gateway beserta koneksi sensor (lihat juga README di masing-masing folder untuk detail wiring).

## Gambaran Sistem

Sistem terdiri dari dua ESP32:
- **Node**: Membaca data kesehatan (suhu, BPM, SpO2) dan mengirimkan via BLE.
- **Gateway**: Menerima data dari Node, membaca sensor gerak dan GPS, lalu menampilkan ke serial monitor atau mengirim ke cloud/website.

Wiring diagram dapat dilihat pada file `Wiring Diagram.jpeg`.

## Referensi & Dokumentasi

- Penjelasan detail dan langkah penggunaan tiap folder dapat dilihat pada README di masing-masing subfolder.
- Untuk pengujian UUID BLE, gunakan [uuidgenerator.net](https://www.uuidgenerator.net/).
- Untuk deploy dashboard website, ikuti petunjuk pada `2.BASIC+WIFI+WEBSITE/README.md`.