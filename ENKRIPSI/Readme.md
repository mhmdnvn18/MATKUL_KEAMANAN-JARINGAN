# 🔐 Kriptografi Ringan untuk IoT dengan Ascon-128
Dalam ekosistem Internet of Things (IoT) yang terus berkembang, keamanan jaringan telah menjadi tantangan krusial. Perangkat IoT, seperti sensor, wearable device, dan aktuator, umumnya beroperasi dengan keterbatasan daya, memori, dan kemampuan komputasi. Oleh karena itu, penerapan algoritma kriptografi ringan menjadi sangat esensial untuk menjaga kerahasiaan dan integritas data tanpa membebani sumber daya sistem.

## ✨ Apa Itu Kriptografi Ringan?
Kriptografi ringan adalah cabang kriptografi yang secara spesifik dirancang untuk perangkat dengan sumber daya terbatas. Algoritma ini memprioritaskan efisiensi tinggi dalam aspek-aspek berikut:
- Konsumsi daya rendah: Memperpanjang masa pakai baterai perangkat IoT.
- Konsumsi memori kecil: Memungkinkan implementasi pada mikrokontroler dengan RAM terbatas.
- Kecepatan proses yang optimal: Memastikan transmisi data yang efisien tanpa latency yang signifikan.
- Ketahanan terhadap serangan fisik dan jaringan: Melindungi perangkat dari ancaman yang beragam.

Beberapa algoritma kriptografi ringan yang dikenal meliputi:
- Ascon-128
- PRESENT
- Speck/Simon
- GIFT
## 🛡️ Ascon-128: Standar Keamanan Jaringan IoT
Ascon-128 adalah algoritma Authenticated Encryption with Associated Data (AEAD) yang telah diakui dan dipilih oleh NIST sebagai standar global kriptografi ringan pada tahun 2023. Keistimewaan Ascon-128 terletak pada kemampuannya menggabungkan fungsi enkripsi dan autentikasi dalam satu proses yang efisien. Ini berarti data yang dikirim tidak hanya terjaga kerahasiaannya, tetapi juga terjamin keaslian dan integritasnya selama transmisi.
### 🔍 Keunggulan Ascon-128 untuk Keamanan Jaringan IoT:
- Efisiensi Tinggi: Dirancang untuk beroperasi secara optimal pada mikrokontroler dan perangkat hemat daya, memastikan bahwa overhead komputasi dan energi untuk keamanan tetap minimal.
- Autentikasi Data Bawaan: Fitur AEAD-nya secara inheren menyediakan autentikasi pesan, yang krusial untuk keamanan jaringan. Ini mencegah pihak yang tidak berwenang memanipulasi atau memalsukan data saat transit, menjaga integritas komunikasi end-to-end.
- Tahan Serangan Fisik: Mampu menahan berbagai jenis serangan fisik, termasuk side-channel attacks (serangan yang menganalisis informasi seperti konsumsi daya atau waktu eksekusi) dan fault injection (serangan yang menginduksi kesalahan pada perangkat keras), yang seringkali menjadi ancaman serius di lingkungan IoT yang rentan.
- Melindungi data dari intersepsi dan modifikasi: Enkripsi memastikan kerahasiaan data, sementara autentikasi mencegah perubahan data oleh penyerang.
- Menjamin integritas pesan melalui tag autentikasi: Setiap pesan dilengkapi dengan tag unik yang memverifikasi bahwa data tidak diubah selama transmisi. Jika ada modifikasi, tag tidak akan cocok, dan pesan akan ditolak.
- Mengurangi risiko serangan replay dan DDoS: Meskipun Ascon-128 sendiri tidak mencegah serangan replay atau DDoS secara langsung, kombinasinya dengan protokol dan praktik jaringan yang tepat (seperti penggunaan timestamp atau nomor urut dengan MQTT) akan secara signifikan mengurangi kerentanan terhadap serangan-serangan ini.
