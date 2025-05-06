# MATKUL_KEAMANAN-JARINGAN

## Topologi Sistem (Flowchart)

```mermaid
flowchart TD
    subgraph NODE
        A[Sensor MLX90614] -- I2C --> B[ESP32 Node]
        B1[Sensor MAX30105] -- I2C --> B
    end
    B -- BLE --> C
    subgraph GATEWAY
        C[ESP32 Gateway]
        E[MPU6500] -- I2C --> C
        F[Neo-6M GPS] -- UART --> C
    end
    C -- Serial --> D[PC / Serial Monitor]
```

## A. Langkah-Langkah Penggunaan
### 1. Merangkai wiring sesuai gambar
![Wiring Diagram](0.Wiring-Diagram.png)

### 2. Menginstal library yang diperlukan:
#### 2.1 ESP32 Node
- SparkFun MAX3010x Pulse and Proximity Sensor Library by SparkFun
- Adafruit MLX90614 by Adafruit

#### 2.2 ESP32 Gateway
- MPU9250_WE by Wolfgang Ewald
- TinyGPSPlus by Mikal Hart

### 3. Mencoba setiap sensor dengan kode example library-nya masing-masing:
#### 3.1 ESP32 Node
- SparkFun MAX3010x Pulse and Proximity
  ![Sensor-MAX3010X-Sparkfun](1.Sensor-MAX3010X-Sparkfun.png)

- Adafruit MLX90614 by Adafruit
  ![Sensor-MLX90614](2.Sensor-MLX90614.png)

#### 3.2 ESP32 Gateway
- MPU9250_WE by Wolfgang Ewald
  ![Sensor-MPU9250_WE](3.Sensor-MPU9250.png)
  
- TinyGPSPlus by Mikal Hart (untuk Neo-6M GPS)
  ![Neo-6M_TinyGPSPlus](4.Neo-6M_TinyGPSPlus.png)

### 4. Salin kode Node dan Gateway ke Arduino IDE
Jika sudah berhasil membaca/muncul di serial monitor dan tidak ada error, lanjut ke GitHub, copy kode Node dan Gateway ke Arduino IDE.  
[Link GitHub](https://github.com/mhmdnvn18/MATKUL_KEAMANAN-JARINGAN/tree/main/1.BASIC)

![Github](5.Github.png)

### 5. Mengubah UUID
Sebelum upload, ubah dulu kedua UUID pada Node dan Gateway menggunakan website UUID Generator.  
[WEBSITE UUID GENERATOR](https://www.uuidgenerator.net/)  
![UUID GENERATOR](8.UUID-Generator.png)

Generate dan tambahkan ke dalam kode, lalu samakan UUID antara kode Node dan Gateway.
- KODE UUID GATEWAY  
  ![KODE UUID GATEWAY](6.UUID-Gateway.png)
- KODE UUID NODE  
  ![KODE UUID NODE](7.UUID-Node.png)

### 6. Hasil yang diharapkan
Berikut hasil dari kedua kode pada serial monitor jika semua konfigurasi benar

#### 6.1 ESP32 Node
![Hasil pada serial monitor Node](9.Hasil-Node-1.png)
![Hasil pada serial monitor Node](10.Hasil-Node-2.png)

#### 6.2 ESP32 Gateway
![Hasil pada serial monitor Gateway](11.Hasil-Gateway.png)

### 7. CATATAN
- Selalu ingat nomor port untuk ESP32 Node dan ESP32 Gateway dan setiap upload program (kode)
- Restart ESP32 dengan menekan tombol EN pada board ESP32

## B. Penjelasan Program Arduino

### 1. NODE (NODE.ino)
**NODE** adalah program untuk ESP32 yang berfungsi sebagai node sensor kesehatan.  
Fitur utama:
- Membaca suhu tubuh menggunakan sensor **MLX90614**.
- Mengukur detak jantung (BPM) dan kadar oksigen darah (SpO2) menggunakan sensor **MAX30105**.
- Menggunakan dua jalur I2C (`Wire` dan `Wire1`) untuk menghubungkan kedua sensor secara bersamaan.
- Mengirimkan data hasil pengukuran (suhu, BPM, SpO2) ke perangkat lain melalui koneksi **Bluetooth Low Energy (BLE)** dengan UUID service dan characteristic tertentu.
- Data dikirim secara periodik jika jari terdeteksi pada sensor MAX30105.

---

### 2. GATEWAY (GATEWAY.ino)
**GATEWAY** adalah program untuk ESP32 yang berfungsi sebagai penerima data dari NODE dan juga sebagai pengumpul data lingkungan.  
Fitur utama:
- Berperan sebagai **BLE client** yang mencari dan terhubung ke NODE (BLE server) berdasarkan UUID service yang sama.
- Menerima data kesehatan (suhu, BPM, SpO2) dari NODE melalui BLE notification.
- Membaca data sensor gerak (**MPU6500**) untuk mendapatkan nilai akselerasi dan gyroscope.
- Membaca data lokasi (latitude, longitude, altitude) dan jumlah satelit dari modul **GPS Neo-6M**.
- Menampilkan seluruh data (dari NODE, sensor gerak, dan GPS) ke serial monitor untuk pemantauan.

---

## C. Alur Kerja Singkat
1. **NODE** mengukur data kesehatan dan mengirimkannya via BLE.
2. **GATEWAY** mencari NODE, terhubung, dan menerima data kesehatan.
3. **GATEWAY** juga membaca data sensor gerak dan GPS, lalu menampilkan semua data ke serial monitor.

---

### Catatan
- Pastikan kedua ESP32 sudah di-flash dengan program masing-masing dan berada dalam jangkauan BLE.
- Library yang digunakan: `Wire`, `MAX30105`, `Adafruit_MLX90614`, `BLE`, `MPU6500_WE`, `TinyGPSPlus`, dsb.
- Koneksi hardware harus sesuai dengan pin yang tertera di masing-masing program.

