#include <NMEAGPS.h>
#include <SoftwareSerial.h>
#include "MQTT.h"
#include "ArduinoJson.h" // Include the JSON library

MQTT mqtt;

#define RXPin 17
#define TXPin 16


#define AIN1 12  // Pin D12 en ESP32
#define AIN2 13  // Pin D13 en ESP32
#define BIN1 14  // Pin D14 en ESP32
#define BIN2 15  // Pin D15 en ESP32
#define STBY 2   // Pin D16 en ESP32
#define buzzPin 19
#define direcIzq 18
#define direcDer 4

void moverAdelante() {
  analogWrite(AIN1, 255); 
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 255); 
  analogWrite(BIN2, 0);
}

void moverAtras() {
  analogWrite(AIN1, 0); 
  analogWrite(AIN2, 255);
  analogWrite(BIN1, 0); 
  analogWrite(BIN2, 255);
}

void moverIzquierda() {
  analogWrite(AIN1, 0); 
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 255); 
  analogWrite(BIN2, 0);
}

void moverDerecha() {
  analogWrite(AIN1, 255); 
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 0); 
  analogWrite(BIN2, 0);
}

void moverAdelanteIzquierda() {
  analogWrite(AIN1, 191); 
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 255); 
  analogWrite(BIN2, 0);
}

void moverAdelanteDerecha() {
  analogWrite(AIN1, 255); 
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 191); 
  analogWrite(BIN2, 0);
}

void moverAtrasIzquierda() {
  analogWrite(AIN1, 0); 
  analogWrite(AIN2, 191);
  analogWrite(BIN1, 0); 
  analogWrite(BIN2, 255);
}

void moverAtrasDerecha() {
  analogWrite(AIN1, 0); 
  analogWrite(AIN2, 255);
  analogWrite(BIN1, 0); 
  analogWrite(BIN2, 191);
}

void parar() {
  analogWrite(AIN1, 0); 
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 0); 
  analogWrite(BIN2, 0);
}

void Buzzer() {
  analogWrite(buzzPin, 255);
  delay(100);
  analogWrite(buzzPin, 0);
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

void setup() {
  Serial.begin(115200);

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
    DynamicJsonDocument doc(256); // Adjust the size as needed
    deserializeJson(doc, msgs);
    String action = doc["action"];
    if(action == "stop"){
      parar();
    }
    else if(action == "N"){
      moverAdelante();
    }
    else if(action == "S"){
      moverAtras();
    }
    else if(action == "E"){
      moverDerecha();
    }
    else if(action == "O"){
      moverIzquierda();
    }
    else if(action == "NO"){
      moverAdelanteIzquierda();
    }
    else if(action == "NE"){
      moverAdelanteDerecha();
    }
    else if(action == "SO"){
      moverAtrasIzquierda();
    }
    else if(action == "SE"){
      moverAtrasDerecha();
    }
    else if(action == "pitar"){
      analogWrite(buzzPin, 255);
      delay(1000);
      analogWrite(buzzPin, 0);
    }
    else if(action == "preventivas"){
      analogWrite(direcIzq, 255);
      analogWrite(direcDer, 255);

      for(int i = 0; i < 5 ; i++){
        analogWrite(direcIzq, 255);
        analogWrite(direcDer, 255);
        delay(250);
        analogWrite(direcIzq, 0);
        analogWrite(direcDer, 0);
        delay(250);
        if(action == "preventivasoff"){
          break;
        }
      }

    }
    else if(action == "preventivasoff"){
      analogWrite(direcIzq, 0);
      analogWrite(direcDer, 0);
    }
    else if(action == "dirizq"){
      for(int i = 0; i < 10 ; i++){
        analogWrite(direcIzq, 255);
        delay(250);
        analogWrite(direcIzq, 0);
        delay(250);
        if(action != "dirizq"){
          break;
        }
      }
    }
    else if(action == "dirder"){
      for(int i = 0; i < 10 ; i++){
        analogWrite(direcDer, 255);
        delay(250);
        analogWrite(direcDer, 0);
        delay(250);
        if(action != "dirder"){
          break;
        }
      }
    }


  mqtt.reconnect_MQTT();
  
}


