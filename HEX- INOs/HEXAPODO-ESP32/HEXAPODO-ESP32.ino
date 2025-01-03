//------------------- LIBRERIAS -------------------
//------------------- Lib Arduino -------------------
#include <Arduino.h>
//------------------- Lib Comunicación I2C -------------------
//#include <I2Cdev.h>
#include <Wire.h>
//------------------- Lib Tarjetas de Contorl Servos -------------------
#include <Adafruit_PWMServoDriver.h>
//------------------- Lib Comunicación I2C -------------------
//#include <I2Cdev.h>
//------------------- Lib Acelerómetro -------------------
#include <MPU6050.h>
//------------------- Lib Motor Paso a Paso -------------------
#include <Stepper.h>
//------------------- Lib Sensor VL53L0X -------------------
#include "Adafruit_VL53L0X.h"
//------------------- Lib Para el Bluetooth -------------------
//#include <SoftwareSerial.h>
#include <HardwareSerial.h>


//#include <TimerOne.h>  // Incluir la biblioteca TimerOne


//---------------- DEFINICIONES -------------------
#define STEPS 2048 // the number of steps in one revolution of your motor (28BYJ-48)
#define BTTx 11
#define BTRx 10
//---------------- Asignación de Pines -------------------
/*
*
*
*
*
*
*/
//---------------- OBJETOS DE MANIPULACION -------------------
//------------------- Obj Segundo Nucleo -------------------
TaskHandle_t Core2;
//------------------- Obj Servos -------------------
Adafruit_PWMServoDriver servos1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver servos2 = Adafruit_PWMServoDriver(0x41);
//------------------- Obj Acelerómetro -------------------
MPU6050 sensor;
//------------------- Obj Lidar -------------------
//Adafruit_VL53L0X lox = Adafruit_VL53L0X();
//------------------- Obj Paso a Paso -------------------
//Stepper stepper(STEPS, 2, 4, 3, 5);
//------------------- Obj Comunicaciones Seriales -------------------
//SoftwareSerial BT(BTRx, BTTx); // RX, TX
HardwareSerial RADAR(0); //RADAR
HardwareSerial BT(2); //BLUETOOTH

//----------------INICIO: VARIABLES-------------------
//----------------Control Servos-------------------
unsigned int pos0 = 172;
unsigned int pos180 = 565;
int tiempo = 2000;
// Variables para el filtro complementario
float ang_x = 0, ang_y = 0;
float ang_x_prev = 0, ang_y_prev = 0;
float dt = 0;
unsigned long tiempo_prev = 0;

//---------------- Comunicación Radar -------------------
int distance;
int Angle;

//---------------- Comunicación Bluetooth -------------------
const int sizeSend = 4;
const int sizeReceive = 6;
String arrayToSend[sizeSend] = {"String1", "String2", "String3", "String4"};
String arrayReceived[sizeReceive];
char delimiter = '|';  // Delimitador para separar los Strings en la trama

//---------------- Control De Modos -------------------
bool changeMode = false;
int mode = 1;

int X1 = 0; 
int Y1 = 0; 
int X2 = 0; 
int Y2 = 0; 
//---------------- Estado de Batería -------------------
int bateria;
//---------------- Tiempo de Actividad -------------------
long int tiempoActivo;
//----------------FIN: VARIABLES-------------------
//
//
//
//
//
//
//-------------------- FUNCIONES --------------------

//---------- Funciones de Caminado ---------- funcion core "avanzar(arg, arg, arg);"
void setServo1(uint8_t n_servo, int angulo)
{
  int duty;
  duty = map(angulo, 0, 180, pos0, pos180);
  servos1.setPWM(n_servo, 0, duty);
}

void setServo2(uint8_t n_servo, int angulo)
{
  int duty;
  duty = map(angulo, 0, 180, pos0, pos180);
  servos2.setPWM(n_servo, 0, duty);
}

void arranque(int x, int d, int t) // cuando el hexapodo se queda se pone de pie al prender
{
  setServo1(0, 90 + x); setServo1(1, 90); setServo1(2, 90);     // motor 2
  setServo1(3, 90); setServo1(4, 90); setServo1(5, 90);         // motor 4
  setServo1(6, 90 - x); setServo1(7, 160); setServo1(8, 90);     // motor 6
  
  setServo2(0, 90 + x); setServo2(1, 90); setServo2(2, 90);     // motor 1
  setServo2(3, 90); setServo2(4, 90); setServo2(5, 90);         // motor 3
  setServo2(6, 90 - x); setServo2(7, 90); setServo2(8, 90);     // motor 5
  delay(t);
}

