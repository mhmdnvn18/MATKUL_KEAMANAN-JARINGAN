# Dokumentasi Instalasi dan Penggunaan Docker Desktop

## Penjelasan Fitur Utama pada Docker Desktop

- **Containers**  
  Container adalah instance dari image yang berjalan secara terisolasi. Container menjalankan aplikasi atau service, dan dapat dimulai, dihentikan, dihapus, atau direstart melalui Docker Desktop.  
  *Contoh:* Container `nginx` yang berjalan di port 8080:
  ```
  docker run -d -p 8080:80 --name webserver nginx
  ```

- **Images**  
  Image adalah template read-only yang berisi sistem file, dependensi, dan aplikasi yang dibutuhkan untuk menjalankan container. Anda dapat menarik (pull) image dari Docker Hub atau membuat image sendiri.  
  *Contoh:* Menarik image Node-RED:
  ```
  docker pull nodered/node-red
  ```

- **Volumes**  
  Volume digunakan untuk menyimpan data secara persisten di luar lifecycle container. Dengan volume, data tetap ada meskipun container dihapus atau dibuat ulang.  
  *Contoh:* Membuat volume dan menggunakannya pada container:
  ```
  docker volume create data-volume
  docker run -d -v data-volume:/data --name nodered nodered/node-red
  ```

- **Builds**  
  Build adalah proses membuat image baru dari Dockerfile. Anda dapat melakukan build image custom sesuai kebutuhan aplikasi melalui menu "Builds" atau perintah `docker build`.  
  *Contoh:* Build image dari Dockerfile di folder saat ini:
  ```
  docker build -t my-custom-image .
  ```

- **Docker Hub**  
  Docker Hub adalah registry online tempat berbagi dan mengelola image Docker. Anda dapat mencari, menarik, dan mengunggah image ke Docker Hub langsung dari Docker Desktop.  
  *Contoh:* Menarik image Mosquitto dari Docker Hub:
  ```
  docker pull eclipse-mosquitto
  ```

- **Docker Scout**  
  Docker Scout adalah fitur untuk menganalisis keamanan dan kerentanan pada image Docker. Scout memberikan insight dan rekomendasi untuk meningkatkan keamanan image.  
  *Contoh:* Analisis keamanan image dapat dilakukan melalui menu "Scout" di Docker Desktop.

- **Extensions**  
  Extensions adalah fitur tambahan yang dapat diinstal pada Docker Desktop untuk memperluas fungsionalitas, seperti integrasi dengan tools monitoring, database, atau workflow pengembangan lainnya.  
  *Contoh:* Install extension "MongoDB" melalui menu Extensions di Docker Desktop.

## Instalasi Docker Desktop

