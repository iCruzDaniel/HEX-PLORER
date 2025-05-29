#include "hexplorer.h"  // La librería Hexplorer ya posee la librería Wire.h y Adafruit_PWMServoDriver
#include <SoftwareSerial.h>

SoftwareSerial WMBT(5, 6);  // RX, TX recorder que se cruzan (Para Arduino)

//--- CONFIG PIN EYES ---
int ledF = 7;
int ledFg = 8;
int ledR = 9;
int ledRg = 10;

//--- VARIABLES GLOBALES ---
int velocidad = 50;
String tramaRecibida;
char movimiento = 'N';
char movimientoAnte = 'K';
bool continuidad = false;


//--- FUNCIONES BÁSICAS ---
void adelante(bool band) {
  if (band) {
    hexplorer.desplazamiento(35, 35, 5, velocidad, 200, false, true);  //Continuidad Verdadera
  } else {
    hexplorer.desplazamiento(35, 35, 5, velocidad, 200, false, false);  //Continuidad Falsa
  }
}

void atras(bool band) {
  if (band) {
    hexplorer.desplazamiento(35, 35, 5, velocidad, 200, true, true);
  } else {
    hexplorer.desplazamiento(35, 35, 5, velocidad, 200, true, false);
  }
}

void girohorario(bool band) {
  if (band) {
    hexplorer.giro(35, 35, 5, velocidad, 200, false, true);
  } else {
    hexplorer.giro(35, 35, 5, velocidad, 200, false, false);
  }
}

void giroantihorario(bool band) {
  if (band) {
    hexplorer.giro(35, 35, 5, velocidad, 200, true, true);
  } else {
    hexplorer.giro(35, 35, 5, velocidad, 200, true, false);
  }
}

void detener(bool band) {
  if (!band) {
    hexplorer.detener();
  } else {
    hexplorer.detener(0);
  }
}

void ledAction() {
}


//--- SETUP ---
void setup() {

  hexplorer.init();      //Iniciamos modulo hexapodo
  hexplorer.arranque();  //posicion inicial del hexapodo
  delay(2000);           //delay de seguridad

  Serial.begin(9600);  // Comunicación con el monitor serial de la PC
  WMBT.begin(9600);    // Comunicación con modulo Bluetooth
  F("serial iniciado");

  pinMode(ledF, OUTPUT);
  pinMode(ledFg, OUTPUT);
  pinMode(ledR, OUTPUT);
  pinMode(ledRg, OUTPUT);

  digitalWrite(ledFg, 0);
  digitalWrite(ledRg, 0);


  for (int i = 0; i < 15; i++) {
    digitalWrite(ledF, 1);
    digitalWrite(ledR, 0);
    delay(80);

    digitalWrite(ledF, 0);
    digitalWrite(ledR, 1);
    delay(150);
  }
  digitalWrite(ledF, 1);
  digitalWrite(ledR, 1);
}

//--- MAIN LOOP ---
void loop() {

  // Leer un carácter desde el módulo Bluetooth
  if (WMBT.available() > 0) {

    //recepción de trama
    delay(10);
    tramaRecibida = WMBT.readStringUntil('|');

    if(tramaRecibida[0] == 'V') velocidad = 50 - tramaRecibida.substring(1).toInt();
    else {
    movimiento = tramaRecibida[0];
    continuidad = (movimiento == movimientoAnte) ? true : false;  //varificamos la continuidad del movimiento anterior
    }

    //Imprimir valor reciñbido
    F("Carácter recibido: ");
    Serial.println(tramaRecibida);

    WMBT.flush();
  } else {
    WMBT.flush();
    movimiento = 'p';
    continuidad = (movimiento == movimientoAnte) ? true : false;  //varificamos la continuidad del movimiento anterior
  }


  // Selección de movimientos
  switch (movimiento) { 
    case 'W':
      //ADELANTE
      adelante(continuidad);
      // WMBT.println("Adelante");
      F("Adelante");
      delay(200);
      break;

    case 'S':
      //ATRAS
      atras(continuidad);
      // WMBT.println("Atrás");
      F("Atrás");
      delay(200);
      break;

    case 'D':
      //GIRO HORARIO
      girohorario(continuidad);
      // WMBT.println("Giro Horario");
      F("Giro Horario");
      delay(200);
      break;

    case 'A':
      //GIRO ANTIHORARIO
      giroantihorario(continuidad);
      // WMBT.println("Giro AntiHorario");
      F("Giro AntiHorario");
      delay(200);
      break;

    case 'p':
      //DETENER
      detener(continuidad);
      // WMBT.println("Detener");
      F("Detener");
      delay(200);
      break;
  }
  movimientoAnte = movimiento;
}