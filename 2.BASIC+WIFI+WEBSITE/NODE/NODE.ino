// filepath: c:\.Github\MATKUL_KEAMANAN-JARINGAN\NODE\NODE.ino
#include <Wire.h> // Library I2C utama
#include <MAX30105.h> // Library sensor MAX30105 (detak jantung & SpO2)
#include <Adafruit_MLX90614.h> // Library sensor suhu MLX90614
#include "heartRate.h" // Library algoritma detak jantung
#include "spo2_algorithm.h" // Library algoritma SpO2
#include <BLEDevice.h> // Library BLE utama
#include <BLEUtils.h> // Utilitas BLE
#include <BLEServer.h> // Library BLE server

// I2C Bus
#define SDA_WIRE 21 // Pin SDA untuk I2C utama (MLX90614)
#define SCL_WIRE 22 // Pin SCL untuk I2C utama (MLX90614)
#define SDA_WIRE1 33 // Pin SDA untuk I2C kedua (MAX30105)
#define SCL_WIRE1 32 // Pin SCL untuk I2C kedua (MAX30105)

// BLE Setup
#define SERVICE_UUID        "89bc34b8-c3a1-4f22-82d9-00a2559bbcc0" // UUID service BLE
#define CHARACTERISTIC_UUID "cbef4b5c-fe06-41dc-ab84-105dbe7a722c" // UUID characteristic BLE

// Instance
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); // Membuat objek sensor suhu
MAX30105 particleSensor; // Membuat objek sensor MAX30105

// MAX30105 Buffer
#define BUFFER_LENGTH 100 // Panjang buffer data sensor
uint32_t irBuffer[BUFFER_LENGTH]; // Buffer data IR
uint32_t redBuffer[BUFFER_LENGTH]; // Buffer data RED
int32_t spo2; // Variabel hasil SpO2
int8_t validSPO2; // Status validasi SpO2
int32_t heartRate; // Variabel hasil detak jantung
int8_t validHeartRate; // Status validasi detak jantung
bool fingerDetected = false; // Status deteksi jari
byte readLED = 13; // Pin LED indikator pembacaan

// BLE Characteristics
BLECharacteristic *pCharacteristic; // Pointer ke characteristic BLE

void setup() {
  Serial.begin(115200); // Inisialisasi serial monitor
  delay(1000); // Delay untuk stabilisasi

  // I2C start
  Wire.begin(SDA_WIRE, SCL_WIRE); // Mulai I2C utama (MLX90614)
  Wire1.begin(SDA_WIRE1, SCL_WIRE1); // Mulai I2C kedua (MAX30105)

  // MLX90614
  if (!mlx.begin(0x5A, &Wire)) { // Inisialisasi sensor suhu
    Serial.println("Failed to find MLX90614 sensor!"); // Jika gagal, tampilkan pesan
    while (1); // Berhenti di sini
  } else {
    Serial.println("MLX90614 found!"); // Jika berhasil, tampilkan pesan
  }

  // MAX30105
  if (!particleSensor.begin(Wire1)) { // Inisialisasi sensor MAX30105
    Serial.println("MAX30105 not found. Check wiring/power!"); // Jika gagal, tampilkan pesan
    while (1); // Berhenti di sini
  } else {
    Serial.println("MAX30105 found!"); // Jika berhasil, tampilkan pesan
  }

  particleSensor.setup(); // Konfigurasi default sensor MAX30105
  particleSensor.setPulseAmplitudeRed(0x32); // Set amplitudo LED merah
  particleSensor.setPulseAmplitudeIR(0x32); // Set amplitudo LED IR
  particleSensor.setPulseAmplitudeGreen(0x00); // Matikan LED hijau

  pinMode(readLED, OUTPUT); // Set pin LED sebagai output

  // BLE Setup
  BLEDevice::init("ESP32 Node"); // Inisialisasi BLE dengan nama "ESP32 Node"
  BLEServer *pServer = BLEDevice::createServer(); // Membuat server BLE
  BLEService *pService = pServer->createService(SERVICE_UUID); // Membuat service BLE
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
  ); // Membuat characteristic BLE
  pService->start(); // Mulai service BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); // Mulai advertising BLE
  pAdvertising->addServiceUUID(SERVICE_UUID); // Tambahkan UUID service ke advertising
  pAdvertising->setScanResponse(true); // Aktifkan scan response
  pAdvertising->setMinPreferred(0x06); // Konfigurasi koneksi BLE
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising(); // Mulai advertising BLE
  Serial.println("BLE advertising started..."); // Tampilkan pesan
}

void loop() {
  // Baca suhu objek dari MLX90614
  double objectTemp = mlx.readObjectTempC(); // Baca suhu tubuh/objek

  // Baca nilai IR dari MAX30105 untuk deteksi jari
  long irValue = particleSensor.getIR(); // Baca nilai IR

  if (irValue > 50000) { // Jika nilai IR di atas threshold
    fingerDetected = true; // Jari terdeteksi
  } else {
    fingerDetected = false; // Jari tidak terdeteksi
  }

  Serial.print("Suhu Tubuh/Objek: ");
  Serial.print(objectTemp);
  Serial.println(" °C");

  if (fingerDetected) { // Jika jari terdeteksi
    Serial.println("Jari terdeteksi. Mengukur BPM dan SpO2...");

    // Baca buffer data dari MAX30105
    for (int i = 0; i < BUFFER_LENGTH; i++) {
      while (particleSensor.available() == false)
        particleSensor.check(); // Tunggu data baru dari sensor

      redBuffer[i] = particleSensor.getRed(); // Simpan data RED
      irBuffer[i] = particleSensor.getIR(); // Simpan data IR
      particleSensor.nextSample(); // Lanjut ke sampel berikutnya
      digitalWrite(readLED, !digitalRead(readLED)); // LED indikator berkedip
    }

    // Hitung BPM dan SpO2
    maxim_heart_rate_and_oxygen_saturation(
      irBuffer, BUFFER_LENGTH, redBuffer,
      &spo2, &validSPO2, &heartRate, &validHeartRate
    ); // Proses data untuk mendapatkan BPM & SpO2

    Serial.print("BPM: ");
    Serial.print(heartRate);
    Serial.print(" (Valid: ");
    Serial.print(validHeartRate);
    Serial.println(")");

    Serial.print("SpO2: ");
    Serial.print(spo2);
    Serial.print("% (Valid: ");
    Serial.print(validSPO2);
    Serial.println(")");

    // Kirim data ke gateway lewat BLE
    String data = "{\"heartRate\":" + String(heartRate) +
                  ",\"spo2\":" + String(spo2) +
                  ",\"temp\":" + String(objectTemp, 1) + "}";
    pCharacteristic->setValue(data.c_str()); // Set nilai characteristic BLE
    pCharacteristic->notify(); // Kirim notifikasi BLE
  } else {
    Serial.println("Tidak ada jari terdeteksi. Letakkan jari Anda pada sensor.");
  }

  Serial.println("----------------------------");
  delay(2000); // Delay agar pembacaan stabil
}