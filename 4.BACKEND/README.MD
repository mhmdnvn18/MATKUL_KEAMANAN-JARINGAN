# Dokumentasi Instalasi dan Penggunaan Docker Desktop

## Penjelasan Fitur Utama pada Docker Desktop

- **Containers**  
  Container adalah instance dari image yang berjalan secara terisolasi. Container menjalankan aplikasi atau service, dan dapat dimulai, dihentikan, dihapus, atau direstart melalui Docker Desktop.

- **Images**  
  Image adalah template read-only yang berisi sistem file, dependensi, dan aplikasi yang dibutuhkan untuk menjalankan container. Anda dapat menarik (pull) image dari Docker Hub atau membuat image sendiri.

- **Volumes**  
  Volume digunakan untuk menyimpan data secara persisten di luar lifecycle container. Dengan volume, data tetap ada meskipun container dihapus atau dibuat ulang.

- **Builds**  
  Build adalah proses membuat image baru dari Dockerfile. Anda dapat melakukan build image custom sesuai kebutuhan aplikasi melalui menu "Builds" atau perintah `docker build`.

- **Docker Hub**  
  Docker Hub adalah registry online tempat berbagi dan mengelola image Docker. Anda dapat mencari, menarik, dan mengunggah image ke Docker Hub langsung dari Docker Desktop.

- **Docker Scout**  
  Docker Scout adalah fitur untuk menganalisis keamanan dan kerentanan pada image Docker. Scout memberikan insight dan rekomendasi untuk meningkatkan keamanan image.

- **Extensions**  
  Extensions adalah fitur tambahan yang dapat diinstal pada Docker Desktop untuk memperluas fungsionalitas, seperti integrasi dengan tools monitoring, database, atau workflow pengembangan lainnya.

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