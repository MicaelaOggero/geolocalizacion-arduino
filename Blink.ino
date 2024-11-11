#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h> // Usa <WiFi.h> si tienes un ESP32
#include <ESP8266HTTPClient.h> // Usa <HTTPClient.h> para ESP32

// Configura TinyGPS++ y el módulo GPS
TinyGPSPlus gps;
SoftwareSerial gpsSerial(D5, D6);  // RX en pin 4, TX en pin 3

// Configura tus credenciales WiFi
const char* ssid = "Fibertel WiFi664 2.4GHz";
const char* password = "01413044670";

// URL del servidor Node.js
const String serverURL = "http://192.168.0.7:3000/actualizar";

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  WiFi.begin(ssid, password);

  // Conectar al WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando al WiFi...");
  }
  Serial.println("Conectado al WiFi!");
}

/* void loop() {
  // Verifica si hay datos disponibles del GPS
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());

    // Si hay nuevas coordenadas, enviarlas al servidor
    if (gps.location.isUpdated()) {
      float latitude = gps.location.lat();
      float longitude = gps.location.lng();
      Serial.println(latitude);
      Serial.println(longitude);
      enviarCoordenadas(latitude, longitude);
    }
    else{
      Serial.println("Esperando datos del gps");
    }
  }
} */

void loop() {
  // Verifica si hay datos disponibles del GPS
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    Serial.print(c);  // Imprime los datos en crudo del GPS

    // Intenta procesar los datos con TinyGPSPlus
    gps.encode(c);

    // Si hay nuevas coordenadas, enviarlas al servidor
    if (gps.location.isUpdated()) {
      float latitude = gps.location.lat();
      float longitude = gps.location.lng();
      Serial.println("Coordenadas:");
      Serial.print("Latitud: ");
      Serial.println(latitude, 6);
      Serial.print("Longitud: ");
      Serial.println(longitude, 6);
      enviarCoordenadas(latitude, longitude);
    }
  }
}


void enviarCoordenadas(float lat, float lng) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient wifiClient; // Crear el cliente WiFi
    HTTPClient http;
    
    // Pasar WiFiClient y URL al método begin
    http.begin(wifiClient, serverURL);

    // Cuerpo JSON con latitud y longitud
    String payload = "{\"lat\":" + String(lat, 6) + ", \"lng\":" + String(lng, 6) + "}";
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Respuesta del servidor: " + response);
    } else {
      Serial.println("Error en la petición HTTP: " + String(httpResponseCode));
    }
    http.end();
  } else {
    Serial.println("No conectado a WiFi");
  }
}