void adelante1(int x, int d, int t) // patas 2, 3, 6 movimiento hacia delante
{
  for (int i = 0; i < 5; i++) {
    if (i == 0) {
      setServo1(0, 90 + x); setServo1(1, 90); setServo1(2, 90); // motor 2
      setServo2(3, 90 - d); setServo2(4, 90 + d); setServo2(5, 90); // motor 3
      setServo1(6, 90 + d - x); setServo1(7, 90 + d); setServo1(8, 90); // motor 6
    } else if (i == 1) {
      setServo1(0, 90 + x); setServo1(1, 90); setServo1(2, 90); // motor 2
      setServo2(3, 90 - d); setServo2(4, 90); setServo2(5, 90); // motor 3
      setServo1(6, 90 + d - x); setServo1(7, 90); setServo1(8, 90); // motor 6
    } else if (i == 2) {
      setServo1(0, 90 + x - d); setServo1(1, 90); setServo1(2, 90); // motor 2
      setServo2(3, 90); setServo2(4, 90); setServo2(5, 90); // motor 3
      setServo1(6, 90 - x); setServo1(7, 90); setServo1(8, 90); // motor 6
    } else if (i == 3) {
      setServo1(0, 90 + x); setServo1(1, 90 + d); setServo1(2, 90); // motor 2
      setServo2(3, 90); setServo2(4, 90); setServo2(5, 90); // motor 3
      setServo1(6, 90 - x); setServo1(7, 90); setServo1(8, 90); // motor 6
    } else if (i == 4) {
      setServo1(0, 90 + x); setServo1(1, 90); setServo1(2, 90); // motor 2
      setServo2(3, 90); setServo2(4, 90); setServo2(5, 90); // motor 3
      setServo1(6, 90 - x); setServo1(7, 90); setServo1(8, 90); // motor 6
    }
    delay(t);
  }
}

void adelante2(int x, int d, int t) // patas 1, 4, 5 movimiento hacia adelante
{
  for (int i = 0; i < 5; i++) {
    if (i == 0) {
      setServo2(6, 90 - x); setServo2(7, 90); setServo2(8, 90); // motor 1
      setServo1(3, 90 + d); setServo1(4, 90 + d); setServo1(5, 90); // motor 4
      setServo2(0, 90 - d + x); setServo2(1, 90 + d); setServo2(2, 90); // motor 5
    } else if (i == 1) {
      setServo2(6, 90 - x); setServo2(7, 90); setServo2(8, 90); // motor 1
      setServo1(3, 90 + d); setServo1(4, 90); setServo1(5, 90); // motor 4
      setServo2(0, 90 - d + x); setServo2(1, 90); setServo2(2, 90); // motor 5
    } else if (i == 2) {
      setServo2(6, 90 - x + d); setServo2(7, 90); setServo2(8, 90); // motor 1
      setServo1(3, 90); setServo1(4, 90); setServo1(5, 90); // motor 4
      setServo2(0, 90 + x); setServo2(1, 90); setServo2(2, 90); // motor 5 
    } else if (i == 3) {
      setServo2(6, 90 - x); setServo2(7, 90 + d); setServo2(8, 90); // motor 1
      setServo1(3, 90); setServo1(4, 90); setServo1(5, 90); // motor 4
      setServo2(0, 90 + x); setServo2(1, 90); setServo2(2, 90); // motor 5
    } else if (i == 4) {
      setServo2(6, 90 - x); setServo2(7, 90); setServo2(8, 90); // motor 1
      setServo1(3, 90); setServo1(4, 90); setServo1(5, 90); // motor 4
      setServo2(0, 90 + x); setServo2(1, 90); setServo2(2, 90); // motor 5
    }
    delay(t);
  }
}

void avanzar(int x, int d, int t) {
  adelante1(x, d, t);
  adelante2(x, d, t);
}

void giro(int x, int d, int t)
{
  arranque(30, 30, 100);

  // pata 1
  setServo2(6, 90 - x + d); setServo2(7, 90 + d); setServo2(8, 90); // paso 1
  delay(t);
  setServo2(6, 90 - x + d); setServo2(7, 90); setServo2(8, 90); // paso 2
  delay(t);

  // pata 2
  setServo1(0, 90 + x + d); setServo1(1, 90 + d); setServo1(2, 90); // paso 1
  delay(t);
  setServo1(0, 90 + x + d); setServo1(1, 90); setServo1(2, 90); // paso 2
  delay(t);

  // pata 3
  setServo2(3, 90 + d); setServo2(4, 90 + d); setServo2(5, 90); // paso 1
  delay(t);
  setServo2(3, 90 + d); setServo2(4, 90); setServo2(5, 90); // paso 2
  delay(t);

  // pata 4
  setServo1(3, 90 + d); setServo1(4, 90 + d); setServo1(5, 90); // paso 1
  delay(t);
  setServo1(3, 90 + d); setServo1(4, 90); setServo1(5, 90); // paso 2
  delay(t);

  // pata 5
  setServo2(0, 90 + x + d); setServo2(1, 90 + d); setServo2(2, 90); // paso 1
  delay(t);
  setServo2(0, 90 + x + d); setServo2(1, 90); setServo2(2, 90); // paso 2
  delay(t);

  // pata 6
  setServo1(6, 90 - x + d); setServo1(7, 90 + d); setServo1(8, 90); // paso 1
  delay(t);
  setServo1(6, 90 - x + d); setServo1(7, 90); setServo1(8, 90); // paso 2
  delay(t);

  arranque(30, 30, 100);
}

