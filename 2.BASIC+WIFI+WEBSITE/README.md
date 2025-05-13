# Health Monitoring Dashboard

## Persiapan Akun

### 1. Membuat Akun GitHub
1. Buka [https://github.com/join](https://github.com/join).
2. Isi username, email, dan password sesuai instruksi.
3. Verifikasi email Anda melalui link yang dikirim ke email.
4. Login ke GitHub.

### 2. Membuat Akun Vercel
1. Buka [https://vercel.com/signup](https://vercel.com/signup).
2. Pilih metode pendaftaran (disarankan menggunakan akun GitHub).
3. Ikuti instruksi hingga selesai.
4. Login ke Vercel.

---

## Deploy ke Vercel

1. **Fork/Clone Repo**
   - Fork atau clone repository ini ke akun GitHub Anda.

2. **Struktur File**
   - File utama harus bernama `index.html` dan berada di root folder.
   - Semua icon sudah menggunakan Font Awesome (CDN), tidak perlu file SVG icon terpisah.

3. **Push ke GitHub**
   - Commit dan push semua file ke repository GitHub Anda.

4. **Deploy ke Vercel**
   - Buka [https://vercel.com/import/git](https://vercel.com/import/git).
   - Login dengan akun Vercel Anda.
   - Pilih repository GitHub yang sudah berisi project ini.
   - Pada form konfigurasi deploy, isi:
     - **Project Name:** `ISI SESUAI NAMA KALIAN`
     - **Framework Preset:** Pilih **Other**
     - **Root Directory:** `./`
     - **Build Command:** *(kosongkan)*
     - **Output Directory:** `.` (atau `public` jika file ada di folder `public`)
     - **Install Command:** *(kosongkan)*
   - (Opsional) Tambahkan environment variable jika diperlukan.
   - Klik **Deploy**.

5. **Akses Website**
   - Setelah deploy selesai, Anda akan mendapatkan URL publik dari Vercel.

---

## Catatan Icon

- Semua icon pada dashboard menggunakan [Font Awesome](https://fontawesome.com/) via CDN.
- Tidak perlu mengupload file SVG icon (`heart-pulse.svg`, `oxygen.svg`, `thermometer.svg`).
- Pastikan koneksi internet untuk memuat CDN Font Awesome.

---

## Format Data ke Supabase

Pastikan data yang dikirim ke Supabase (dari ESP32/Arduino) memiliki format JSON pada field `sensor_value`, **bukan string**.  
**Tambahkan juga field `device_id` untuk identifikasi perangkat ESP32.**

Contoh payload yang benar:
```json
{
  "device_id": "ESP32-ROOM1",
  "sensor_value": {
    "heartRate": 80,
    "validHeartRate": 1,
    "spo2": 98,
    "validSpO2": 1,
    "temp": 36.7
  }
}
```
Jangan gunakan escape karakter pada JSON.

**Catatan:**  
- Ganti `device_id` sesuai nama unik ESP32 Anda, misal: `ESP32-ROOM1`, `ESP32-ICU`, dll.
- Pada kode ESP32, tambahkan variabel `device_id` dan sertakan pada payload.

---

## Troubleshooting

- **Data tidak muncul di dashboard:**  
  Pastikan ESP32 mengirim data dengan format JSON yang benar ke endpoint Supabase.
- **Chart tidak tampil:**  
  Pastikan ada data pada tabel `sensor_data` di Supabase.
- **Koneksi gagal:**  
  Cek koneksi WiFi pada ESP32 dan pastikan API Key Supabase benar.

---

**Catatan:**  
- Untuk project HTML statis, Build Command dan Install Command bisa dikosongkan.
- Output Directory diisi `.` jika file `index.html` ada di root, atau `public` jika file ada di folder `public`.
- Environment variable hanya perlu diisi jika memang digunakan di project.
