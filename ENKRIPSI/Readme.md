# 🔐Kriptografi Ringan untuk IoT dengan Ascon-128
Dalam ekosistem *Internet of Things* (IoT), keamanan jaringan menjadi tantangan utama. Perangkat IoT seperti sensor, *wearable device* dan aktuator umumnya memiliki keterbatasan daya, memori, dan komputasi. Oleh karena itu, diperlukan algoritma kriptografi ringan yang mampu menjaga kerahasiaan dan integritas data tanpa membebani sistem.

## ✨Apa Itu Kriptografi Ringan?
Kriptografi ringan adalah pendekatan kriptografi yang dirancang khusus untuk perangkat dengan sumber daya dan komputasi yang terbatas. Algoritma ini mengutamakan efisiensi dalam hal:
- Konsumsi daya rendah
- Jejak memori kecil
- Kecepatan proses yang optimal
- Ketahanan terhadap serangan fisik dan jaringan

Beberapa algoritma ringan yang dikenal:
- Ascon-128
- PRESENT
- Speck/Simon
- GIFT
## 🛡️Ascon-128: Standar Keamanan Jaringan IoT
Ascon-128 adalah algoritma *Authenticated Encryption with Associated Data* (AEAD) yang dipilih oleh NIST sebagai standar global kriptografi ringan pada tahun 2023. Algoritma ini menggabungkan enkripsi dan autentikasi dalam satu proses, sehingga data yang dikirim tidak hanya rahasia, tetapi juga terjamin keasliannya.

### 🔍 Keunggulan Ascon-128 untuk IoT:
- Efisiensi tinggi: Cocok untuk mikrokontroler dan perangkat hemat daya
- Autentikasi data: Mencegah manipulasi dan pemalsuan selama transmisi
- Tahan serangan fisik: Termasuk *side-channel* dan *fault injection*

## 🔒 Fokus Keamanan Jaringan
Ascon-128 sangat efektif dalam mengamankan komunikasi dalam jaringan IoT. Dengan *overhead* rendah dan performa tinggi, algoritma ini mampu:
- Melindungi data dari intersepsi dan modifikasi
- Menjamin integritas pesan melalui tag autentikasi
- Mengurangi risiko serangan *replay* dan DDoS jika dikombinasikan dengan protokol seperti MQTT
