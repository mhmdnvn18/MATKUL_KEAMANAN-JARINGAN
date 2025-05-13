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
   Fork atau clone repository ini ke akun GitHub Anda.

2. **Struktur File**  
   File utama harus bernama `index.html` dan berada di root folder.  
   Semua icon sudah menggunakan Font Awesome (CDN), tidak perlu file SVG icon terpisah.

3. **Push ke GitHub**  
   Commit dan push semua file ke repository GitHub Anda.

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
   Setelah deploy selesai, Anda akan mendapatkan URL publik dari Vercel.

---

## Cara Menggunakan Supabase

1. **Buat Project di Supabase**  
   - Buka [https://app.supabase.com/](https://app.supabase.com/) dan login.
   - Klik **New Project** dan isi detail project (nama, password, region).
   - Tunggu hingga project selesai dibuat.

2. **Buat Tabel sensor_data**  
   - Masuk ke menu **SQL Editor** di sidebar kiri.
   - Masukkan perintah SQL berikut untuk membuat tabel dan policy:
     ```sql
     DROP TABLE IF EXISTS sensor_data CASCADE;

     CREATE TABLE IF NOT EXISTS sensor_data (
         id SERIAL PRIMARY KEY,
         device_id TEXT NOT NULL,
         sensor_value JSONB NOT NULL,
         created_at TIMESTAMP WITH TIME ZONE DEFAULT now()
     );

     ALTER TABLE sensor_data ENABLE ROW LEVEL SECURITY;

     CREATE POLICY "Enable read access for all users" 
       ON sensor_data 
       FOR SELECT 
       USING (true);

     CREATE POLICY "Enable insert for all users"
       ON sensor_data
       FOR INSERT
       WITH CHECK (true);

     -- Dummy data insert example
     INSERT INTO sensor_data (device_id, sensor_value)
     VALUES
       ('ESP32-ROOM1', '{"heartRate":78,"spo2":98,"temp":36.5}'),
       ('ESP32-ROOM2', '{"heartRate":85,"spo2":97,"temp":36.8}'),
       ('ESP32-ROOM1', '{"heartRate":92,"spo2":96,"temp":37.1}'),
       ('ESP32-ROOM3', '{"heartRate":60,"spo2":99,"temp":36.2}');
     ```

3. **Ambil API URL dan API Key**  
   - Masuk ke menu **Project Settings > API**.
   - Salin **Project URL** dan **anon public API key**.
   - Ganti konfigurasi di kode ESP32/website sesuai URL dan API key project Anda.

4. **Kirim Data dari ESP32**  
   - Pastikan payload JSON yang dikirim sesuai format pada contoh di bawah.
   - Data akan otomatis masuk ke tabel `sensor_data` di Supabase.

5. **Lihat Data**  
   - Data yang masuk bisa dilihat di dashboard Supabase (Table Editor) atau pada website dashboard ini.

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
