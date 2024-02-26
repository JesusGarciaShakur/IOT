
#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>


#include <addons/TokenHelper.h>


#include <addons/RTDBHelper.h>

// aqui van a poner el nombre de su red y contraseña 
#define WIFI_SSID "nombreRed"
#define WIFI_PASSWORD "COntraseña"



/* 2. API Key de base de datos  */
#define API_KEY "AIzaSyC_9W1GIvU0ZDYpy4_WDD61jWcKnf7B6EQ"

/* 3. Define la RTDB URL */
#define DATABASE_URL "iot-fire-7e935-default-rtdb.firebaseio.com"

// actual mente este funciona con correo definido desde la base de datos,se tendra que establecer mediante la info de la app movil 
#define USER_EMAIL "correo@gmail.com"
#define USER_PASSWORD "12345678"


// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

void setup()
{

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1800);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  config.token_status_callback = tokenStatusCallback; 

  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);/////////////////////////////////////////////////////////////////////////////////

  Firebase.begin(&config, &auth);

  Firebase.setDoubleDigits(5);



  // ingresar el comando de del circuito

  
}

void loop()
{

  // Firebase.ready() debe de ser usado constantemente para que autentifique la conexion como editor 


// 
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 10000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    Serial.printf("enviando bool... %s\n", Firebase.setBool(fbdo, F("/test/bool"), count % 2 == 0) ? "ok" : fbdo.errorReason().c_str());

    bool bVal;

    // metodo set del Firebase.setInt manda cosas a la base de datos
       Serial.printf("enviando int... %s\n", Firebase.setInt(fbdo, F("/test/int"), count) ? "ok" : fbdo.errorReason().c_str());

       // metodo get en Firebase.get trae cosas de la base 
      Serial.printf("obteniendo int... %s\n", Firebase.getInt(fbdo, F("/test/int")) ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
    


    int iVal = 0;

    Serial.printf("enviando float... %s\n", Firebase.setFloat(fbdo, F("/test/float"), count + 10.2) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("Set double... %s\n", Firebase.setDouble(fbdo, F("/test/double"), count + 35.517549723765) ? "ok" : fbdo.errorReason().c_str());
    Serial.printf("enviando string... %s\n", Firebase.setString(fbdo, F("/test/string"), "Hello!") ? "ok" : fbdo.errorReason().c_str());


// si llegamos a necesitar un archivo json en la base de datos con este comando se hacre   
    FirebaseJson json;

    if (count == 0)
    {
      json.set("value/round/" + String(count), F("cool!"));
      json.set(F("vaue/ts/.sv"), F("timestamp"));
      Serial.printf("Set json... %s\n", Firebase.set(fbdo, F("/test/json"), json) ? "ok" : fbdo.errorReason().c_str());
    }
    else
    {
      json.add(String(count), "smart!");
      Serial.printf("acualizando el nodo del json... %s\n", Firebase.updateNode(fbdo, F("/test/json/value/round"), json) ? "ok" : fbdo.errorReason().c_str());
    }

    Serial.println();

    count++;
  }
}



 /* 
 * while (!Firebase.ready()) <---- Nunca hagan este loop segun documentacion 
 * {
 *     delay(1000);
 * }
 *
 */