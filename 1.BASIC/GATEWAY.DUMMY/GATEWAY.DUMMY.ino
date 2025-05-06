// Kode GATEWAY (ESP32) dengan NimBLE, belum wifi fix
#include <Wire.h>
#include <MPU6500_WE.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <NimBLEDevice.h>
#include <NimBLEUtils.h>
#include <NimBLEClient.h>
#include <NimBLEScan.h>

// --- Konfigurasi MPU6500 ---
#define MPU6500_ADDR 0x68
MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR);

// --- Konfigurasi GPS ---
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);  // UART2: RX=16, TX=17

// --- Konfigurasi NimBLE Client ---
static NimBLEUUID serviceUUID("89bc34b8-c3a1-4f22-82d9-00a2559bbcc0");
static NimBLEUUID charUUID("cbef4b5c-fe06-41dc-ab84-105dbe7a722c");

static bool doConnect = false;
static bool connected = false;
static bool doScan = false;
static bool isScanning = false; // Tambahkan variabel untuk melacak status pemindaian
static NimBLERemoteCharacteristic *pRemoteCharacteristic;
static const NimBLEAdvertisedDevice *myDevice;
static uint32_t scanTimeMs = 5000; // Waktu pemindaian dalam milisekon

static void notifyCallback(NimBLERemoteCharacteristic *pBLERemoteCharacteristic, uint8_t *pData, size_t length, bool isNotify) {
  Serial.print("NimBLE Notify dari Node: ");
  Serial.write(pData, length);
  Serial.println();
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
  }
};

bool connectToServer() {
  Serial.print("Mencoba terhubung ke perangkat NimBLE: ");
  Serial.println(myDevice->getAddress().toString().c_str());

  NimBLEClient *pClient = NimBLEDevice::createClient();
  Serial.println(" - Membuat client NimBLE");

  pClient->setClientCallbacks(new MyClientCallback(), false);

  // Hubungkan ke server NimBLE remote.
  if (!pClient->connect(myDevice)) {
    Serial.println(" - Gagal terhubung ke server NimBLE");
    NimBLEDevice::deleteClient(pClient);
    return false;
  }
  Serial.println(" - Terhubung ke server NimBLE");
 // pClient->setMTU(517);

  // Dapatkan referensi ke service yang dicari di server NimBLE remote.
  NimBLERemoteService *pRemoteService = pClient->getService(serviceUUID);
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
    std::string value = pRemoteCharacteristic->readValue();
    Serial.print("Nilai characteristic awal: ");
    Serial.println(value.c_str());
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
    Serial.print("Perangkat NimBLE ditemukan: ");
    Serial.println(advertisedDevice->toString().c_str());

    if (advertisedDevice->isAdvertisingService(serviceUUID)) {
      Serial.println("Perangkat dengan service yang dicari ditemukan.");
      NimBLEDevice::getScan()->stop();
      isScanning = false; // Set status pemindaian menjadi false
      myDevice = advertisedDevice;
      doConnect = true;
      doScan = false; // Stop scanning setelah perangkat ditemukan
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

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 Gateway Started dengan NimBLE!");

  // --- Setup MPU6500 ---
  Wire.begin();
  if (!myMPU6500.init()) {
    Serial.println("MPU6500 tidak terdeteksi");
  } else {
    Serial.println("MPU6500 terhubung");
  }

  delay(1000);
  Serial.println("Kalibrasi MPU6500... Letakkan sensor dengan tenang");
  myMPU6500.autoOffsets();
  Serial.println("Kalibrasi MPU6500 selesai!");

  myMPU6500.enableGyrDLPF();
  myMPU6500.setGyrDLPF(MPU6500_DLPF_6);
  myMPU6500.setSampleRateDivider(5);
  myMPU6500.setGyrRange(MPU6500_GYRO_RANGE_250);
  myMPU6500.setAccRange(MPU6500_ACC_RANGE_2G);
  myMPU6500.enableAccDLPF(true);
  myMPU6500.setAccDLPF(MPU6500_DLPF_6);

  // --- Setup GPS ---
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  Serial.println("GPS Neo-6M siap!"); 

  // --- Setup NimBLE Client ---
  NimBLEDevice::init("NimBLE_Client_Gateway");
  Serial.println("Mempersiapkan NimBLE Client...");

  NimBLEScan *pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setScanCallbacks(new MyAdvertisedDeviceCallbacks(), false);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(100);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(scanTimeMs, false, true); // Scan selama scanTimeMs untuk pertama kali, dan ulangi
  isScanning = true; // Set status pemindaian menjadi true
  doScan = true;
}

void loop() {
  // --- Data MPU6500 ---
  xyzFloat gValue = myMPU6500.getGValues();
  xyzFloat gyr = myMPU6500.getGyrValues();
  float resultantG = myMPU6500.getResultantG(gValue);

  Serial.println("=== Data Accel / Gyro ===");
  Serial.print("Accelerometer (g): ");
  Serial.print(gValue.x); Serial.print(", ");
  Serial.print(gValue.y); Serial.print(", ");
  Serial.println(gValue.z);
  Serial.print("Resultant g: ");
  Serial.println(resultantG, 2);

  Serial.print("Gyroscope (°/s): ");
  Serial.print(gyr.x); Serial.print(", ");
  Serial.print(gyr.y); Serial.print(", ");
  Serial.println(gyr.z);

  // --- Data GPS ---
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated()) {
    Serial.println("=== Data GPS ===");
    Serial.print("Latitude: "); Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: "); Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: "); Serial.print(gps.altitude.meters()); Serial.println(" m");
    Serial.print("Satellites: "); Serial.println(gps.satellites.value());
  } else {
    Serial.println("GPS: mencari sinyal...");
  }

  // --- Koneksi NimBLE ---
  if (doConnect) {
    if (connectToServer()) {
      Serial.println("Berhasil terhubung ke perangkat NimBLE.");
    } else {
      Serial.println("Gagal terhubung! Memulai ulang pemindaian...");
      NimBLEDevice::getScan()->start(scanTimeMs, false, true); // Coba scan lagi
      isScanning = true; // Set status pemindaian menjadi true
      doScan = true;
      doConnect = false; // Reset flag setelah mencoba koneksi
    }
    doConnect = false; // Pastikan flag direset setelah proses koneksi
  }

  // Jika terhubung, kita bisa melakukan sesuatu (misalnya mengirim data dari MPU/GPS ke node jika diperlukan)
  if (connected) {
    // Contoh: Kirim data gabungan (MPU + GPS) ke node (jika node memiliki characteristic untuk ditulis)
    // String gatewayData = "Acc:" + String(gValue.x) + "," + String(gValue.y) + "," + String(gValue.z) +
    //                      ",Gyr:" + String(gyr.x) + "," + String(gyr.y) + "," + String(gyr.z) +
    //                      ",Lat:" + String(gps.location.lat(), 6) + ",Lng:" + String(gps.location.lng(), 6);
    // if (pRemoteCharacteristic->canWrite()) {
    //   if (pRemoteCharacteristic->writeValue((uint8_t*)gatewayData.c_str(), gatewayData.length())) {
    //     Serial.println("Data Gateway dikirim ke Node: " + gatewayData);
    //   } else {
    //     Serial.println("Gagal mengirim data ke Node.");
    //   }
    // }
  } else if (doScan && !isScanning) {
    // Jika tidak terhubung, dan pemindaian belum aktif, mulai pemindaian
    Serial.println("Memulai pemindaian NimBLE...");
    NimBLEDevice::getScan()->start(scanTimeMs, false, true);
    isScanning = true;
  }

  Serial.println("*************");
  delay(3000);
}