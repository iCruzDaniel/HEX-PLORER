#include <Stepper.h>
#include <Wire.h>
#include "Adafruit_VL53L0X.h"

#define STEPS 2038 // the number of steps in one revolution of your motor (28BYJ-48)
const int stepsPerRevolution = 2038;

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

long duration;
int distance;
int roundAngle;
int Angle;
Stepper stepper(STEPS, 8, 10, 9, 11);

int stepCount = 0;
bool lastSensorState = LOW;
const int sensorPin = 2; // Pin del sensor Hall

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(sensorPin, INPUT); // Configurar el pin del sensor como entrada
  if (!lox.begin()) {
    Serial.println(F("Error al iniciar VL53L0X"));
    while(1);
  }
}

void loop() {
  stepper.setSpeed(4);
  stepper.step(5);
  distance = calculateDistance();
  delay(0);
  
  // Verificar la detección del sensor Hall
  bool currentSensorState = digitalRead(sensorPin);
  if (currentSensorState == HIGH && lastSensorState == LOW) {
    stepCount = 0; // Restablecer el contador de pasos a 0 cuando el sensor Hall detecta el imán por primera vez
  }
  lastSensorState = currentSensorState;
  
  Angle = stepCount * 0.883218842;
  roundAngle = (int)Angle;
  Serial.print(Angle);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(".");
  stepCount++;

  if (stepCount == 408) {
    stepCount = 0;
  }
}

int calculateDistance() {
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  
  if (measure.RangeStatus != 4) {
    return measure.RangeMilliMeter / 10; // convertir mm a cm
  } else {
    return -1; // Devolver -1 si está fuera de rango
  }
}
