# Health Monitoring Dashboard

## Deploy ke Vercel

Ikuti langkah berikut untuk melakukan deploy project ini ke Vercel:

1. **Fork/Clone Repo**
   - Fork atau clone repository ini ke akun GitHub Anda.

2. **Pastikan Struktur File**
   - File utama harus bernama `index.html`.
   - Pastikan semua asset (seperti `heart-pulse.svg`, `oxygen.svg`, `thermometer.svg`) ada di repository.

3. **Push ke GitHub**
   - Commit dan push semua file ke repository GitHub Anda.

4. **Deploy ke Vercel**
   - Buka [https://vercel.com/import/git](https://vercel.com/import/git).
   - Login dengan akun Vercel Anda.
   - Pilih repository GitHub yang sudah berisi project ini.
   - Pada form konfigurasi deploy, isi sebagai berikut:
     - **Project Name:** `matkul-keamanan-jaringan-website`
     - **Framework Preset:** Pilih **Other**
     - **Root Directory:** `./`
     - **Build Command:** *(kosongkan, karena project ini statis dan tidak perlu build)*
     - **Output Directory:** `.` (atau `public` jika Anda meletakkan file di folder `public`)
     - **Install Command:** *(kosongkan, tidak perlu install dependency)*
   - Jika ingin menambahkan environment variable:
     - **Key:** `EXAMPLE_NAME`
     - **Value:** `I9JU23NF394R6HH`
   - Klik **Deploy**.

5. **Akses Website**
   - Setelah deploy selesai, Anda akan mendapatkan URL publik dari Vercel.

---

**Catatan:**  
- Untuk project HTML statis, Build Command dan Install Command bisa dikosongkan.
- Output Directory diisi `.` jika file `index.html` ada di root, atau `public` jika file ada di folder `public`.
- Environment variable hanya perlu diisi jika memang digunakan di project.
