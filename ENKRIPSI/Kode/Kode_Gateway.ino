#include <Wire.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEClient.h>
#include <NimBLEScan.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <MPU6050_WE.h> // Tentukan MPU yg kalian gunakan (MPU6500/9250 warna MERAH MPU6050 warna HIJAU)
#include <Crypto.h>
#include <CryptoLW.h>
#include <Ascon128.h>

// WiFi dan MQTT
const char* ssid = "YOUR WIFI SSID";
const char* password = "YOUR PASSWORD";
const char* mqtt_server = "YOUR_MQTT_BROKER_";
const int mqtt_port = 1883;
const char* mqtt_topic = "gateway/health_data";

WiFiClient espClient;
PubSubClient client(espClient);

// GPS
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);
const long gpsReadInterval = 1000;

// BLE NimBLE
static NimBLEUUID serviceUUID("YOUR UUID");
static NimBLEUUID charUUID("YOUR UUID");

static bool doConnect = false;
static bool connected = false;
static bool doScan = false;
static NimBLERemoteCharacteristic *pRemoteCharacteristic;
static const NimBLEAdvertisedDevice *myDevice;
static uint32_t scanTimeMs = 5000;

// Data BLE dari Node
float ble_temp = 0.0;
int ble_bpm = 0;
int ble_spo2 = 0;
bool has_ble_health_data = false;

// MPU6050
#define MPU6050_ADDR 0x68
MPU6050_WE myMPU6050 = MPU6050_WE(MPU6050_ADDR);
const long mpuReadInterval = 1000;
float accX, accY, accZ;
float gyroX, gyroY, gyroZ;

// --- Ascon Encryption ---
Ascon128 ascon;
// Kunci dan IV 128-bit. PENTING: Ganti dengan nilai aman Anda sendiri dan pastikan sama dengan di sisi dekripsi.
uint8_t ascon_key[16] = {TANYAKAN PADA AI UNTUK BUATKAN KEY};
uint8_t ascon_iv[16]  = {TANYAKAN PADA AI UNTUK BUATKAN KEY};

String bytesToHexString(const uint8_t* bytes, size_t len) {
  String str = "";
  str.reserve(len * 2);
  for (size_t i = 0; i < len; ++i) {
    char buf[3];
    sprintf(buf, "%02x", bytes[i]);
    str += buf;
  }
  return str;
}

static void notifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
  Serial.print("NimBLE Notify JSON dari Node: ");
  String jsonStr = String((char*)pData, length);
  Serial.println(jsonStr);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, jsonStr);

  if (!error) {
    ble_temp = doc["temperature"];
    ble_bpm = doc["bpm"];
    ble_spo2 = doc["spo2"];
    has_ble_health_data = true;

    Serial.printf("Diterima - Temp: %.2f °C, BPM: %d, SpO2: %d %%\n", ble_temp, ble_bpm, ble_spo2);
  } else {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
  }
}

class MyClientCallback : public NimBLEClientCallbacks {
  void onConnect(NimBLEClient *pclient) override {
    connected = true;
    Serial.println("Terhubung ke NimBLE Node!");
  }
  void onDisconnect(NimBLEClient *pclient, int reason) override {
    connected = false;
    Serial.printf("Terputus dari NimBLE Node, reason = %d - Memulai pemindaian\n", reason);
    NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    doScan = true;
    has_ble_health_data = false;
  }
};

