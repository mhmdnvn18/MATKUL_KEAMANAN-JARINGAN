## Ascon-128: Kriptografi Ringan untuk Keamanan IoT

Dalam ekosistem IoT, perangkat seperti sensor dan wearable memiliki keterbatasan daya dan komputasi. Oleh karena itu, diperlukan algoritma kriptografi ringan yang tetap kuat dalam menjaga kerahasiaan, integritas, dan keaslian data. 

Ascon-128 adalah algoritma **Authenticated Encryption with Associated Data (AEAD)** yang dipilih oleh **NIST (2023)** sebagai standar global untuk kriptografi ringan. Cocok untuk mikrokontroler kecil dan jaringan IoT karena:

- **🔒 Keamanan**: Tahan terhadap serangan fisik (*side-channel, fault injection*) dan kriptanalisis modern.
- **⚡ Efisiensi Komputasi**: Berjalan optimal pada perangkat terbatas daya dan memori.
- **✅ Autentikasi Data**: Mencegah manipulasi dan pemalsuan selama transmisi data.

Dengan Ascon-128, sistem IoT memperoleh lapisan keamanan terintegrasi dalam proses komunikasi data. Setiap paket tidak hanya dienkripsi, tetapi juga divalidasi keasliannya, menjadikannya ideal untuk aplikasi yang sensitif terhadap integritas data seperti monitoring kesehatan, sensor industri, dan sistem otomatisasi.
