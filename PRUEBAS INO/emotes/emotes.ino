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

void despliegue(int t)
{
  //------------C------------
  setServo2(8, 90);//1C
  setServo1(2, 90);//2C
  setServo2(5, 90);//3C
  setServo1(5, 90);//4C
  setServo2(2, 90);//5C
  setServo1(8, 90);//6C
  delay(t);

  //------------A------------
  setServo2(6, 90);//1A
  setServo1(0, 90);//2A
  setServo2(3, 90);//3A
  setServo1(3, 90);//4A
  setServo2(0, 90);//5A
  setServo1(6, 90);//6A
  delay(t);

  //------------B------------
  setServo2(7, 90);//1B
  setServo1(1, 90);//2B
  setServo2(4, 90);//3B
  setServo1(4, 90);//4B  
  setServo2(1, 90);//5B
  setServo1(7, 90);//6B
  //delay(t);
}

void arranque() // cuando el hexapodo se queda se pone de pie al prender
{
  //------------Pata 1------------
  setServo2(6, 90);//A
  setServo2(7, 90);//B
  setServo2(8, 90);//C

  //------------Pata 2------------
  setServo1(0, 90);//A
  setServo1(1, 90);//B
  setServo1(2, 90);//C

  //------------Pata 3------------
  setServo2(3, 90);//A
  setServo2(4, 90);//B
  setServo2(5, 90);//C

  //------------Pata 4------------
  setServo1(3, 90);//A
  setServo1(4, 90);//B
  setServo1(5, 90);//C

  //------------Pata 5------------
  setServo2(0, 90);//A
  setServo2(1, 90);//B
  setServo2(2, 90);//C

  //------------Pata 6------------
  setServo1(6, 90);//A
  setServo1(7, 90);//B
  setServo1(8, 90);//C
}

void recogida()
{
  //------------B------------
  

  setServo2(7, 95);//1B
  setServo1(1, 95);//2B
  setServo2(4, 95);//3B
  setServo1(4, 95);//4B  
  setServo2(1, 95);//5B
  setServo1(7, 95);//6B
  delay(200);

  setServo2(7, 105);//1B
  setServo1(1, 105);//2B
  setServo2(4, 105);//3B
  setServo1(4, 105);//4B  
  setServo2(1, 105);//5B
  setServo1(7, 105);//6B
  delay(200);

  setServo2(7, 115);//1B
  setServo1(1, 115);//2B
  setServo2(4, 115);//3B
  setServo1(4, 115);//4B  
  setServo2(1, 115);//5B
  setServo1(7, 115);//6B
  delay(200);

  setServo2(7, 120);//1B
  setServo1(1, 120);//2B
  setServo2(4, 120);//3B
  setServo1(4, 120);//4B  
  setServo2(1, 120);//5B
  setServo1(7, 120);//6B
  delay(200);
  
  setServo2(7, 160);//1B
  setServo1(1, 160);//2B
  setServo2(4, 160);//3B
  setServo1(4, 160);//4B  
  setServo2(1, 160);//5B
  setServo1(7, 160);//6B
  delay(200);

  //------------A------------
  setServo2(6, 120);//1A
  setServo1(0, 50);//2A
  setServo2(3, 90);//3A
  setServo1(3, 90);//4A
  setServo2(0, 70);//5A
  setServo1(6, 110);//6A
  delay(500);

  //------------C------------
  setServo2(8, 160);//1C
  setServo1(2, 160);//2C
  setServo2(5, 160);//3C
  setServo1(5, 160);//4C
  setServo2(2, 160);//5C
  setServo1(8, 160);//6C
  delay(500);
}

