#include <Stepper.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <SoftwareSerial.h>

#define RX_PIN 10
#define TX_PIN 11


#define STEPS 2048  // the number of steps in one revolution of your motor (28BYJ-48)
const int stepsPerRevolution = 2048;

//instancia de SoftwareSerial
SoftwareSerial PORT(RX_PIN, TX_PIN);

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

long duration;
int distance;
int roundAngle;
int Angle = 0;
Stepper stepper(STEPS, 2, 3, 4, 5);

int stepCount = 0;
bool lastSensorState = LOW;
const int sensorPin = 6;  // Pin del sensor Hall

const int sizeSend = 2;
String arrayToSend[sizeSend];
char delimiter = '|';  // Delimitador para separar los Strings en la trama

void setup() {
  Serial.begin(9600);
  PORT.begin(9600);
  Wire.begin();

  pinMode(sensorPin, INPUT);  // Configurar el pin del sensor como entrada

  /*if (!lox.begin()) 
  {
    Serial.println(F("Error al iniciar VL53L0X"));
    while(1);
  }*/
}
int i=0;
void loop() {
  //INICIO DE COMUNICACION
  //int data = 0;
  /*if (PORT.available()>0) {
    int data = PORT.parseInt();
    if (data) {*/
      //Serial.println(PORT.parseInt());  // Ver lo que se recibió
      if(i==50){

      arrayToSend[0] = String(distance);
      arrayToSend[1] = String(roundAngle);  //Establecer Arraya a mandar
      //PORT.write(Angle); //Envíar a Maestro
      //PORT.write(distance); //Envíar a Maestro
      //Serial.write(PORT.read());
      sendArray(arrayToSend, sizeSend);
      i=0;
      }
      i++;
  //FIN DE COMUNICACION

  stepper.setSpeed(4);
  stepper.step(5);
  distance = calculateDistance();

  // Verificar la detección del sensor Hall
  bool currentSensorState = digitalRead(sensorPin);
  if (currentSensorState == HIGH && lastSensorState == LOW) {
    stepCount = 0;  // Restablecer el contador de pasos a 0 cuando el sensor Hall detecta el imán por primera vez
  }

  lastSensorState = currentSensorState;

  Angle = stepCount * 0.883218842;
  roundAngle = (int)Angle;
  Serial.println(Angle);
  //Serial.print(",");
  Serial.println(distance);
  //Serial.print(".");
  //delay(50);


  stepCount++;

  if (stepCount == 408) {
    stepCount = 0;
  }
  
}

int calculateDistance() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) {
    return measure.RangeMilliMeter / 10;  // convertir mm a cm
  } else {
    return -1;  // Devolver -1 si está fuera de rango
  }
}

void sendArray(String arr[], int size) {
  String serializedData = "";
  for (int i = 0; i < size; i++) {
    serializedData += arr[i];
    if (i < size - 1) {
      serializedData += delimiter;  // Añadir delimitador entre los Strings
    }
  }
  PORT.println(serializedData);  // Enviar la cadena serializada a través del Bluetooth
}
