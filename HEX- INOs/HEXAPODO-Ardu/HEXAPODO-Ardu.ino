//------------------- LIBRERIAS -------------------
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
//------------------- Lib Para comunicación Bluetooth Y Lidar -------------------
#include <SoftwareSerial.h>

//#include <TimerOne.h>  // Incluir la biblioteca TimerOne


//---------------- DEFINICIONES -------------------
//#define STEPS 2038  // the number of steps in one revolution of your motor (28BYJ-48)
#define BTTx 11
#define BTRx 10
#define LIDARTx 3
#define LiDARRx 2
//---------------- Asignación de Pines -------------------
/*
*
*
*
*
*
*/
//---------------- OBJETOS DE MANIPULACION -------------------
//------------------- Obj Servos -------------------
Adafruit_PWMServoDriver servos1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver servos2 = Adafruit_PWMServoDriver(0x41);
//------------------- Obj Acelerómetro -------------------
MPU6050 sensor;
//------------------- Obj Comunicacion Lidar -------------------
SoftwareSerial LIDAR(LiDARRx, LIDARTx);  // RX, TX
//Adafruit_VL53L0X lox = Adafruit_VL53L0X();

//------------------- Obj Bluetooth -------------------
SoftwareSerial BT(BTRx, BTTx);  // RX, TX

/*/------------------- Obj Paso a Paso -------------------
Stepper stepper(STEPS, 2, 4, 3, 5);*/



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
int distance = 1;
int Angle = 1;

volatile bool radarInterruptorFlag = false;

//---------------- Comunicación Bluetooth -------------------
const int sizeSend = 5;
const int sizeReceive = 6;  //6;
String arrayToSend[sizeSend] = { "String1", "String2", "String3", "String4" };
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

//---------------- Número de Pasos -------------------
int steps;
//----------------FIN: VARIABLES-------------------
//
//
//
//-------------------- FUNCIONES --------------------

//---------- Funciones de Caminado ---------- funcion core "avanzar(arg, arg, arg);"
void setServo1(uint8_t n_servo, int angulo) {
  int duty;
  duty = map(angulo, 0, 180, pos0, pos180);
  servos1.setPWM(n_servo, 0, duty);
}

void setServo2(uint8_t n_servo, int angulo) {
  int duty;
  duty = map(angulo, 0, 180, pos0, pos180);
  servos2.setPWM(n_servo, 0, duty);
}

void despliegue(int t) {
  //------------C------------
  setServo2(8, 90);  //1C
  setServo1(2, 90);  //2C
  setServo2(5, 90);  //3C
  setServo1(5, 90);  //4C
  setServo2(2, 90);  //5C
  setServo1(8, 90);  //6C
  delay(t);

  //------------A------------
  setServo2(6, 90);  //1A
  setServo1(0, 90);  //2A
  setServo2(3, 90);  //3A
  setServo1(3, 90);  //4A
  setServo2(0, 90);  //5A
  setServo1(6, 90);  //6A
  delay(t);

  for (int i = 160; i >= 90; i -= 3) {
    //------------B------------
    setServo2(7, i);  //1B
    setServo1(1, i);  //2B
    setServo2(4, i);  //3B
    setServo1(4, i);  //4B
    setServo2(1, i);  //5B
    setServo1(7, i);  //6B

    // Pequeña pausa para suavizar el movimiento
    delay(30);

    // Verifica si todos los servomotores han alcanzado 90

    /*if (i == 90) 
    {
      estatico();
    }*/
  }
}

void estatico()  // cuando el hexapodo se queda se pone de pie al prender
{
  //------------Pata 1------------
  setServo2(6, 90);  //A
  setServo2(7, 90);  //B
  setServo2(8, 90);  //C

  //------------Pata 2------------
  setServo1(0, 90);  //A
  setServo1(1, 90);  //B
  setServo1(2, 90);  //C

  //------------Pata 3------------
  setServo2(3, 90);  //A
  setServo2(4, 90);  //B
  setServo2(5, 90);  //C

  //------------Pata 4------------
  setServo1(3, 90);  //A
  setServo1(4, 90);  //B
  setServo1(5, 90);  //C

  //------------Pata 5------------
  setServo2(0, 90);  //A
  setServo2(1, 90);  //B
  setServo2(2, 90);  //C

  //------------Pata 6------------
  setServo1(6, 90);  //A
  setServo1(7, 90);  //B
  setServo1(8, 90);  //C
}

