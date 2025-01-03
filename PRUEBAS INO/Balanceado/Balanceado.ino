#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <MPU6050.h>
#include <I2Cdev.h>

Adafruit_PWMServoDriver servos1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver servos2 = Adafruit_PWMServoDriver(0x41);
MPU6050 sensor;

unsigned int pos0 = 172;
unsigned int pos180 = 565;
int tiempo = 2000;


// Variables para el filtro complementario
float ang_x = 0, ang_y = 0;
float ang_x_prev = 0, ang_y_prev = 0;
float dt = 0;
unsigned long tiempo_prev = 0;
// Función para ajustar los servos con PWM
 void setServo1(uint8_t n_servo, int angulo) {
  int duty = map(angulo, 0, 180, pos0, pos180);
  servos1.setPWM(n_servo, 0, duty);
}
void setServo2(uint8_t n_servo, int angulo) {
  int duty = map(angulo, 0, 180, pos0, pos180);
  servos2.setPWM(n_servo, 0, duty);
}

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

void setup() {
  Wire.begin();
  Serial.begin(9600);
  sensor.initialize();
  //sensor.setFullScaleGyroRange(2);



  servos1.begin();
  servos1.setPWMFreq(60);  // Frecuencia de PWM en Hz
  servos2.begin();
  servos2.setPWMFreq(60);  // Frecuencia de PWM en Hz

  //función para balanceo y estabilizador de las patas.
}

void loop() {
  balance();
  
}
