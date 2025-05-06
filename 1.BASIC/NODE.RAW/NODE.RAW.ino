//KODE ESP32 NODE FIXXXXXXXXX

#include <Wire.h> // Library I2C utama
#include <MAX30105.h> // Library sensor MAX30105 (pulse oximeter)
#include <Adafruit_MLX90614.h> // Library sensor suhu MLX90614
#include "heartRate.h" // Library untuk perhitungan heart rate
#include "spo2_algorithm.h" // Library untuk perhitungan SpO2
#include <BLEDevice.h> // Library BLE utama ESP32
#include <BLEUtils.h> // Library utilitas BLE
#include <BLEServer.h> // Library BLE server

// I2C Bus
#define SDA_WIRE 21 // Pin SDA untuk I2C utama
#define SCL_WIRE 22 // Pin SCL untuk I2C utama
#define SDA_WIRE1 33 // Pin SDA untuk I2C kedua (MAX30105)
#define SCL_WIRE1 32 // Pin SCL untuk I2C kedua (MAX30105)

// BLE Setup
#define SERVICE_UUID        "89bc34b8-c3a1-4f22-82d9-00a2559bbcc0" // UUID service BLE
#define CHARACTERISTIC_UUID "cbef4b5c-fe06-41dc-ab84-105dbe7a722c" // UUID characteristic BLE

// Instance
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); // Objek sensor suhu MLX90614
MAX30105 particleSensor; // MAX30105 from SparkFun // Objek sensor MAX30105

// MAX30105 Buffer
#define BUFFER_LENGTH 100 // Panjang buffer data sensor
uint32_t irBuffer[BUFFER_LENGTH]; // Buffer data IR
uint32_t redBuffer[BUFFER_LENGTH]; // Buffer data RED
int32_t spo2; // Variabel hasil SpO2
int8_t validSPO2; // Status validasi SpO2
int32_t heartRate; // Variabel hasil heart rate
int8_t validHeartRate; // Status validasi heart rate
bool fingerDetected = false; // Status deteksi jari
byte readLED = 13; // Pin untuk indikator pembacaan data

// BLE Characteristics
BLECharacteristic *pCharacteristic; // Pointer ke characteristic BLE

void setup() {
  Serial.begin(115200); // Mulai serial monitor
  delay(1000); // Delay untuk stabilisasi

  // I2C start
  Wire.begin(SDA_WIRE, SCL_WIRE); // Inisialisasi I2C utama (MLX90614)
  Wire1.begin(SDA_WIRE1, SCL_WIRE1); // Inisialisasi I2C kedua (MAX30105)

  // MLX90614
  if (!mlx.begin(0x5A, &Wire)) {
    Serial.println("Failed to find MLX90614 sensor!"); // Jika sensor tidak ditemukan
    while (1); // Berhenti di sini
  } else {
    Serial.println("MLX90614 found!"); // Sensor ditemukan
  }

  // MAX30105
  if (!particleSensor.begin(Wire1)) {
    Serial.println("MAX30105 not found. Check wiring/power!"); // Jika sensor tidak ditemukan
    while (1); // Berhenti di sini
  } else {
    Serial.println("MAX30105 found!"); // Sensor ditemukan
  }

  particleSensor.setup(); // Konfigurasi default MAX30105
  particleSensor.setPulseAmplitudeRed(0x32); // Set amplitudo LED merah
  particleSensor.setPulseAmplitudeIR(0x32); // Set amplitudo LED IR
  particleSensor.setPulseAmplitudeGreen(0x00); // Matikan LED hijau

  pinMode(readLED, OUTPUT); // Set pin indikator sebagai output

  // BLE Setup
  BLEDevice::init("ESP32 Node"); // Inisialisasi BLE dengan nama "ESP32 Node"
  BLEServer *pServer = BLEDevice::createServer(); // Buat server BLE
  BLEService *pService = pServer->createService(SERVICE_UUID); // Buat service BLE
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY); // Buat characteristic BLE
  pService->start(); // Mulai service BLE
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising(); // Dapatkan objek advertising BLE
  pAdvertising->addServiceUUID(SERVICE_UUID); // Tambahkan UUID service ke advertising
  pAdvertising->setScanResponse(true); // Aktifkan scan response
  pAdvertising->setMinPreferred(0x06);  // Fungsi untuk koneksi iPhone
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising(); // Mulai advertising BLE
  Serial.println("BLE advertising started..."); // Info BLE sudah mulai
}

void loop() {
  // Baca suhu objek dari MLX90614
  double objectTemp = mlx.readObjectTempC(); // Baca suhu objek

  // Baca nilai IR dari MAX30105 untuk deteksi jari
  long irValue = particleSensor.getIR(); // Baca nilai IR

  if (irValue > 50000) { // Jika IR cukup besar, jari terdeteksi
    fingerDetected = true;
  } else {
    fingerDetected = false;
  }

  Serial.print("Suhu Tubuh/Objek: ");
  Serial.print(objectTemp);
  Serial.println(" °C");

  if (fingerDetected) {
    Serial.println("Jari terdeteksi. Mengukur BPM dan SpO2...");

    // Baca buffer data dari MAX30105
    for (int i = 0; i < BUFFER_LENGTH; i++) {
      while (particleSensor.available() == false)
        particleSensor.check(); // Cek sensor untuk data baru

      redBuffer[i] = particleSensor.getRed(); // Simpan data RED
      irBuffer[i] = particleSensor.getIR(); // Simpan data IR
      particleSensor.nextSample(); // Lanjut ke sampel berikutnya
      digitalWrite(readLED, !digitalRead(readLED)); // Berkedip setiap pembacaan
    }

    // Hitung BPM dan SpO2
    maxim_heart_rate_and_oxygen_saturation(irBuffer, BUFFER_LENGTH, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

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
    String data = "Temp:" + String(objectTemp) + "C, BPM:" + String(heartRate) + ", SpO2:" + String(spo2) + "%";
    pCharacteristic->setValue(data.c_str()); // Set nilai characteristic BLE
    pCharacteristic->notify(); // Kirim notifikasi BLE
  } else {
    Serial.println("Tidak ada jari terdeteksi. Letakkan jari Anda pada sensor.");
  }

  Serial.println("----------------------------");
  delay(2000); // Delay lebih lama agar pembacaan stabil
}