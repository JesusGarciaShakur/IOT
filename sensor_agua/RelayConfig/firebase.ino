#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define API_KEY "AIzaSyCGeO9X01brIfhPZphJ2NupsBHHqZL14ps"
#define DATABASE_URL "newiot-632c1-default-rtdb.firebaseio.com"
#define USER_EMAIL "correo@gmail.com" // aqui se tendra que cambiar el correo segun la info de la app Movil
#define USER_PASSWORD "12345678"

#define TRIG_PIN 23 // ESP32 pin GPIO23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 22 // ESP32 pin GPIO22 connected to Ultrasonic Sensor's ECHO pin

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

float duration_us, distance_cm;
int relayState;
int relayPin = 16;  //--> Pin del relay

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(relayPin, OUTPUT);  //--> Establecer el pin como salida
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1800);
  }

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
  Firebase.setInt(fbdo, F("/sensor/relay"), relayState); //--> Establecer el valor por defecto en la base de datos
  Serial.println("Datos enviados");
  Serial.println(relayState);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

}

void loop() {

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration_us = pulseIn(ECHO_PIN, HIGH);
  distance_cm = 0.012 * duration_us;
  Firebase.setFloat(fbdo, F("/sensor/ultrasonico"), distance_cm); // Enviar la distancia a Firebase
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
  delay(1000);

  Serial.printf("obteniendo int... %s\n", Firebase.getInt(fbdo, F("/sensor/relay")) ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str()); //--> Consultar el valor registrado del relay
  Serial.println("Datos originales: ");
  Serial.println(relayState);
  relayState = fbdo.to<int>();  //--> Guardar la consulta de Firebase en una variable
  Serial.println("Datos modificados: ");
  Serial.println(relayState);

  if (relayState == 1) {  //--> Encender y apagar el relay
    Serial.println("Estado: ON");
    digitalWrite(relayPin, LOW);

  } else if (relayState == 0) {
    Serial.println("Estado: OFF");
    digitalWrite(relayPin, HIGH);

  } else {
    Serial.println("Datos erróneos");
  }

  Serial.println();
  }