void recogida() {
  //------------B------------
  setServo2(7, 95);  //1B
  setServo1(1, 95);  //2B
  setServo2(4, 95);  //3B
  setServo1(4, 95);  //4B
  setServo2(1, 95);  //5B
  setServo1(7, 95);  //6B
  delay(200);

  setServo2(7, 105);  //1B
  setServo1(1, 105);  //2B
  setServo2(4, 105);  //3B
  setServo1(4, 105);  //4B
  setServo2(1, 105);  //5B
  setServo1(7, 105);  //6B
  delay(200);

  setServo2(7, 115);  //1B
  setServo1(1, 115);  //2B
  setServo2(4, 115);  //3B
  setServo1(4, 115);  //4B
  setServo2(1, 115);  //5B
  setServo1(7, 115);  //6B
  delay(200);

  setServo2(7, 120);  //1B
  setServo1(1, 120);  //2B
  setServo2(4, 120);  //3B
  setServo1(4, 120);  //4B
  setServo2(1, 120);  //5B
  setServo1(7, 120);  //6B
  delay(200);

  setServo2(7, 160);  //1B
  setServo1(1, 160);  //2B
  setServo2(4, 160);  //3B
  setServo1(4, 160);  //4B
  setServo2(1, 160);  //5B
  setServo1(7, 160);  //6B
  delay(200);

  //------------A------------
  setServo2(6, 120);  //1A
  setServo1(0, 50);   //2A
  setServo2(3, 90);   //3A
  setServo1(3, 90);   //4A
  setServo2(0, 70);   //5A
  setServo1(6, 110);  //6A
  delay(500);

  //------------C------------
  setServo2(8, 160);  //1C
  setServo1(2, 160);  //2C
  setServo2(5, 160);  //3C
  setServo1(5, 160);  //4C
  setServo2(2, 160);  //5C
  setServo1(8, 160);  //6C
  delay(500);
}

void bordeInterno() {
  //Posicion
  setServo2(6, 40);  //1A antihorario
  setServo2(7, 20);  //1B Abajo
  setServo2(8, 40);  //1C Afuera

  setServo1(0, 140);  //2A horario
  setServo1(1, 20);   //2B Abajo
  setServo1(2, 40);   //2C Afuera

  setServo2(3, 60);  //3A antihorario
  setServo2(4, 50);  //3B Abajo

  setServo1(3, 120);  //4A horario
  setServo1(4, 50);   //4B Abajo

  setServo2(0, 120);  //5A horario
  setServo2(1, 110);  //5B Arriba
  setServo2(2, 60);   //5C Afuera

  setServo1(6, 30);   //6A antihorario
  setServo1(7, 110);  //6B Arriba
  setServo1(8, 120);  //6C Afuera

  delay(200);


  //Movimiento

  setServo2(2, 90);  //5C Adentro
  delay(200);

  setServo1(4, 10);   //4B Abajo
  setServo2(0, 90);   //5A antihorario
  setServo2(2, 120);  //5C Adentro
  setServo1(7, 140);  //6B Arriba
  delay(200);

  setServo2(0, 60);  //5A antihorario
  delay(200);

  setServo2(0, 90);  //5A antihorario
  setServo2(2, 90);  //5C Adentro
  delay(200);

  setServo1(4, 40);   //4B Arriba
  setServo2(0, 120);  //5A horario
  setServo2(2, 60);   //5C Afuera
  setServo1(7, 50);   //6B Abajo
  delay(200);
}

void saludar() {
  int t = 500;
  //Posicion
  estatico();
  delay(t);

  //Movimiento
  setServo2(7, 160);  //1B Abajo
  delay(t);

  setServo2(8, 40);  //1C Afuera
  delay(t);

  setServo2(6, 40);  //1A antihorario
  delay(t);

  setServo2(6, 140);  //1A Horario
  delay(t);

  setServo2(6, 40);  //1A antihorario
  delay(t);

  setServo2(6, 140);  //1A Horario
  delay(t);

  setServo2(6, 40);  //1A antihorario
  delay(500);

  setServo2(6, 140);  //1A Horario
  delay(t);

  setServo2(6, 90);  //1A Antihorario
  delay(t);

  estatico();
}

