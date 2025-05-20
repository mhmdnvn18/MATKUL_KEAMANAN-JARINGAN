#include <Wire.h>
#include <MAX30105.h>
#include <Adafruit_MLX90614.h>
#include "heartRate.h"
#include "spo2_algorithm.h"
#include <NimBLEDevice.h>

// I2C Pins
#define SDA_WIRE   21
#define SCL_WIRE   22
#define SDA_WIRE1  33
#define SCL_WIRE1  32

// BLE UUIDs
#define SERVICE_UUID         "Pakai UUID Generator"
#define CHARACTERISTIC_UUID  "Pakai UUID Generator"

// Sensor Buffers
#define BUFFER_LENGTH 100
uint32_t irBuffer[BUFFER_LENGTH];
uint32_t redBuffer[BUFFER_LENGTH];

// Status LED
//#define READ_LED 13

// Sensor Objects
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
MAX30105 particleSensor;

// Sensor Values
int32_t spo2, heartRate;
int8_t validSPO2, validHeartRate;
bool fingerDetected = false;

// BLE
NimBLECharacteristic* pCharacteristic;
bool bleConnected = false;

// BLE Server Callback Class
class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer, NimBLEConnInfo&) override {
    Serial.println("Client connected");
    bleConnected = true;
  }

  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo&, int) override {
    Serial.println("Client disconnected, restarting advertising...");
    bleConnected = false;
    NimBLEDevice::startAdvertising();
  }
} serverCallbacks;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Init I2C
  Wire.begin(SDA_WIRE, SCL_WIRE);
  Wire1.begin(SDA_WIRE1, SCL_WIRE1);

  // Init MLX90614
  if (!mlx.begin(0x5A, &Wire)) {
    Serial.println("MLX90614 not found!");
    while (1);
  }

  // Init MAX30105
  if (!particleSensor.begin(Wire1)) {
    Serial.println("MAX30105 not found!");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x32);
  particleSensor.setPulseAmplitudeIR(0x32);
  particleSensor.setPulseAmplitudeGreen(0);

  //pinMode(READ_LED, OUTPUT);

  // Init BLE
  NimBLEDevice::init("ESP32 Node NimBLE");
  NimBLEServer* pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(&serverCallbacks);

  NimBLEService* pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY
  );
  pService->start();

  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
  Serial.println("BLE NimBLE advertising started");
}

void loop() {
  double objectTemp = mlx.readObjectTempC();
  long irValue = particleSensor.getIR();
  fingerDetected = irValue > 50000;

  Serial.printf("Suhu Tubuh/Objek: %.2f °C\n", objectTemp);

  if (fingerDetected) {
    Serial.println("Jari terdeteksi. Mengukur BPM dan SpO2...");

    for (int i = 0; i < BUFFER_LENGTH; i++) {
      while (!particleSensor.available()) {
        particleSensor.check();
      }

      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample();

     // digitalWrite(READ_LED, !digitalRead(READ_LED)); // Toggle LED
    }

    maxim_heart_rate_and_oxygen_saturation(
      irBuffer, BUFFER_LENGTH, redBuffer,
      &spo2, &validSPO2, &heartRate, &validHeartRate
    );

    Serial.printf("BPM: %d (Valid: %d)\n", heartRate, validHeartRate);
    Serial.printf("SpO2: %d%% (Valid: %d)\n", spo2, validSPO2);

    if (validSPO2 && validHeartRate && bleConnected) {
      String data = "{\"temperature\":" + String(objectTemp, 2) +
                    ",\"bpm\":" + String(heartRate) +
                    ",\"spo2\":" + String(spo2) + "}";
      pCharacteristic->setValue(data);
      pCharacteristic->notify();
      Serial.println("Data dikirim melalui BLE NimBLE");
    } else {
      Serial.println("Data tidak dikirim: BLE tidak terhubung atau data tidak valid.");
    }
  } else {
    Serial.println("Tidak ada jari terdeteksi. Letakkan jari Anda pada sensor.");
  }

  Serial.println("----------------------------");
  delay(1000);
}
