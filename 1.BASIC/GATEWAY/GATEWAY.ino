// Kode GATEWAY (ESP32) dengan BLE, belum wifi fix
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLEScan.h>

// --- Konfigurasi BLE Client ---
static BLEUUID serviceUUID("89bc34b8-c3a1-4f22-82d9-00a2559bbcc0");
static BLEUUID charUUID("cbef4b5c-fe06-41dc-ab84-105dbe7a722c");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static bool isScanning = false; // Tambahkan variabel untuk melacak status pemindaian
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLEAdvertisedDevice *myDevice;

static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
  Serial.print("BLE Notify dari Node: ");
  Serial.write(pData, length);
  Serial.println();
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
  Serial.println(" - Membuat client BLE");

  pClient->setClientCallbacks(new MyClientCallback());

  // Hubungkan ke server BLE remote.
  pClient->connect(myDevice);
  Serial.println(" - Terhubung ke server BLE");
  pClient->setMTU(517);

  // Dapatkan referensi ke service yang dicari di server BLE remote.
  BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Gagal menemukan UUID service: ");
    Serial.println(serviceUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Service ditemukan");

  // Dapatkan referensi ke characteristic di dalam service.
  pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
  if (pRemoteCharacteristic == nullptr) {
    Serial.print("Gagal menemukan UUID characteristic: ");
    Serial.println(charUUID.toString().c_str());
    pClient->disconnect();
    return false;
  }
  Serial.println(" - Characteristic ditemukan");

  if (pRemoteCharacteristic->canRead()) {
    String value = pRemoteCharacteristic->readValue();
    Serial.print("Nilai characteristic awal: ");
    Serial.println(value.c_str());
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
      isScanning = false; // Set status pemindaian menjadi false
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = false; // Stop scanning setelah perangkat ditemukan
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Gateway Started!");

  // --- Setup BLE Client ---
  BLEDevice::init("");
  Serial.println("Mempersiapkan BLE Client...");

  BLEScan *pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false); // Scan selama 5 detik untuk pertama kali
  isScanning = true; // Set status pemindaian menjadi true
  doScan = true;
}

void loop() {
  // --- Koneksi BLE ---
  if (doConnect) {
    if (connectToServer()) {
      Serial.println("Berhasil terhubung ke perangkat BLE.");
    } else {
      Serial.println("Gagal terhubung! Memulai ulang pemindaian...");
      BLEDevice::getScan()->start(5, false); // Coba scan lagi
      isScanning = true; // Set status pemindaian menjadi true
      doScan = true;
    }
    doConnect = false;
  }

  // Jika terhubung, kita bisa melakukan sesuatu (misalnya mengirim data ke node jika diperlukan)
  if (connected) {
    // Contoh: Kirim data ke node (jika node memiliki characteristic untuk ditulis)
    // if (pRemoteCharacteristic->canWrite()) {
    //   String data = "Contoh data";
    //   pRemoteCharacteristic->writeValue(data.c_str(), data.length());
    //   Serial.println("Data Gateway dikirim ke Node: " + data);
    // }
  } else if (doScan && !isScanning) {
    // Jika tidak terhubung, dan pemindaian belum aktif, mulai pemindaian
    Serial.println("Memulai pemindaian BLE...");
    BLEDevice::getScan()->start(5, false);
    isScanning = true;
  }
}