void baile() {
  int t = 500;
  int a = 30;
  int h = 30;

  // Levantar y adelantar patas del grupo 1, 3, 5 (derecho)
  // Levantar patas
  setServo2(7, 90 + h);  //1B arriba
  setServo2(4, 90 + h);  //3B arriba
  setServo2(1, 90 + h);  //5B arriba
  delay(t);

  // Adelantar patas
  setServo2(6, 90 + a);  //1A horario
  setServo2(3, 90 + a);  //3A horario
  setServo2(0, 90 + a);  //5A horario
  delay(t);

  // Bajar patas
  setServo2(7, 90 - h);  //1B abajo
  setServo2(4, 90 - h);  //3B abajo
  setServo2(1, 90 - h);  //5B abajo
  delay(t);

  // Retrasar patas
  setServo2(6, 90);  //1A antihorario
  setServo2(3, 90);  //3A antihorario
  setServo2(0, 90);  //5A antihorario
  delay(t);

  // Levantar y adelantar patas del grupo 2, 4, 6 (izquierdo)
  // Levantar patas
  setServo1(1, 90 + h);  //2B arriba
  setServo1(4, 90 + h);  //4B arriba
  setServo1(7, 90 + h);  //6B arriba
  delay(t);

  // Adelantar patas
  setServo1(0, 90 + a);  //2A horario
  setServo1(3, 90 + a);  //4A horario
  setServo1(6, 90 + a);  //6A horario
  delay(t);

  // Bajar patas
  setServo1(1, 90 - h);  //2B abajo
  setServo1(4, 90 - h);  //4B abajo
  setServo1(7, 90 - h);  //6B abajo
  delay(t);

  // Retrasar patas
  setServo1(0, 90);  //2A antihorario
  setServo1(3, 90);  //4A antihorario
  setServo1(6, 90);  //6A antihorario
  delay(t);
}

void giro(int a, int h, int t) {
  estatico();

  setServo2(7, 90 + h);  //1B Arriba
  setServo1(4, 90 + h);  //4B Arriba
  setServo2(1, 90 + h);  //5B Arriba
  delay(t);

  setServo2(6, 90 + a);  //1A horario
  setServo1(3, 90 + a);  //4A horario
  setServo2(0, 90 + a);  //5A horario
  delay(t);

  //setServo2(7, 90 - h);   //1B Abajo
  //setServo1(4, 90 - h);   //4B Abajo
  //setServo2(1, 90 - h);   //5B Abajo
  setServo2(7, 90);  //1B Abajo
  setServo1(4, 90);  //4B Abajo
  setServo2(1, 90);  //5B Abajo
  delay(t);


  setServo1(1, 90 + h);  //2B Arriba
  setServo2(4, 90 + h);  //3B Arriba
  setServo1(7, 90 + h);  //6B Arriba
  delay(t);

  setServo1(0, 90 + a);  //2A horario
  setServo2(3, 90 + a);  //3A horario
  setServo1(6, 90 + a);  //6A horario
  delay(t);

  //setServo1(1, 90 - h);   //2B Abajo
  //setServo2(4, 90 - h);   //3B Abajo
  //setServo1(7, 90 - h);   //6B Abajo
  setServo1(1, 90);  //2B Abajo
  setServo2(4, 90);  //3B Abajo
  setServo1(7, 90);  //6B Abajo
  delay(t);

  estatico();
}

