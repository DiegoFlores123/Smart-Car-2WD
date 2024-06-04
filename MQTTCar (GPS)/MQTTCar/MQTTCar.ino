#include <NMEAGPS.h>
#include <SoftwareSerial.h>
#include "MQTT.h"
#include "ArduinoJson.h"

MQTT mqtt;

#define RXPin 17
#define TXPin 16

SoftwareSerial gpsSerial(RXPin, TXPin);

NMEAGPS gps;

float latitude, longitude; // Variables para las coordenadas del carrito
float targetLatitude, targetLongitude; // Variables para las coordenadas objetivo
bool targetReceived = false; // Indicador de si se recibieron las coordenadas objetivo
bool emergencyStop = false; // Indicador de freno de emergencia

#define AIN1 12  // Pin D12 en ESP32
#define AIN2 13  // Pin D13 en ESP32
#define BIN1 14  // Pin D14 en ESP32
#define BIN2 15  // Pin D15 en ESP32
#define STBY 2   // Pin D16 en ESP32
#define buzzPin 19
#define direcIzq 18
#define direcDer 4

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600);

  Serial.println("Conexión al GPS establecida.");

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH); // Habilitar motores

  mqtt.setup_WiFi();
  mqtt.set_MQTT_server();
  mqtt.set_MQTT_callback();
  Buzzer();
}

void loop() {

  DynamicJsonDocument doc(256);
  deserializeJson(doc, msgs);
  String action = doc["action"];

  if(action == "stop"){
    stopCar();
  }

  if (emergencyStop) {
    parar();
    encenderPrev();
    return; // Para que no siga ejecutando el resto del loop
  }

  // Captura de las coordenadas del carrito
  if (gps.available(gpsSerial)) {
    gps_fix fix = gps.read();

    if (fix.valid.location) {
      latitude = fix.latitude();
      longitude = fix.longitude();

      Serial.print("Latitud: ");
      Serial.println(latitude, 6);
      Serial.print("Longitud: ");
      Serial.println(longitude, 6);
      
      JSON();
      
      // Comprobar si se recibieron las coordenadas objetivo
      if (targetReceived) {
        Serial.println("4 Coordenadas recibidas");

        // Comprobar si estamos cerca del objetivo comparando las coordenadas
        if (isClose(latitude, longitude, targetLatitude, targetLongitude)) {
          parar();
          encenderPrev();
          Serial.println("¡Llegamos al destino!");
          Buzzer();
        } 
        else {

          Serial.println("Avanzando...");
          moverAdelante(); // Continuar avanzando hacia el objetivo
          apagarLeds();
          
          // Si se está alejando, girar a la izquierda por un 3/4 de segundo para reubicar el carrito
          if (carritoAlejandose(latitude, longitude, targetLatitude, targetLongitude)) {
            Serial.println("Girando a la izquierda para reubicación...");
            moverIzquierda();
            encenderLedIzquierdo();
            delay(750);
            apagarLeds();
            moverAdelante();
            delay(5000);
          }
        }
      }
    }
  }

  mqtt.reconnect_MQTT();
}

void moverAdelante() {
  analogWrite(AIN1, 255); 
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 255); 
  analogWrite(BIN2, 0);
}

void moverIzquierda() {
  analogWrite(AIN1, 0); 
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 255); 
  analogWrite(BIN2, 0);
}

void parar() {
  analogWrite(AIN1, 0); 
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 0); 
  analogWrite(BIN2, 0);
}

void encenderLedIzquierdo() {
  analogWrite(direcIzq, 255);
  analogWrite(direcDer, 0);
}

void encenderLedDerecho() {
  analogWrite(direcIzq, 0);
  analogWrite(direcDer, 255);
}

void apagarLeds() {
  analogWrite(direcIzq, 0);
  analogWrite(direcDer, 0);
}

void encenderPrev() {
  analogWrite(direcIzq, 255);
  analogWrite(direcDer, 255);
}

void Buzzer() {
  analogWrite(buzzPin, 255);
  delay(1000);
  analogWrite(buzzPin, 0);
}

// Funcion para capturar las coordenadas del mapa
void JSON(){
  DynamicJsonDocument doc(256); // Ajustar el size para el gusto del usuario

  deserializeJson(doc, msgs);
  targetLatitude = doc["x"];
  targetLongitude = doc["y"];
  if(targetLatitude != 0 && targetLongitude != 0){ 
    targetReceived = true;
  }
  Serial.print("Objetivo x: ");
  Serial.println(targetLatitude, 6);
  Serial.print("Objetivo y: ");
  Serial.println(targetLongitude, 6);
  delay(100);
}

// Función para comprobar si las coordenadas destino y del carrito están cerca
bool isClose(float lat1, float lon1, float lat2, float lon2) {
  float threshold = 0.00001;
  Serial.println(lat1);
  Serial.println(lat2);
  Serial.println(lon1);
  Serial.println(lon2);

  Serial.println(abs(lon1 - lon2), 10);
  Serial.println(abs(lat1 - lat2), 10);

  return (abs(lat1 - lat2) < threshold) && (abs(lon1 - lon2) < threshold);
}

// Función para comprobar si el carrito se está alejando del objetivo
bool carritoAlejandose(float lat1, float lon1, float lat2, float lon2) {
  static float lastLat = 0.0;
  static float lastLon = 0.0;

  bool banderaLejos = false;
  if (lastLat != 0.0 && lastLon != 0.0) {
    float distanciaActual = abs(lat1 - lat2) + abs(lon1 - lon2);
    float ultimaUbicacion = abs(lastLat - lat2) + abs(lastLon - lon2);
    banderaLejos = distanciaActual > ultimaUbicacion;
  }

  lastLat = lat1;
  lastLon = lon1;

  return banderaLejos;
}

// Funcion para el freno de emergencia
void stopCar() {
  emergencyStop = true;
}