void bordeInterno()
{
  //Posicion
  //------------Pata 1------------
  //A
  setServo2(6, 40);   //antihorario
  //B
  setServo2(7, 20);   //Abajo
  //C
  setServo2(8, 40);   //Afuera

  //------------Pata 2------------
  //A
  setServo1(0, 140);   //horario
  //B
  setServo1(1, 20);   //Abajo
  //C
  setServo1(2, 40);   //Afuera

  //------------Pata 3------------
  //A
  setServo2(3, 60);   //antihorario
  //B
  setServo2(4, 50);   //Abajo

  //------------Pata 4------------
  //A
  setServo1(3, 120);   //horario
  //B
  setServo1(4, 50);   //Abajo

  //------------Pata 5------------
  //A
  setServo2(0, 120);   //horario
  //B
  setServo2(1, 110);   //Arriba
  //C
  setServo2(2, 60);   //Afuera

  //------------Pata 6------------
  //A
  setServo1(6, 30);   //antihorario
  //B
  setServo1(7, 110);   //Arriba
  //C
  setServo1(8, 120);   //Afuera

  delay(200);




  //Movimiento
  //------------Pata 5------------
  //C
  setServo2(2, 90);   //Adentro
  delay(200);

  //------------Pata 4------------
  //B
  setServo1(4, 10);   //Abajo
  
  //------------Pata 5------------
  //A
  setServo2(0, 90);   //antihorario
  //C
  setServo2(2, 120);   //Adentro
  
  //------------Pata 6------------
  setServo1(7, 140);   //Arriba
  delay(200);

  //------------Pata 5------------
  //A
  setServo2(0, 60);   //antihorario
  delay(200);

  //------------Pata 5------------
  //A
  setServo2(0, 90);   //antihorario
  //C
  setServo2(2, 90);   //Adentro
  delay(200);

  //------------Pata 4------------
  //B
  setServo1(4, 40);   //Arriba
  
  //------------Pata 5------------
  //A
  setServo2(0, 120);   //horario
  //C
  setServo2(2, 60);   //Afuera

  //------------Pata 6------------
  setServo1(7, 50);   //Abajo
  delay(200);
}

void saludar()
{
  int t = 500;
  //Posicion
  arranque();
  delay(t);

  //Movimiento
  //1B
  setServo2(7, 160);   //Abajo
  delay(t);
  
  //1C
  setServo2(8, 40);   //Afuera
  delay(t);

  //1A
  setServo2(6, 40);   //antihorario
  delay(t);

  //1A
  setServo2(6, 140);   //Horario
  delay(t);

  //1A
  setServo2(6, 40);   //antihorario
  delay(t);

  //1A
  setServo2(6, 140);   //Horario
  delay(t);
  //1A
  setServo2(6, 40);   //antihorario
  delay(500);

  //1A
  setServo2(6, 140);   //Horario
  delay(t);

  //1A
  setServo2(6, 90);   //Antihorario
  delay(t);

  arranque();
  delay(t);
}

void baile()
{
  int t = 500;
  int a = 30;
  int h = 30;

  // Levantar y adelantar patas del grupo 1, 3, 5 (derecho)
  // Levantar patas
  setServo2(7, 90 + h); // Pata 1 arriba
  setServo2(4, 90 + h); // Pata 3 arriba
  setServo2(1, 90 + h); // Pata 5 arriba
  delay(t);

  // Adelantar patas
  setServo2(6, 90 + a); // Pata 1 adelante
  setServo2(3, 90 + a); // Pata 3 adelante
  setServo2(0, 90 + a); // Pata 5 adelante
  delay(t);

  // Bajar patas
  setServo2(7, 90 - h); // Pata 1 abajo
  setServo2(4, 90 - h); // Pata 3 abajo
  setServo2(1, 90 - h); // Pata 5 abajo
  delay(t);

  // Retrasar patas
  setServo2(6, 90); // Pata 1 atrás
  setServo2(3, 90); // Pata 3 atrás
  setServo2(0, 90); // Pata 5 atrás
  delay(t);

  // Levantar y adelantar patas del grupo 2, 4, 6 (izquierdo)
  // Levantar patas
  setServo1(1, 90 + h); // Pata 2 arriba
  setServo1(4, 90 + h); // Pata 4 arriba
  setServo1(7, 90 + h); // Pata 6 arriba
  delay(t);

  // Adelantar patas
  setServo1(0, 90 + a); // Pata 2 adelante
  setServo1(3, 90 + a); // Pata 4 adelante
  setServo1(6, 90 + a); // Pata 6 adelante
  delay(t);

  // Bajar patas
  setServo1(1, 90 - h); // Pata 2 abajo
  setServo1(4, 90 - h); // Pata 4 abajo
  setServo1(7, 90 - h); // Pata 6 abajo
  delay(t);

  // Retrasar patas
  setServo1(0, 90); // Pata 2 atrás
  setServo1(3, 90); // Pata 4 atrás
  setServo1(6, 90); // Pata 6 atrás
  delay(t);
}


//-------------------------------------

void setup() 
{
  Serial.begin(9600);
  servos1.begin();
  servos2.begin();

  servos1.setPWMFreq(60);
  servos2.setPWMFreq(60);
  
  //despliegue(1000);
  //delay(100);
  //arranque();
  //delay(2000);
  //recogida(); 
}

void loop() 
{
  //bordeInterno();
  //saludar();
  //baile();
}