void caminar1(int a, int x, int h, int t) {
  //Movimiento 1
  setServo1(0, 90 - x);  //2A  antihorario
  setServo1(1, 90 - h);  //2B  Abajo

  setServo2(3, 90 + x);  //3A  horario
  setServo2(4, 90 - h);  //3B  abajo

  setServo1(6, 90 - x);  //6A  antihorario
  setServo1(7, 90 - h);  //6B  abajo


  setServo2(6, 90 + x);  //1A  horario
  setServo2(7, 90 + h);  //1B  arriba

  setServo1(3, 90 - x);  //4A antihorario
  setServo1(4, 90 + h);  //4B Arriba

  setServo2(0, 90 + x);  //5A  horario
  setServo2(1, 90 + h);  //5B  arriba

  delay(t);

  //Movimiento 2
  setServo2(6, 90 - a);  //1A  antihorario
  setServo1(3, 90 + a);  //4A  horario
  setServo2(0, 90 - a);  //5A  antihorario

  delay(t);

  //Movimiento 3
  setServo2(7, 90 - h);  //1B  abajo
  setServo1(4, 90 - h);  //4B  abajo
  setServo2(1, 90 - h);  //5B  abajo

  delay(t);

  //Movimiento 4
  setServo2(6, 90 + x);  //1A  horario
  setServo1(3, 90 - x);  //4A  antihorario
  setServo2(0, 90 + x);  //5A  horario

  setServo1(1, 90 + h);  //2B  Arriba
  setServo2(4, 90 + h);  //3B  arriba
  setServo1(7, 90 + h);  //6B  arriba

  //delay(t);
}

void caminar2(int a, int x, int h, int t) {
  //Movimiento 1
  setServo2(6, 90 + x);  //1A  horario
  setServo2(7, 90 - h);  //1B  abajo

  setServo1(3, 90 - a);  //4A  antihorario
  setServo1(4, 90 - h);  //4B  abajo

  setServo2(0, 90 + x);  //5A  horario
  setServo2(1, 90 - h);  //5B  abajo


  setServo1(0, 90 - x);  //2A  antihorario
  setServo1(1, 90 + h);  //2B  arriba

  setServo2(3, 90 + x);  //3A  horario
  setServo2(4, 90 + h);  //3B  arriba

  setServo1(6, 90 - x);  //6A  antihorario
  setServo1(7, 90 + h);  //6B  arriba

  delay(t);

  //Movimiento 2
  setServo1(0, 90 + a);  //2A  horario
  setServo2(3, 90 - a);  //3A  antihorario
  setServo1(6, 90 + a);  //6A  horario

  delay(t);

  //Movimiento 3
  setServo1(1, 90 - h);  //2B  abajo
  setServo2(4, 90 - h);  //3B  abajo
  setServo1(7, 90 - h);  //6B  abajo

  delay(t);

  //Movimiento 4
  setServo1(0, 90 - x);  //2A  antihorario
  setServo2(3, 90 + x);  //3A  horario
  setServo1(6, 90 - x);  //6A  antihorario

  setServo2(7, 90 + h);  //1B  arriba
  setServo1(4, 90 + h);  //4B  arriba
  setServo2(1, 90 + h);  //5B  arriba

  //delay(t);
}

void lateral1(int a, int x, int h, int t) {
  //Movimiento1
  setServo1(0, 90);  //2A
  setServo1(1, 90);  //2B
  setServo1(2, 90);  //2C

  setServo2(3, 90);  //3A
  setServo2(4, 90);  //3B
  setServo2(5, 90);  //3C

  setServo1(6, 90);  //6A
  setServo1(7, 90);  //6B
  setServo1(8, 90);  //6C

  setServo2(6, 90 - a);  //1A antihorario
  setServo2(7, 90 + h);  //1B arriba
  setServo2(8, 90);      //1C

  setServo1(3, 90);      //4A
  setServo1(4, 90 + h);  //4B arriba
  setServo1(5, 90);      //4C


  setServo2(0, 90 + a);  //5A horario
  setServo2(1, 90 + h);  //5B arriba
  setServo2(2, 90);      //5C
  delay(t);

  //Movimiento2
  setServo2(6, 90 + a);  //1A horario
  setServo1(5, 90 + x);  //4C adentro
  setServo2(0, 90 - a);  //5A antihorario
  delay(t);

  //Movimiento3
  setServo2(7, 90);  //1B abajo
  setServo1(4, 90);  //4B abajo
  setServo2(1, 90);  //5B abajo
  delay(t);

  //Movimiento4
  setServo1(1, 90 + h);  //2B arriba
  setServo2(4, 90 + h);  //3B arriba
  setServo1(7, 90 + h);  //6B arriba
  delay(t);

  //Movimmiento5
  setServo1(0, 90 + a);  //2A horario
  setServo1(6, 90 - a);  //6A antihorario

  setServo2(6, 90);  //1A antihorario
  setServo1(5, 90);  //4C afuera
  setServo2(0, 90);  //5A antihorario

  //delay(t);
}