bool connectToServer() {
  Serial.printf("Mencoba terhubung ke perangkat NimBLE: %s\n", myDevice->getAddress().toString().c_str());

  NimBLEClient *pClient = NimBLEDevice::createClient();
  Serial.println(" - Membuat client NimBLE");

  pClient->setClientCallbacks(new MyClientCallback(), false);

  if (!pClient->connect(myDevice)) {
    Serial.println(" - Gagal terhubung ke server NimBLE");
    NimBLEDevice::deleteClient(pClient);
    return false;
  }
  Serial.println(" - Terhubung ke server NimBLE");

  NimBLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.printf("Gagal menemukan UUID service: %s\n", serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Service ditemukan");

  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.printf("Gagal menemukan UUID characteristic: %s\n", charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Characteristic ditemukan");

  if (pRemoteCharacteristic->canNotify()) {
    if (!pRemoteCharacteristic->subscribe(true, notifyCallback)) {
      Serial.println("Gagal mendaftar untuk notifikasi.");
      pClient->disconnect();
      return false;
    }
  }

  return true;
}
class MyAdvertisedDeviceCallbacks : public NimBLEScanCallbacks {
  void onResult(const NimBLEAdvertisedDevice *advertisedDevice) override {
    Serial.printf("Perangkat NimBLE ditemukan: %s\n", advertisedDevice->toString().c_str());
    if (advertisedDevice->isAdvertisingService(serviceUUID)) {
      Serial.println("Perangkat dengan service yang dicari ditemukan.");
      NimBLEDevice::getScan()->stop();
      myDevice = advertisedDevice;
      doConnect = true;
      doScan = false;
    }
  }
  void onScanEnd(const NimBLEScanResults &results, int reason) override {
    Serial.printf("Pemindaian Selesai, alasan: %d, jumlah perangkat: %d", reason, results.getCount());
    if (doScan && !connected) {
      Serial.println(", memulai pemindaian ulang...");
      NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    } else {
      Serial.println();
    }
  }
};

void connectWifi() {
  WiFi.begin(ssid, password);
  Serial.printf("Menghubungkan ke WiFi: %s\n", ssid);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(".");
  }
  Serial.println("\nWiFi terhubung.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.println("Menghubungkan ke MQTT Broker...");
    if (client.connect("ESP32Gateway")) {
      Serial.println("Terhubung ke MQTT Broker!");
    } else {
      Serial.print("Gagal terhubung, rc=");
      Serial.print(client.state());
      Serial.println(" Coba lagi dalam 2 detik");
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
  }
}

// Task BLE Scan dan Connect
void TaskBLE(void *pvParameters) {
  NimBLEDevice::init("");
  NimBLEScan *pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setScanCallbacks(new MyAdvertisedDeviceCallbacks(), false);
  pBLEScan->setInterval(45);
  pBLEScan->setWindow(15);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(scanTimeMs, false, true);

  while (1) {
    if (doConnect) {
      if (connectToServer()) {
        Serial.println("Berhasil connect ke Node.");
      } else {
        Serial.println("Gagal connect ke Node. Memulai pemindaian ulang...");
        doScan = true; // Set doScan jika koneksi gagal agar scan dimulai lagi
        NimBLEDevice::getScan()->start(scanTimeMs, false, true);
      }
      doConnect = false;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Task MQTT Publish (dengan Enkripsi Ascon)
void TaskMQTT(void *pvParameters) {
  while (1) {
    if (WiFi.status() != WL_CONNECTED) {
      connectWifi();
    }
    if (!client.connected()) {
      reconnectMQTT();
    }
    client.loop();

    if (has_ble_health_data) {
      // 1. Buat Plaintext JSON
      StaticJsonDocument<256> plaintextDoc;
      plaintextDoc["temperature"] = ble_temp;
      plaintextDoc["bpm"] = ble_bpm;
      plaintextDoc["spo2"] = ble_spo2;
      
      char plaintextBuffer[256];
      size_t plaintextLength = serializeJson(plaintextDoc, plaintextBuffer);

      // 2. Enkripsi Plaintext menggunakan Ascon-128
      uint8_t ciphertext[plaintextLength];
      uint8_t tag[16];

      ascon.clear();
      ascon.setKey(ascon_key, sizeof(ascon_key));
      ascon.setIV(ascon_iv, sizeof(ascon_iv));
      ascon.encrypt(ciphertext, (uint8_t*)plaintextBuffer, plaintextLength);
      ascon.computeTag(tag, sizeof(tag));

      // 3. Ubah ciphertext dan tag ke Hex String
      String hexCiphertext = bytesToHexString(ciphertext, plaintextLength);
      String hexTag = bytesToHexString(tag, sizeof(tag));

      // 4. Buat JSON baru berisi data terenkripsi
      StaticJsonDocument<512> encryptedDoc;
      encryptedDoc["ciphertext"] = hexCiphertext;
      encryptedDoc["tag"] = hexTag;

      char encryptedBuffer[512];
      size_t encryptedLength = serializeJson(encryptedDoc, encryptedBuffer);

      // 5. Publish data terenkripsi ke MQTT
      Serial.print("Publish ke MQTT (encrypted): ");
      Serial.println(encryptedBuffer);
      client.publish(mqtt_topic, encryptedBuffer, encryptedLength);

      has_ble_health_data = false;  // Reset flag
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Task GPS Read (Hanya baca, tidak publish)
void TaskGPS(void *pvParameters) {
  while (1) {
    while (gpsSerial.available() > 0) {
      if (gps.encode(gpsSerial.read())) {
        if (gps.location.isUpdated()) {
          Serial.print(" GPS: ");
          Serial.print("Lat: "); Serial.print(gps.location.lat(), 6);
          Serial.print(", Lng: "); Serial.println(gps.location.lng(), 6);
        }
      }
    }
    vTaskDelay(gpsReadInterval / portTICK_PERIOD_MS);
  }
}


// Task MPU Read
void TaskMPU(void *pvParameters) {
  if (!myMPU6050.init()) {
    Serial.println("MPU6050 not connected!");
    vTaskDelete(NULL);
  }
  while (1) {
    xyzFloat gValue = myMPU6050.getGValues();
    xyzFloat gyr = myMPU6050.getGyrValues();

    accX = gValue.x;
    accY = gValue.y;
    accZ = gValue.z;
    gyroX = gyr.x;
    gyroY = gyr.y;
    gyroZ = gyr.z;

    Serial.println("MPU:");
    Serial.printf("  Acc (g): X=%.2f, Y=%.2f, Z=%.2f\n", accX, accY, accZ);
    Serial.printf("  Gyro (°/s): X=%.2f, Y=%.2f, Z=%.2f\n", gyroX, gyroY, gyroZ);

    vTaskDelay(mpuReadInterval / portTICK_PERIOD_MS);
  }
}


void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin();
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);  // GPS UART pins

  WiFi.mode(WIFI_STA);

  client.setServer(mqtt_server, mqtt_port);

  // Create FreeRTOS Tasks
  xTaskCreatePinnedToCore(TaskBLE, "TaskBLE", 8192, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskMQTT, "TaskMQTT", 8192, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskGPS, "TaskGPS", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskMPU, "TaskMPU", 4096, NULL, 1, NULL, 0);
}

void loop() {
  vTaskDelay(portMAX_DELAY);
}
