//KODE ESP32 NODE FIXXXXXXXXX
#include <Wire.h>
#include <MAX30105.h>
#include <Adafruit_MLX90614.h>
#include "heartRate.h"
#include "spo2_algorithm.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// I2C Bus
#define SDA_WIRE 21
#define SCL_WIRE 22
#define SDA_WIRE1 33
#define SCL_WIRE1 32

// BLE Setup
#define SERVICE_UUID        "89bc34b8-c3a1-4f22-82d9-00a2559bbcc0"
#define CHARACTERISTIC_UUID "cbef4b5c-fe06-41dc-ab84-105dbe7a722c"

// Instance
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
MAX30105 particleSensor; // MAX30105 from SparkFun

// MAX30105 Buffer
#define BUFFER_LENGTH 100
uint32_t irBuffer[BUFFER_LENGTH];
uint32_t redBuffer[BUFFER_LENGTH];
int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;
bool fingerDetected = false;
byte readLED = 13; // Pin untuk indikator pembacaan data

// BLE Characteristics
BLECharacteristic *pCharacteristic;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // I2C start
  Wire.begin(SDA_WIRE, SCL_WIRE);
  Wire1.begin(SDA_WIRE1, SCL_WIRE1);

  // MLX90614
  if (!mlx.begin(0x5A, &Wire)) {
    Serial.println("Failed to find MLX90614 sensor!");
    while (1);
  } else {
    Serial.println("MLX90614 found!");
  }

  // MAX30105
  if (!particleSensor.begin(Wire1)) {
    Serial.println("MAX30105 not found. Check wiring/power!");
    while (1);
  } else {
    Serial.println("MAX30105 found!");
  }

  particleSensor.setup(); // Konfigurasi default MAX30105
  particleSensor.setPulseAmplitudeRed(0x32);
  particleSensor.setPulseAmplitudeIR(0x32);
  particleSensor.setPulseAmplitudeGreen(0x00); // Matikan LED Hijau

  pinMode(readLED, OUTPUT);

  // BLE Setup
  BLEDevice::init("ESP32 Node");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // Functions to help with iPhone connections
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("BLE advertising started...");
}

void loop() {
  // Baca suhu objek dari MLX90614
  double objectTemp = mlx.readObjectTempC();

  // Baca nilai IR dari MAX30105 untuk deteksi jari
  long irValue = particleSensor.getIR();

  if (irValue > 50000) {
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

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample();
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
    pCharacteristic->setValue(data.c_str());
    pCharacteristic->notify();
  } else {
    Serial.println("Tidak ada jari terdeteksi. Letakkan jari Anda pada sensor.");
  }

  Serial.println("----------------------------");
  delay(2000); // Delay lebih lama agar pembacaan stabil
}