void lateral2(int a, int x, int h, int t) {
  //Movimiento1
  setServo2(6, 90);  //1A
  setServo2(7, 90);  //1B
  setServo2(8, 90);  //1C

  setServo1(3, 90);  //4A
  setServo1(4, 90);  //4B
  setServo1(5, 90);  //4C

  setServo2(0, 90);  //5A
  setServo2(1, 90);  //5B
  setServo2(2, 90);  //5C

  setServo1(0, 90 + a);  //2A horario
  setServo1(1, 90 + h);  //2B arriba
  setServo1(2, 90);      //2C

  setServo2(3, 90);      //3A
  setServo2(4, 90 + h);  //3B arriba
  setServo2(5, 90);      //3C

  setServo1(6, 90 - a);  //6A antihorario
  setServo1(7, 90 + h);  //6B arriba
  setServo1(8, 90);      //6C
  delay(t);

  //Movimiento2
  setServo1(0, 90 - a);  //2A antihorario
  setServo2(5, 90 - x);  //3C afuera
  setServo1(6, 90 + a);  //6A horario
  delay(t);

  //Movimiento3
  setServo1(1, 90);  //2B abajo
  setServo2(4, 90);  //3B abajo
  setServo1(7, 90);  //6B abajo
  delay(t);

  //Movimiento4
  setServo2(7, 90 + h);  //1B arriba
  setServo1(4, 90 + h);  //4B arriba
  setServo2(1, 90 + h);  //5B arriba
  delay(t);

  //Movimiento5

  setServo2(6, 90 - a);  //1A antihorario
  setServo2(0, 90 + a);  //5A horario

  setServo1(0, 90);  //2A  horario
  setServo2(5, 90);  //3C  adentro
  setServo1(6, 90);  //6A  horario
  //delay(t);
}

void avanzar(int x, int d, int h, int t) {
  caminar1(x, d, h, t);
  caminar2(x, d, h, t);
}


//---------- Funcion de Estabilidad ----------

void balance() {
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
  valxizq = map(valxizq, -45, 45, 10, 170);  ///Las patas iaquierdas el mapeo tiene que ir de 0/180

  //patas del lado derecho
  float valxder = ang_x;
  valxder = map(valxder, -45, 45, 170, 10);  ///las patas derechas el mapeo tiene que ir de 180/0
                                             /////////////////////////////////////////////////////////////////////////////////////////////////////
  //patas delanteras
  float valydel = ang_y;
  valydel = map(valydel, -40, 20, 30, 70);  ///las patas delanteras el mapeo tiene que ir de 180/0

  //patas traseras
  float valytras = ang_y;
  valytras = map(valytras, -40, 20, 150, 110);  ///las patas traseras el mapeo tiene que ir de 0/180
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
  setServo2(6, 30);
  setServo2(7, valxder);
  setServo2(8, valxder);

  //pata 2 izq delantero
  setServo1(0, 150);
  setServo1(1, valxizq);
  setServo1(2, valxizq);

  //pata 3 der medio
  setServo2(3, 90);
  setServo2(4, valxder);
  setServo2(5, valxder);

  //pata 4 izq medio
  setServo1(3, 90);
  setServo1(4, valxizq);
  setServo1(5, valxizq);
  //pata 5 der trasero
  setServo2(0, valytras);
  setServo2(1, valxder);
  setServo2(2, valxder);

  //pata 6 izq trasero
  setServo1(6, valydel);
  setServo1(7, valxizq);
  setServo1(8, valxizq);
  delay(100);
}

