#include <WiFi.h>
#include <HTTPClient.h>

// Ganti dengan SSID dan password WiFi Anda
const char* ssid = "Iphone";
const char* password = "12345678";

// Hapus/komentar baris berikut karena tidak digunakan lagi
// const char* serverUrl = "http://your-server-address/api/data"; // contoh: http://192.168.1.100:5000/api/data

// Konfigurasi Supabase
const char* supabaseUrl = "https://cvmsregwxtcvxdspxysq.supabase.co";
const char* supabaseApiKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImN2bXNyZWd3eHRjdnhkc3B4eXNxIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDY1MTUwNTEsImV4cCI6MjA2MjA5MTA1MX0.SiqgWiZtGDdqndi6Zxf6T5q9ogDCiB7AcGBh0XJjmIo";
const char* supabaseTableEndpoint = "/rest/v1/sensor_data"; // ganti dengan nama tabel Anda

// Tambahkan device_id (nama ESP32)
const char* device_id = "ESP32-ROOM1"; // Ganti sesuai nama/ID ESP32 Anda

// Fungsi untuk mengirim data ke Supabase
void sendToSupabase(int heartRate, int validHeartRate, int spo2, int validSpO2, float temp, const char* device_id) {
  HTTPClient http;
  String url = String(supabaseUrl) + String(supabaseTableEndpoint);
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", supabaseApiKey);
  http.addHeader("Authorization", String("Bearer ") + supabaseApiKey);

  // Buat JSON string untuk field sensor_value (TANPA escape)
  String sensorValue = "{";
  sensorValue += "\"heartRate\":" + String(heartRate) + ",";
  sensorValue += "\"validHeartRate\":" + String(validHeartRate) + ",";
  sensorValue += "\"spo2\":" + String(spo2) + ",";
  sensorValue += "\"validSpO2\":" + String(validSpO2) + ",";
  sensorValue += "\"temp\":" + String(temp, 1);
  sensorValue += "}";

  // Payload utama (tambahkan device_id)
  String jsonPayload = "{\"device_id\":\"" + String(device_id) + "\",\"sensor_value\":" + sensorValue + "}";

  int httpResponseCode = http.POST(jsonPayload);
  if (httpResponseCode > 0) {
    Serial.print("Supabase sent, response: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Supabase error: ");
    Serial.println(http.errorToString(httpResponseCode));
  }
  http.end();
}

void setup() {
  Serial.begin(115200);

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void loop() {
  // Generate data dummy sesuai format yang diinginkan
  int heartRate = random(60, 120);         // BPM
  int validHeartRate = random(0, 2);       // 0 atau 1
  int spo2 = random(90, 100);              // SpO2 %
  int validSpO2 = random(0, 2);            // 0 atau 1
  float temp = random(350, 400) / 10.0;    // 35.0 - 40.0 C

  // Print ke serial sesuai permintaan
  Serial.print("BPM: ");
  Serial.print(heartRate);
  Serial.print(" (Valid: ");
  Serial.print(validHeartRate);
  Serial.println(")");

  Serial.print("SpO2: ");
  Serial.print(spo2);
  Serial.print("% (Valid: ");
  Serial.print(validSpO2);
  Serial.println(")");

  // Kirim ke Supabase jika WiFi terhubung
  if (WiFi.status() == WL_CONNECTED) {
    sendToSupabase(heartRate, validHeartRate, spo2, validSpO2, temp, device_id);
  } else {
    Serial.println("WiFi not connected, data not sent.");
  }

  delay(5000);
}