1. **Download Docker Desktop**
   - Kunjungi [https://www.docker.com/products/docker-desktop/](https://www.docker.com/products/docker-desktop/)
   - Pilih versi sesuai sistem operasi (Windows/Mac).

2. **Install Docker Desktop**
   - Jalankan installer yang telah diunduh.
   - Ikuti instruksi instalasi hingga selesai.
   - Setelah instalasi, restart komputer jika diminta.

3. **Verifikasi Instalasi**
   - Buka Command Prompt atau PowerShell.
   - Jalankan perintah berikut:
     ```
     docker --version
     ```
   - Jika berhasil, akan muncul versi Docker yang terpasang.

## Apakah Perlu Menginstal WSL?

- **Untuk pengguna Windows:**  
  Docker Desktop pada Windows membutuhkan WSL 2 (Windows Subsystem for Linux versi 2) sebagai backend default untuk menjalankan container Linux.  
  Jika WSL 2 belum terinstal, Docker Desktop akan memandu proses instalasinya saat pertama kali dijalankan.  
  **Jadi, ya—Anda perlu menginstal WSL 2 agar Docker Desktop dapat berjalan optimal di Windows.**

- **Untuk pengguna Mac:**  
  Tidak perlu menginstal WSL. Docker Desktop berjalan langsung di atas sistem operasi Mac.

## Langkah-langkah Instalasi WSL 2 untuk Docker Desktop pada Windows 11

1. **Buka PowerShell sebagai Administrator**
   - Klik kanan pada Start Menu, pilih **Windows Terminal (Admin)** atau **PowerShell (Admin)**.

2. **Install WSL dan Distro Linux (misal Ubuntu)**
   - Jalankan perintah berikut:
     ```
     wsl --install
     ```
   - Perintah ini akan menginstal WSL 2 dan Ubuntu secara otomatis.
   - Jika diminta, restart komputer setelah proses instalasi selesai.

3. **Pastikan Versi WSL adalah 2**
   - Setelah restart, cek versi WSL dengan perintah:
     ```
     wsl --list --verbose
     ```
   - Pastikan distro Linux (misal Ubuntu) menggunakan versi 2 pada kolom "VERSION".
   - Jika belum, ubah ke WSL 2 dengan perintah:
     ```
     wsl --set-version <nama-distro> 2
     ```
     Contoh:
     ```
     wsl --set-version Ubuntu 2
     ```

4. **Set WSL 2 sebagai Default**
   - Jalankan perintah berikut:
     ```
     wsl --set-default-version 2
     ```

5. **Update Kernel WSL (Jika Diperlukan)**
   - Jika muncul pesan untuk update kernel, download dan install dari:
     [https://aka.ms/wsl2kernel](https://aka.ms/wsl2kernel)

6. **Cek Instalasi**
   - Jalankan:
     ```
     wsl
     ```
   - Jika masuk ke terminal Linux, WSL sudah terinstal dengan benar.

7. **Lanjutkan Instalasi Docker Desktop**
   - Setelah WSL 2 aktif, install dan jalankan Docker Desktop seperti biasa.
   - Pada pengaturan Docker Desktop, pastikan backend menggunakan WSL 2.

> **Catatan:**  
> Windows 11 biasanya sudah mendukung WSL 2 secara default, sehingga proses instalasi lebih mudah dibandingkan versi Windows sebelumnya.

## Penggunaan Dasar Docker Desktop

1. **Menjalankan Docker Desktop**
   - Buka aplikasi Docker Desktop dari Start Menu.
   - Pastikan status Docker “Running”.

2. **Menggunakan GUI Docker Desktop**
   - Setelah aplikasi berjalan, Anda dapat menggunakan antarmuka grafis (GUI) Docker Desktop untuk:
     1. **Melihat Daftar Images dan Containers**
        - Klik tab "Containers" untuk melihat, menjalankan, menghentikan, atau menghapus container.
        - Klik tab "Images" untuk melihat dan mengelola image Docker.
     2. **Menarik (Pull) Image Baru**
        - Buka tab "Images".
        - Klik tombol "Pull" atau "Add Image".
        - Masukkan nama image (misal: `nginx`, `nodered/node-red`, atau `eclipse-mosquitto`) lalu klik "Pull".
     3. **Menjalankan Container**
        - Setelah image tersedia, klik tombol "Run" pada image yang diinginkan.
        - Atur port mapping (misal: `8080:80` untuk nginx, `1883:1883` untuk mosquitto).
        - Beri nama container jika perlu, lalu klik "Run".
     4. **Mengelola Container**
        - Gunakan tombol "Start", "Stop", "Restart", atau "Delete" pada daftar container sesuai kebutuhan.
     5. **Melihat Log dan Resource**
        - Klik nama container untuk melihat log, statistik penggunaan CPU/memori, dan pengaturan lainnya.

## Langkah-langkah Instalasi Node-RED dan MQTT Mosquitto dengan Docker Desktop

1. **Pastikan Docker Desktop sudah terinstal dan berjalan.**

2. **Menarik (Pull) Image Node-RED dan Mosquitto**
   - Buka Docker Desktop.
   - Pilih tab **Images**.
   - Klik tombol **Pull** atau **Add Image**.
   - Masukkan nama image berikut satu per satu, lalu klik **Pull**:
     - `nodered/node-red`
     - `eclipse-mosquitto`

3. **Menjalankan Container Node-RED**
   - Setelah image `nodered/node-red` tersedia di tab **Images**, klik tombol **Run** pada image tersebut.
   - Atur port mapping:
     - Host: `1880`, Container: `1880`
   - Beri nama container, misal: `nodered`
   - Klik **Run**.
   - Buka browser dan akses `http://localhost:1880` untuk menggunakan Node-RED.

4. **Menjalankan Container Mosquitto**
   - Setelah image `eclipse-mosquitto` tersedia di tab **Images**, klik tombol **Run** pada image tersebut.
   - Atur port mapping:
     - Host: `1883`, Container: `1883`
     - Host: `9001`, Container: `9001`
   - Beri nama container, misal: `mosquitto`
   - Klik **Run**.
   - MQTT broker akan aktif di `localhost:1883`.

5. **Cek Status Container**
   - Buka tab **Containers** di Docker Desktop.
   - Pastikan container `nodered` dan `mosquitto` berstatus **Running**.

6. **Menghubungkan Node-RED ke Mosquitto**
   - Buka editor Node-RED di browser (`http://localhost:1880`).
   - Tambahkan node `mqtt in` atau `mqtt out`.
   - Pada konfigurasi server MQTT, masukkan:
     - Server: `localhost`
     - Port: `1883`
   - Deploy flow untuk mulai berkomunikasi dengan broker MQTT.

7. **Testing**
   - Gunakan node `inject` dan `debug` di Node-RED untuk mengirim dan menerima pesan MQTT.
   - Anda juga dapat menggunakan aplikasi MQTT client lain (misal MQTT Explorer) untuk testing.

## Penggunaan Dasar Docker Desktop

3. **Menarik (Pull) Gambar Docker**
   - Contoh menarik gambar `nginx`:
     ```
     docker pull nginx
     ```
   - Contoh menarik gambar `node-red`:
     ```
     docker pull nodered/node-red
     ```
   - Contoh menarik gambar MQTT (Eclipse Mosquitto):
     ```
     docker pull eclipse-mosquitto
     ```

4. **Menjalankan Container**
   - Contoh menjalankan container `nginx`:
     ```
     docker run -d -p 8080:80 --name webserver nginx
     ```
   - Akses aplikasi di browser: `http://localhost:8080`
   - Contoh menjalankan container MQTT (Eclipse Mosquitto):
     ```
     docker run -d -p 1883:1883 -p 9001:9001 --name mosquitto eclipse-mosquitto
     ```

5. **Langkah-langkah Menggunakan Node-RED dan Mosquitto**
   1. **Menjalankan Node-RED**
      - Jalankan container Node-RED:
        ```
        docker run -d -p 1880:1880 --name nodered nodered/node-red
        ```
      - Buka browser dan akses `http://localhost:1880` untuk membuka editor Node-RED.
   2. **Menjalankan Mosquitto**
      - Pastikan container Mosquitto sudah berjalan (lihat langkah sebelumnya).
      - MQTT broker akan tersedia di `localhost:1883`.
   3. **Menghubungkan Node-RED ke Mosquitto**
      - Di editor Node-RED, tambahkan node `mqtt in` atau `mqtt out`.
      - Pada konfigurasi server MQTT, masukkan:
        - Server: `localhost`
        - Port: `1883`
      - Deploy flow untuk mulai berkomunikasi dengan broker MQTT.
   4. **Testing**
      - Gunakan node `inject` dan `debug` di Node-RED untuk mengirim dan menerima pesan melalui MQTT.
      - Anda juga bisa menggunakan aplikasi MQTT client lain (misal MQTT Explorer) untuk testing.

6. **Melihat Daftar Container**
   - Jalankan perintah berikut untuk melihat daftar container yang sedang berjalan:
     ```
     docker ps
     ```
   - Untuk melihat semua container (termasuk yang berhenti), jalankan:
     ```
     docker ps -a
     ```