//---------- Funcion de Comunicación Lidar ----------
void lidar() {
  String arr[2] = { String(distance), String(Angle) };
  /*int r = 1;
  LIDAR.println(r);
  delay(50);*/
  if (LIDAR.available() > 0) {
    String receivedData = LIDAR.readStringUntil('\n');
    receiveArray(receivedData, arr, 2);
    LIDAR.flush();
  }
  distance = arr[0].toInt();
  Angle = arr[1].toInt();
  //Serial.print("D: ");
  //Serial.println(distance);
  //Serial.print("A: ");
  //Serial.println(Angle);

  /*for(int i =0; i<2; i++) {
  }*/
  //Serial.println(r);
}

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

// FUNCIONES PARA DIRECCIONES
bool arrJoy(int val) {
  if (val >= 800) {
    return true;
  } else {
    return false;
  }
}

bool abaJoy(int val) {
  if (val <= 30) {
    return true;
  } else {
    return false;
  }
}

bool izqJoy(int val) {
  if (val >= 700) return true;
  else return false;
}

bool derJoy(int val) {
  if (val >= 800) return true;
  else return false;
}
//-------------------------------------
//
//
//
//
//
//
//---------------- INICIO: SETUP --------------------

void setup() {

  //INICIALIZACIÓN DE COMUNICACIONES
  Wire.begin();
  Serial.begin(9600);
  LIDAR.begin(9600);
  BT.begin(9600);

  //CONFIG SERVOS
  servos1.begin();
  servos1.setPWMFreq(60);  // Frecuencia de PWM en Hz
  servos2.begin();
  servos2.setPWMFreq(60);  // Frecuencia de PWM en Hz
  despliegue(1000);
  mode = 2;
  delay(7000);
}
//---------------- FIN: SETUP --------------------
//
//
//
//
void loop() {
  //estatico();
  lidar();
  coreBlueRX();
  //delay(200);
  coreBlueTX();
  coreModos();
}
//
//
//
//
//
//---------------- INICIO: CONTROLADOR DE MODOS --------------------
void mode2(int X1, int Y1 /*, bool arr2, bool aba2, bool izq2, bool der2*/) {
  if (Y1 >= 800 && Y1 <= 1023) {
    avanzar(20, 10, 20, 250);
  } else if (Y1 <= 200 && Y1>=55) {
    avanzar(-20, -10, 20, 250);
  }
  else {
    estatico();
  }
  //y1 ? avanzar(35, 35, 500) : avanzar(35, 35, 500);
}
void coreModos() {

  switch (mode) {
    case 2:
      mode2(X1, Y1 /*, arrJoy(Y2), abaJoy(Y2), izqJoy(X2), derJoy(X2)*/);
      break;

    case 3:
    
    break;

    default:
      break;
  }
}
//---------------- FIN: CONTROLADOR DE MODOS --------------------
/*
*
*
*
*
*/
//---------------- INICIO: CONTROLADOR DE COMUNICACIONES --------------------
void coreBlueRX() {

  //ENTRADA SERIAL: LEYENDO DATOS DEL MANDO...
  if (BT.available() > 0) {
    String receivedData = BT.readStringUntil('\n');
    receiveArray(receivedData, arrayReceived, sizeReceive);
    BT.flush();

    //changeMode = stringToBoolean(arrayReceived[0]);
    changeMode = false;
    mode = (arrayReceived[1]).toInt();
    X1 = (arrayReceived[2]).toInt();
    Y1 = (arrayReceived[3]).toInt();
    X2 = (arrayReceived[4]).toInt();
    Y2 = (arrayReceived[5]).toInt();
    Serial.println(Y1);
    // Imprimir el array recibido para verificar
    /*for (int i = 0; i < 6; i++) {
      Serial.println(arrayReceived[i]);
    }*/
  }
}

void coreBlueTX() {
  //SALIDA SERIAL: ENVIADO DE VARIABLES AL CONTROL...
  arrayToSend[0] = String(bateria);
  arrayToSend[1] = String(steps);
  arrayToSend[2] = String(distance);
  arrayToSend[3] = String(Angle);
  arrayToSend[4] = String(tiempoActivo);

  //delay(1000);
  sendArray(arrayToSend, sizeSend);
  BT.flush();  // Asegurarse de que el mensaje se ha enviado completamente
}

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
//---------------- FIN: CONTROLADOR DE COMUNICACIONES --------------------
