// 0E/GND -> GND alimentacion
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver servos1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver servos2 = Adafruit_PWMServoDriver(0x41);

unsigned int pos0 = 172;
unsigned int pos180 = 565;
int tiempo = 2000;

//----------Funcion----------
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
  while (true) { // Asume un bucle infinito para avanzar constantemente
    adelante1(x, d, t);
    adelante2(x, d, t);
  }
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

//-------------------------------------

void setup() 
{
  Serial.begin(9600);
  servos1.begin();
  servos2.begin();

  servos1.setPWMFreq(60);
  servos2.setPWMFreq(60);
  
  arranque(60, 60, 500);
}

void loop() 
{
  //avanzar(35,35,500);
  arranque(30, 30, 500);
  //giro(30, 30, 100);
  //delay(500);
  //adelante1(45, 45, 500);
  //delay(500);
  //adelante2(45, 45, 500);  
  //delay(500);
}