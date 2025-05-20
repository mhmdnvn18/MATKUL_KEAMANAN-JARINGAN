#include <Wire.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEClient.h>
#include <NimBLEScan.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h> // Library untuk parsing JSON

// --- Konfigurasi WiFi ---
const char* ssid = ".............";          // Ganti dengan SSID WiFi Anda
const char* password = "............"; // Ganti dengan password WiFi Anda

// --- Konfigurasi MQTT ---
const char* mqtt_server = "IP lokal laptop"; // Ganti dengan IP address server MQTT Anda
const int mqtt_port = 1883;                  // Port default MQTT
const char* mqtt_topic = "gateway/health_data";    // Topik MQTT

WiFiClient espClient;
PubSubClient client(espClient);
bool mqttConnectedPreviously = false;

// --- Konfigurasi GPS ---
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);   // UART2: RX=16, TX=17
unsigned long lastGpsReadTime = 0;
const long gpsReadInterval = 1000;

// --- Konfigurasi NimBLE Client ---
static NimBLEUUID serviceUUID("Disamakan dengan node");
static NimBLEUUID charUUID("Disamakan dengan node");

static bool doConnect = false;
static bool connected = false;
static bool doScan = false;
static bool isScanning = false;
static NimBLERemoteCharacteristic *pRemoteCharacteristic;
static const NimBLEAdvertisedDevice *myDevice;
static uint32_t scanTimeMs = 5000;

// Variabel untuk menyimpan data kesehatan dari Node BLE
float ble_temp = 0.0;
int ble_bpm = 0;
int ble_spo2 = 0;
bool has_ble_health_data = false;

static void notifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
  Serial.print("NimBLE Notify JSON dari Node: ");
  String jsonStr = String((char*)pData);
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
    isScanning = true;
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

  if (pRemoteCharacteristic->canRead()) {
    std::string value = pRemoteCharacteristic->readValue();
    Serial.printf("Nilai characteristic awal: %s\n", value.c_str());
  }

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
      isScanning = false;
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
      isScanning = true;
    } else {
      Serial.println();
    }
  }
};

void connectWifi() {
  Serial.println("Menghubungkan ke WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung");
  Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
}

void connectMQTT() {
  if (!client.connected()) {
    Serial.printf("Mencoba menghubungkan ke MQTT... %s:%d\n", mqtt_server, mqtt_port);
    if (client.connect("ESP32ClientGateway")) {
      Serial.println("Terhubung ke MQTT Broker");
      mqttConnectedPreviously = true;
    } else {
      Serial.print("Gagal terhubung ke MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds");
      mqttConnectedPreviously = false;
    }
  } else if (!mqttConnectedPreviously) {
    Serial.println("Sudah terhubung ke MQTT Broker.");
    mqttConnectedPreviously = true;
  }
}

void publishHealthData() {
  if (has_ble_health_data && client.connected()) {
    StaticJsonDocument<200> doc; // Gunakan StaticJsonDocument jika ukuran data tetap
    doc["temperature"] = ble_temp;
    doc["bpm"] = ble_bpm;
    doc["spo2"] = ble_spo2;
    String jsonPayload;
    serializeJson(doc, jsonPayload);
    client.publish(mqtt_topic, jsonPayload.c_str());
    Serial.printf("Data Kesehatan dipublikasikan ke MQTT: %s\n", jsonPayload.c_str());
  }
}

void readGPS() {
  unsigned long currentTime = millis();
  if (currentTime - lastGpsReadTime >= gpsReadInterval) {
    while (gpsSerial.available() > 0) {
      gps.encode(gpsSerial.read());
    }

    Serial.println("=== Data GPS ===");
    if (gps.location.isUpdated()) {
      Serial.printf("Latitude: %.6f, Longitude: %.6f, Altitude: %.2f m, Satellites: %d\n",
                    gps.location.lat(), gps.location.lng(), gps.altitude.meters(), gps.satellites.value());
    } else {
      Serial.println("GPS: mencari sinyal...");
    }
    lastGpsReadTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Gateway Started dengan NimBLE (JSON)!");

  client.setServer(mqtt_server, mqtt_port);
  connectWifi();
  connectMQTT();

  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("GPS Neo-6M siap!");

  NimBLEDevice::init("NimBLE_Client_Gateway");
  Serial.println("Mempersiapkan NimBLE Client...");

  NimBLEScan *pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setScanCallbacks(new MyAdvertisedDeviceCallbacks(), false);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(100);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(scanTimeMs, false, true);
  isScanning = true;
  doScan = true;
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWifi();
  }

  connectMQTT();
  client.loop();

  readGPS();

  if (doConnect) {
    if (connectToServer()) {
      Serial.println("Berhasil terhubung ke perangkat NimBLE.");
    } else {
      Serial.println("Gagal terhubung! Memulai ulang pemindaian...");
      NimBLEDevice::getScan()->start(scanTimeMs, false, true);
      isScanning = true;
      doScan = true;
      doConnect = false;
    }
    doConnect = false;
  }

  static unsigned long lastPublishTime = 0;
  unsigned long currentTime = millis();
  const long publishInterval = 5000;
  if (connected && has_ble_health_data && (currentTime - lastPublishTime >= publishInterval)) {
    publishHealthData();
    lastPublishTime = currentTime;
  }

  if (doScan && !isScanning && !connected) {
    Serial.println("Memulai pemindaian NimBLE...");
    NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    isScanning = true;
  }

  delay(10);
}