//---------- Funcion de Estabilidad ----------

void balance(){
  // Tomar datos del acelerómetro y giroscopio
  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  sensor.getAcceleration(&ax, &ay, &az);
  sensor.getRotation(&gx, &gy, &gz);
  float accel_ang_x = atan(ay / sqrt(pow(ax, 2) + pow(az, 2))) * (180.0 / 3.14);
  float accel_ang_y = atan(-ax / sqrt(pow(ay, 2) + pow(az, 2))) * (180.0 / 3.14);
  
  // Calcular el tiempo transcurrido
  dt = (millis() - tiempo_prev) / 1000.0;
  tiempo_prev = millis();
  
  // Calcular ángulo de rotación con filtro complementario
  ang_x = 0.98 * (ang_x_prev + (gx / 131.0) * dt) + 0.02 * accel_ang_x;
  ang_y = 0.98 * (ang_y_prev + (gy / 131.0) * dt) + 0.02 * accel_ang_y;
  
  // Actualizar los ángulos previos
  ang_x_prev = ang_x;
  ang_y_prev = ang_y;


  //hacemos un mapeo del valor leido del giroscopio y lo mapeamos junto a los angulos de los servos.
////////////////////////////////////////////////////////////////////////////////////////////////////
  //patas del lado izquierdo 
  float valxizq = ang_x;
  valxizq = map(valxizq, -45, 45, 10, 170); ///Las patas iaquierdas el mapeo tiene que ir de 0/180

  //patas del lado derecho
  float valxder = ang_x;
  valxder = map(valxder, -45, 45, 170, 10); ///las patas derechas el mapeo tiene que ir de 180/0
/////////////////////////////////////////////////////////////////////////////////////////////////////
  //patas delanteras
  float valydel = ang_y;
  valydel= map(valydel, -40, 20, 30, 70); ///las patas delanteras el mapeo tiene que ir de 180/0 

  //patas traseras
  float valytras = ang_y;
  valytras= map(valytras, -40, 20, 150, 110); ///las patas traseras el mapeo tiene que ir de 0/180
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  Serial.print("Rotacion en X:  ");
  Serial.print(ang_x); 
  Serial.print("tRotacion en Y: ");
  Serial.println(ang_y);

  delay(10);

  //////////////////////////////////////////////////////////////////////////////////////////////////
  //if 1 modo Y 
  //if( -20 > ang_x > 20){

 /////////////////////////////////////////////////////////////////////////////////////////////////////
 //if 2 modo X
 //if(-20 < ang_y < 20){
  setServo2(6, 30); setServo2(7, valxder); setServo2(8, valxder);  

  //pata 2 izq delantero 
  setServo1(0, 150); setServo1(1, valxizq); setServo1(2, valxizq);

  //pata 3 der medio
  setServo2(3, 90); setServo2(4, valxder); setServo2(5, valxder);  

  //pata 4 izq medio
  setServo1(3, 90); setServo1(4, valxizq); setServo1(5, valxizq); 
  //pata 5 der trasero
  setServo2(0, valytras); setServo2(1, valxder); setServo2(2, valxder); 

  //pata 6 izq trasero
  setServo1(6, valydel); setServo1(7, valxizq); setServo1(8, valxizq);
  delay(100);
}

/*
void radar() {
  
  stepper.setSpeed(4);
  stepper.step(5);
  distance = calculateDistance();
  delay(0);
  
  // Verificar la detección del sensor Hall
  bool currentSensorState = digitalRead(sensorPin);
  if (currentSensorState == HIGH && lastSensorState == LOW) 
  {
    stepCount = 0; // Restablecer el contador de pasos a 0 cuando el sensor Hall detecta el imán por primera vez
  }

  lastSensorState = currentSensorState;
  
  Angle = stepCount * 0.883218842;
  roundAngle = (int)Angle;
  //Serial.print(Angle);
  //Serial.print(",");
  //Serial.print(distance);
  //Serial.print(".");
  stepCount++;

  if (stepCount == 408) 
  {
    stepCount = 0;
  }
}
*/

