// ESP32 NODE FIXED VERSION WITH NimBLE
#include <Wire.h>
#include <MAX30105.h>
#include <Adafruit_MLX90614.h>
#include "heartRate.h"
#include "spo2_algorithm.h"
#include <NimBLEDevice.h>

#define SDA_WIRE 21
#define SCL_WIRE 22
#define SDA_WIRE1 33
#define SCL_WIRE1 32

#define SERVICE_UUID        "89bc34b8-c3a1-4f22-82d9-00a2559bbcc0"
#define CHARACTERISTIC_UUID "cbef4b5c-fe06-41dc-ab84-105dbe7a722c"

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
MAX30105 particleSensor;

#define BUFFER_LENGTH 100
uint32_t irBuffer[BUFFER_LENGTH];
uint32_t redBuffer[BUFFER_LENGTH];
int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;
bool fingerDetected = false;
byte readLED = 13;

NimBLECharacteristic* pCharacteristic;

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
    Serial.println("Client connected");
  }
  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
    Serial.println("Client disconnected, restarting advertising...");
    NimBLEDevice::startAdvertising();
  }
} serverCallbacks;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_WIRE, SCL_WIRE);
  Wire1.begin(SDA_WIRE1, SCL_WIRE1);

  if (!mlx.begin(0x5A, &Wire)) {
    Serial.println("MLX90614 not found!");
    while (1);
  }

  if (!particleSensor.begin(Wire1)) {
    Serial.println("MAX30105 not found!");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x32);
  particleSensor.setPulseAmplitudeIR(0x32);
  particleSensor.setPulseAmplitudeGreen(0);

  pinMode(readLED, OUTPUT);

  NimBLEDevice::init("ESP32 Node NimBLE");
  NimBLEServer* pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(&serverCallbacks);

  NimBLEService* pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY);

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
      while (!particleSensor.available()) particleSensor.check();
      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample();
      digitalWrite(readLED, !digitalRead(readLED));
    }

    maxim_heart_rate_and_oxygen_saturation(irBuffer, BUFFER_LENGTH, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

    Serial.printf("BPM: %d (Valid: %d)\n", heartRate, validHeartRate);
    Serial.printf("SpO2: %d%% (Valid: %d)\n", spo2, validSPO2);

    String data = "Temp:" + String(objectTemp) + "C, BPM:" + String(heartRate) + ", SpO2:" + String(spo2) + "%";
    pCharacteristic->setValue(data);
    pCharacteristic->notify();
  } else {
    Serial.println("Tidak ada jari terdeteksi. Letakkan jari Anda pada sensor.");
  }

  Serial.println("----------------------------");
  delay(2000);
}
