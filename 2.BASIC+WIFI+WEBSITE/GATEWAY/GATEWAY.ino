// GATEWAY ESP32 BLE + WiFi + Supabase JSON
#include <WiFi.h>
#include <HTTPClient.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLEScan.h>

// --- Konfigurasi WiFi ---
const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";

// --- Konfigurasi Supabase ---
const String supabaseUrl = "https://cvmsregwxtcvxdspxysq.supabase.co";
const String apiKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImN2bXNyZWd3eHRjdnhkc3B4eXNxIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDY1MTUwNTEsImV4cCI6MjA2MjA5MTA1MX0.SiqgWiZtGDdqndi6Zxf6T5q9ogDCiB7AcGBh0XJjmIo";
const String tableName = "sensor_data";

// --- Konfigurasi BLE Client ---
static BLEUUID serviceUUID("89bc34b8-c3a1-4f22-82d9-00a2559bbcc0");
static BLEUUID charUUID("cbef4b5c-fe06-41dc-ab84-105dbe7a722c");
static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static bool isScanning = false;
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;

// Fungsi kirim data ke Supabase (format JSON string)
void sendToSupabase(const String& jsonValue) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected, skip send.");
    return;
  }
  HTTPClient http;
  String fullUrl = supabaseUrl + "/rest/v1/" + tableName;
  http.begin(fullUrl);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", apiKey);
  http.addHeader("Authorization", "Bearer " + apiKey);

  // Data JSON: sensor_value kolom string JSON
  String httpRequestData = "{\"sensor_value\":" + jsonValue + "}";

  int httpResponseCode = http.POST(httpRequestData);
  if (httpResponseCode > 0) {
    Serial.print("Supabase HTTP Response: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Supabase HTTP Error: ");
    Serial.println(httpResponseCode);
  }
  http.end();
}

// BLE Notify Callback
static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
  // Asumsikan data BLE sudah berupa JSON string
  String sensorData = "";
  for (size_t i = 0; i < length; i++) {
    sensorData += (char)pData[i];
  }
  Serial.print("BLE Notify (JSON): ");
  Serial.println(sensorData);

  // Validasi sederhana JSON
  if (sensorData.startsWith("{") && sensorData.endsWith("}")) {
    sendToSupabase(sensorData);
  } else {
    Serial.println("Data BLE bukan JSON, tidak dikirim ke Supabase.");
  }
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient *pclient) {
    connected = true;
    Serial.println("Terhubung ke BLE Node!");
  }
  void onDisconnect(BLEClient *pclient) {
    connected = false;
    Serial.println("Terputus dari BLE Node.");
  }
};

bool connectToServer() {
  Serial.print("Mencoba terhubung ke perangkat BLE: ");
  Serial.println(myDevice->getAddress().toString().c_str());

  BLEClient *pClient = BLEDevice::createClient();
  pClient->setClientCallbacks(new MyClientCallback());
  if (!pClient->connect(myDevice)) {
    Serial.println("Gagal connect BLE.");
    return false;
  }
  pClient->setMTU(517);

  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Gagal menemukan UUID service: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Gagal menemukan UUID characteristic: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  if (pRemoteCharacteristic->canNotify()) {
    pRemoteCharacteristic->registerForNotify(notifyCallback);
  }
  return true;
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.print("Perangkat BLE ditemukan: ");
    Serial.println(advertisedDevice.toString().c_str());
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      Serial.println("Perangkat dengan service yang dicari ditemukan.");
      BLEDevice::getScan()->stop();
      isScanning = false;
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = false;
    }
  }
};

void setup() {
  Serial.begin(115200);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  int wifiTry = 0;
  while (WiFi.status() != WL_CONNECTED && wifiTry < 30) {
    delay(500);
    Serial.print(".");
    wifiTry++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nTerhubung WiFi!");
  } else {
    Serial.println("\nGagal konek WiFi!");
  }

  // BLE
  BLEDevice::init("");
  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  isScanning = true;
  doScan = true;
}

void loop() {
  // Reconnect WiFi jika terputus
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi terputus, reconnect...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    delay(1000);
    return;
  }

  // BLE connect logic
  if (doConnect) {
    if (connectToServer()) {
      Serial.println("Berhasil terhubung ke perangkat BLE.");
    } else {
      Serial.println("Gagal terhubung! Scan ulang...");
      BLEDevice::getScan()->start(5, false);
      isScanning = true;
      doScan = true;
    }
    doConnect = false;
  }

  // Jika tidak terhubung BLE, scan ulang jika perlu
  if (!connected && doScan && !isScanning) {
    Serial.println("Memulai pemindaian BLE...");
    BLEDevice::getScan()->start(5, false);
    isScanning = true;
  }

  delay(100); // Loop ringan
}