/*
int calculateDistance() 
{
  VL53L0X_RangingMeasurementData_t measure;
  lox.rangingTest(&measure, false);
  
  if (measure.RangeStatus != 4) 
  {
    return measure.RangeMilliMeter / 10; // convertir mm a cm
  } 
  else 
  {
    return -1; // Devolver -1 si está fuera de rango
  }
}
*/

//-------------------------------------
// Función para convertir int a boolean
bool stringToBoolean(String str) {
  str.toLowerCase();
  if (str == "true" || str == "1" || str == "yes" || str == "on") {
    return true;
  }
  if (str == "false" || str == "0" || str == "no" || str == "off") {
    return false;
  }
  return false;
}
//-------------------------------------
//
//
//
//
//
//
//---------------- INICIO: SETUP --------------------


void setup() 
{

  //INICIALIZACIÓN DE COMUNICACIONES
  Wire.begin();
  Serial.begin(9600);
  BT.begin(9600, SERIAL_8N1, 16, 17);
  RADAR.begin(9600, SERIAL_8N1, 3, 1);

  //CONFIG SERVOS
  servos1.begin();
  servos1.setPWMFreq(60);  // Frecuencia de PWM en Hz
  servos2.begin();
  servos2.setPWMFreq(60);  // Frecuencia de PWM en Hz
  //arranque(60, 60, 500);
  
  //CONFIG ACELERÓMETRO
  //sensor.initialize();


  //CONFIG EMOTES...


  // Inicializar el temporizador con un periodo de 1 segundo (1000000 microsegundos)
  //Timer1.initialize(5000);
  
  // Adjuntar la función de interrupción que se ejecutará cada 1 segundo
  //Timer1.attachInterrupt(radarInterruptor);

}
//---------------- FIN: SETUP --------------------
//
//
//
//
void loop() {
  //core();
  //arranque(60, 60, 500);
  RADAR.print("data");
  if(RADAR.available()) {
    Serial.println(RADAR.read());
    Serial.println(RADAR.read());
    delay(1000);

  }
}



//
//
//
//
//
//---------------- INICIO: CONTROLADOR DE MODOS --------------------
void core() {

  /*if(radarInterruptorFlag) {
    radarInterruptorFlag = false;
  }*/
    //radar();
    //avanzar(35,35, 200);
  //balance();

    //ENTRADA SERIAL: LEYENDO DATOS DEL MANDO...
    if (BT.available() > 0) {
    String receivedData = BT.readStringUntil('\n');
    receiveArray(receivedData, arrayReceived, sizeReceive);
    BT.flush(); 

    /*changeMode = stringToBoolean(arrayReceived[0]);
    mode = (arrayReceived[1]).toInt();
    X1 =   (arrayReceived[2]).toInt(); 
    Y1 =   (arrayReceived[3]).toInt(); 
    X2 =   (arrayReceived[4]).toInt(); 
    Y2 =   (arrayReceived[5]).toInt(); */
    
    // Imprimir el array recibido para verificar
    //Control de Modos.
    /*if(changeMode) {
      while(false);
    }*/
  }
    for (int i = 0; i < sizeReceive; i++) {
      Serial.println(arrayReceived[i]);
    }


  //arrayToSend[4] = String(tempMicro);

  /* LÓGICA DEL PROGRAMA
  *
  *
  *
  *
  */

  //SALIDA SERIAL: ENVIADO DE VARIABLES AL CONTROL...
  /*arrayToSend[0] = String(bateria);
  arrayToSend[1] = String(distance);
  arrayToSend[2] = String(Angle);
  arrayToSend[3] = String(tiempoActivo);*/

  delay(1000);
  sendArray(arrayToSend, sizeSend);
  BT.flush(); // Asegurarse de que el mensaje se ha enviado completamente

}
//---------------- FIN: CONTROLADOR DE MODOS --------------------
//
//
//
//
//
/*
void radarInterruptor() {
  radarInterruptorFlag = true;
}*/
//
//
//
//
//
//---------------- INICIO: FUNCIONES CONTROL DE BLUETOOTH --------------------

void sendArray(String arr[], int size) {
  String serializedData = "";
  for (int i = 0; i < size; i++) {
    serializedData += arr[i];
    if (i < size - 1) {
      serializedData += delimiter;  // Añadir delimitador entre los Strings
    }
  }
  BT.println(serializedData);  // Enviar la cadena serializada a través del Bluetooth
}

void receiveArray(String data, String arr[], int size) {
  int startIndex = 0;
  int endIndex = data.indexOf(delimiter);

  for (int i = 0; i < size; i++) {
    if (endIndex == -1) {
      arr[i] = data.substring(startIndex);
      break;
    } else {
      arr[i] = data.substring(startIndex, endIndex);
      startIndex = endIndex + 1;
      endIndex = data.indexOf(delimiter, startIndex);
    }
  }
}
//---------------- FIN: FUNCIONES CONTROL DE BLUETOOTH --------------------


