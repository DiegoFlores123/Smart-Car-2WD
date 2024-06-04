#define NUM_SENSORS 4
const int sensorPins[NUM_SENSORS] = {34, 35, 32, 33};
int buttonMode = 0; // 0 es apagado, 1 es seguidor, 2 es evitador

#define AIN1 12  // Pin D12 en ESP32
#define AIN2 13  // Pin D13 en ESP32
#define BIN1 14  // Pin D14 en ESP32
#define BIN2 15  // Pin D15 en ESP32
#define STBY 2  // Pin D16 en ESP32
#define buttonPin 23
#define direcIzq 18
#define direcDer 4
#define buzzPin 19
#define TRIGGER 21
#define ECHO 22
#define VELOCIDAD_SONIDO 0.034 

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(sensorPins[i], INPUT);
  }

  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  pinMode(direcIzq, OUTPUT);
  pinMode(direcDer, OUTPUT);
  digitalWrite(STBY, HIGH); // Habilitar motores
  pinMode ( TRIGGER, OUTPUT ); 
  pinMode ( ECHO, INPUT ); 
  pinMode ( buttonPin, INPUT );

  Serial.println(buttonMode);

}

// Sensor 1: Izquierda, Sensor 2: Izquierda medio, Sensor 3: Derecha medio, Sensor 4: Derecha
void loop() {

  if (digitalRead(buttonPin)){
    buttonMode++;
    analogWrite(buzzPin, 255);

    if(buttonMode > 2){
    buttonMode = 0 ;
    Serial.println("Regresamos a 0");
    parar();
    apagarLeds();
    }

    Serial.println(buttonMode);
    delay(500);
    analogWrite(buzzPin, 0);

  }

  if(buttonMode == 1) {
    int sensorValues[NUM_SENSORS];
    bool sensorFlags[NUM_SENSORS];
    int limite = 4000;  // Ajusta el limite a 4000, arriba de ese valor es porque el color negro es detectado por algun sensor

    for (int i = 0; i < NUM_SENSORS; i++) {
      sensorValues[i] = analogRead(sensorPins[i]);
      sensorFlags[i] = (sensorValues[i] > limite);
      Serial.print("Sensor ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(sensorValues[i]);
      Serial.print("\t");
    }
    Serial.println();
    delay(50);

    // Valores de velocidad
    int refuerzo = 176;
    int velocidadNormal = 150;

      analogWrite(AIN1, 143);  
      analogWrite(AIN2, 0);
      analogWrite(BIN1, 150);  
      analogWrite(BIN2, 0);
      analogWrite(direcIzq, 0);
      analogWrite(direcDer, 0);

    // Control del motor basado en las banderas de los sensores
    if (sensorFlags[3] || sensorFlags[2]) {  // Sensor derecho o derecho medio detecta negro
      // Reducir velocidad del motor izquierdo para girar a la derecha
      analogWrite(AIN1, 0);  // Velocidad reducida
      analogWrite(AIN2, 0);
      analogWrite(BIN1, refuerzo);  // Velocidad alta
      analogWrite(BIN2, 0);
      analogWrite(direcIzq, 255);

    } else if (sensorFlags[0] || sensorFlags[1]) {  // Sensor izquierdo o izquierdo medio detecta negro
      // Reducir velocidad del motor derecho para girar a la izquierda
      analogWrite(AIN1, velocidadNormal);  // Velocidad alta
      analogWrite(AIN2, 0);
      analogWrite(BIN1, 0);  // Velocidad reducida
      analogWrite(BIN2, 0);
      analogWrite(direcDer, 255);

    } 
  }

  else if(buttonMode == 2){

    float limitecol = obtener_distancia();
    Serial.print ( "Distancia en cm: ");
    Serial.println (limitecol);


    if (limitecol < 30){
      analogWrite(AIN1, 0); 
      analogWrite(AIN2, 0);
      analogWrite(BIN1, 226); 
      analogWrite(BIN2, 0);
      analogWrite(direcIzq, 255);
      analogWrite(direcDer, 255);
    }
    else{
      analogWrite(AIN1, 191); 
      analogWrite(AIN2, 0);
      analogWrite(BIN1, 200); 
      analogWrite(BIN2, 0);
      analogWrite(direcIzq, 0);
      analogWrite(direcDer, 0);
    }
    
  }

}

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

void parar() {
  analogWrite(AIN1, 0); 
  analogWrite(AIN2, 0);
  analogWrite(BIN1, 0); 
  analogWrite(BIN2, 0);
}

void apagarLeds() {
  analogWrite(direcIzq, 0);
  analogWrite(direcDer, 0);
}

float obtener_distancia ( void ) {

    digitalWrite ( TRIGGER, LOW );
    delayMicroseconds ( 2 );
    digitalWrite ( TRIGGER, HIGH );
    delayMicroseconds ( 10 );
    digitalWrite ( TRIGGER, LOW );

    return ( pulseIn ( ECHO, HIGH ) * VELOCIDAD_SONIDO / 2 );
}