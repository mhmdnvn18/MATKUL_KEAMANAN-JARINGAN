// Kode GATEWAY (ESP32) dengan BLE, belum wifi fix
#include <Wire.h>
#include <MPU6500_WE.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEClient.h>
#include <BLEScan.h>

// --- Konfigurasi MPU6500 ---
#define MPU6500_ADDR 0x68
MPU6500_WE myMPU6500 = MPU6500_WE(MPU6500_ADDR);

// --- Konfigurasi GPS ---
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);  // UART2: RX=16, TX=17

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

  // Jika terhubung, kita bisa melakukan sesuatu (misalnya mengirim data dari MPU/GPS ke node jika diperlukan)
  if (connected) {
    // Contoh: Kirim data gabungan (MPU + GPS) ke node (jika node memiliki characteristic untuk ditulis)
    // String gatewayData = "Acc:" + String(gValue.x) + "," + String(gValue.y) + "," + String(gValue.z) +
    //                      ",Gyr:" + String(gyr.x) + "," + String(gyr.y) + "," + String(gyr.z) +
    //                      ",Lat:" + String(gps.location.lat(), 6) + ",Lng:" + String(gps.location.lng(), 6);
    // if (pRemoteCharacteristic->canWrite()) {
    //   pRemoteCharacteristic->writeValue(gatewayData.c_str(), gatewayData.length());
    //   Serial.println("Data Gateway dikirim ke Node: " + gatewayData);
    // }
  } else if (doScan && !isScanning) {
    // Jika tidak terhubung, dan pemindaian belum aktif, mulai pemindaian
    Serial.println("Memulai pemindaian BLE...");
    BLEDevice::getScan()->start(5, false);
    isScanning = true;
  }

  Serial.println("*************");
  delay(1000);
}