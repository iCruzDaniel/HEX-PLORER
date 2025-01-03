//------------------- LIBRERIAS -------------------
//------------------- Libs Pantalla TFT -------------------
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_ST7735.h>  // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h>  // Hardware-specific library for ST7789
#include <SPI.h>
#include "Imagenes.h"  //Direccion de las Imagenes y colores

//------------------- Libs RF -------------------
/*
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
*/
//------------------- Lib Para el Bluetooth -------------------
#include <SoftwareSerial.h>
//---------------- DEFINICIONES -------------------
//---------------- Asignación de Pines -------------------
#define TFT_CS 10  //
#define TFT_RST 9  // Or set to -1 and connect to Arduino RESET pin //verde
#define TFT_DC 8   //

#define TFT_MOSI 11  // Data out //azul (SDA)
#define TFT_SCLK 13  // Clock out //morado (SCL)

//#define CE_PIN 9
//#define CSN_PIN 10

//PINES PARA EL BLUETOOTH
#define BTTx 5
#define BTRx 4

//Controll de Brillo
#define BRILLO 6    //PWM Brillo
#define Battery A0  //Lectura de Batería

//ASIGNACIÓN DE PULSADORES
#define menu 3
#define ok 2

//PINES JOYSTICKES
#define joyX1 A2  //Joystick X1
#define joyY1 A1  //Joystick Y1

#define joyX2 A5  //Joystick X2
#define joyY2 A4  //Joystick Y2


//---------------- OBJETOS DE MANIPULACION -------------------
//------------------- Obj Pantalla TFT -------------------
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);  //INTANCIAMOS OBJETO DE LAPANTALLA TFT
//------------------- Obj RF -------------------
//RF24 radio(CE_PIN, CSN_PIN); //INTANCIAMOS OBJETO DE MODULO RF
//------------------- Obj Bluetooth -------------------
SoftwareSerial BT(BTRx, BTTx);  // RX, TX

//----------------INICIO: VARIABLES-------------------
//---------------- Variables de Control General --------------------
int refX = 0;  // Variables Iniciales del Modo Ingreso de coordenadas
int refY = 0;  // Variables Iniciales del Modo Ingreso de coordenadas

int angle = 0;
int distance = 0;

bool changeMode=false;

//---------------- Control TFT --------------------
int modo, actualVista = 2;
//int cambi = 2;
int batMando, actualBatMando = 0;
int batHex = 0;
long int lectura;
bool conectHex = false;
bool conectHMI = false;
bool states[] = { false, true, false, true, true, false };  //Estados de la patas
//bool conectionStateHmi = false;
//---------------- Control RF --------------------
/*
const byte address[][6] = { "00001", "00002" };  // Direcciones para comunicar ambos Arduinos
const int vectorSizeSend = 6;                    // Tamaño del vector de enviado
const int vectorSizeRecieve = 4;                 // Tamaño del vector de recibido
int sendVector[vectorSizeSend];                  // Vector para enviar
int receivedVector[vectorSizeRecieve];           // Vector para recibir
*/
//---------------- Comunicación Bluetooth -------------------
const int sizeSend = 6;
const int sizeReceive = 4;
String arrayToSend[sizeSend] = { "String1", "String2", "String3", "String4", "String5", "String6" };
String arrayReceived[sizeReceive];
char delimiter = '|';  // Delimitador para separar los Strings en la trama
//---------------- FIN: VARIABLES --------------------
//
//
//
//
//
//----------- INICIO: FUNCIONES PARA EL JOYSTICK ---------
bool arrJoy(int val) {
  if (val <= 200) return true;
  else return false;
}

bool abaJoy(int val) {
  if (val >= 800) return true;
  else return false;
}

bool izqJoy(int val) {
  if (val >= 700) return true;
  else return false;
}

bool derJoy(int val) {
  if (val <= 100) return true;
  else return false;
}
//----------- FINAL: FUNCIONES PARA EL JOYSTICK ---------
//
//
//
//
//---------------- INICIO: FUNCIONES PANTALLA TFT-------------------
void Fondo() {
  //tft.fillRoundRect(inicio horizontal, Inicio vertical, ancho, alto, curva, color);
  tft.fillRoundRect(0, 0, 240, 40, 0, BLACK);  //Banda 1
  tft.fillRoundRect(0, 40, 240, 10, 0, AQUA);  //Banda 2
  tft.fillRoundRect(0, 50, 240, 40, 0, CYAN);  //Banda 3
  //Texto
  Escribir("Modo: ", 5, 60, 2, BLACK);
  tft.fillRoundRect(0, 90, 165, 160, 0, GRAY);    //Hexapodo
  tft.fillRoundRect(165, 90, 75, 160, 0, BLACK);  //Indicadores
  tft.fillRoundRect(0, 230, 240, 50, 0, BLACK);   //Banda inferior
}

void Escribir(char *text, int x, int y, int size, uint16_t color) {  //(Texto, posicion en X, posicion en Y, tamaño de letra(1,2,3), color de fuente)
  tft.setCursor(x, y);                                               //Posicion
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.setTextWrap(true);
  tft.print(text);
}

void EscribirVar(int variable, int x, int y, int size, uint16_t color) {
  tft.setCursor(x, y);  //Posicion
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.setTextWrap(true);
  tft.print(variable);
}

void bateriaView(float x, float y, float size, float porcentaje, uint16_t color1, uint16_t color2, uint16_t color3) {  //(posicion en X, posicion en Y, tamaño de figura, porcentaje de carga, color de contorno y carga, color de fondo, color contraste carga)
  //Primer rectangulo de contorno
  tft.fillRoundRect(x, y, size * 10, size * 6, size * 0.8, BLACK);
  //Rectangulo de la punta de la bateria
  tft.fillRoundRect(x - 1 + size * 10, y + size * 1.5, size * 0.8, size * 3, size * 0.2, BLACK);

  //Rectangulo para simular un fondo transparente
  tft.fillRoundRect(x + size * 0.5, y + size * 0.5, size * 9, size * 5, size * 0.8, color2);

  //Rectangulo de contraste para barra de carga
  tft.fillRoundRect(x + size, y + size, size * 8, size * 4, size * 0.4, color3);

  //se convierte el porcentaje
  porcentaje = porcentaje / 100;
  //Rectangulo para barra de carga
  tft.fillRoundRect(x + size, y + size, size * 8 * porcentaje, size * 4, size * 0.4, color1);
}

void stateBar(int porcentBat, bool conexionHex, bool conexionHMI)  //Barra de esto del mando (Conexión con hexapod, Conexiuón con HMI, Batería mando)
{
  //tft.drawBitmap(inicio horizontal, Inicio vertical, Nombre de la imagen, ancho, alto, color);

  tft.fillRoundRect(0, 0, 240, 40, 0, WHITE);

  if (conexionHex) {
    tft.drawBitmap(15, 1, Senal, 40, 40, GREEN);
  } else {
    tft.drawBitmap(15, 1, Senal, 40, 40, BLACK);
  }

  if (conexionHMI) {
    tft.drawBitmap(70, 2, TranDatos, 35, 35, GREEN);
  } else {
    tft.drawBitmap(70, 2, TranDatos, 35, 35, BLACK);
  }
  switch (porcentBat) {  //Bateria Control
    case 10:
      bateriaView(165, 2, 5, porcentBat, RED, WHITE, WHITE);  //Porcentaje de Bateria en 10% o menos
      break;
    case 25:
      bateriaView(165, 2, 5, porcentBat, ORANGE, WHITE, WHITE);  //Porcentaje de Bateria entre 10% y 25%
      break;
    case 50:
      bateriaView(165, 2, 5, porcentBat, YELLOW, WHITE, WHITE);  //Porcentaje de Bateria entre 25% y 50%
      break;
    case 75:
      bateriaView(165, 2, 5, porcentBat, GREEN, WHITE, WHITE);  //Porcentaje de Bateria entre 50% y 75%
      break;
    case 100:
      bateriaView(165, 2, 5, porcentBat, GREEN, WHITE, WHITE);  //Porcentaje de Bateria entre 75% y 100%
      break;
    default:
      break;
  }
}

void bandaTitulo(int modo) {
  double color1 = WHITE;
  double color2 = WHITE;
  double color3 = WHITE;

  switch (modo) {
    case 1:
      color1 = AQUA;
      color2 = CYAN;
      color3 = BLACK;

      break;
    case 2:
      color1 = PURPLE;
      color2 = MAGENTA;
      color3 = WHITE;

      break;
    case 3:
      color1 = RED;
      color2 = ORANGE;
      color3 = WHITE;

      break;
    case 4:
      color1 = ORANGE;
      color2 = YELLOW;
      color3 = BLACK;

      break;
    default:
      break;
  }
  tft.fillRoundRect(0, 40, 240, 10, 0, color1);  //Banda delgada
  tft.fillRoundRect(0, 50, 240, 40, 0, color2);  //Banda gruesa
  //Texto
  Escribir("Modo: ", 5, 60, 2, BLACK);

  switch (modo) {
    case 1:
      Escribir(" AutoMap", 62, 60, 2, color3);
      break;
    case 2:
      Escribir(" Manual", 62, 60, 2, color3);
      break;
    case 3:
      Escribir(" Coordenadas", 62, 60, 2, color3);
      break;
    case 4:
      Escribir(" Baile", 62, 60, 2, color3);
      break;
    default:
      break;
  }
}

double colorPata(bool state) {
  if (state) {
    return GREEN;
  } else {
    return RED;
  }
}

void pisadasView(bool pata[])  //Sirve para modo 1 y 2 (manual y automap)
{
  tft.fillRoundRect(0, 90, 165, 160, 0, AQUA);        //Hexapodo fondo
  tft.drawBitmap(18, 110, Hexapodo, 128, 100, GRAY);  //Hexapodo frente

  /*tft.fillCircle(25, 113, 6, colorPata(pata[0]));        //PATA 0
  tft.fillCircle(142, 113, 6, colorPata(pata[1]));      //PATA 1
  tft.fillCircle(20, 175, 6, colorPata(pata[2]));       //PATA 2 
  tft.fillCircle(147, 175, 6, colorPata(pata[3]));       //PATA 3   
  tft.fillCircle(20, 205, 6, colorPata(pata[4]));        //PATA 4      
  tft.fillCircle(147, 205, 6, colorPata(pata[5]));      //PATA 5  */
}

void balanceFondo() {

  tft.fillRoundRect(0, 90, 165, 160, 0, AQUA);           //Hexapodo fondo
  tft.drawBitmap(18, 93, BalanceFondo, 115, 115, GRAY);  //Hexapodo frente

  Escribir("Balanceandose", 5, 212, 2, GRAY);
}

void baile() {

  tft.fillRoundRect(0, 90, 165, 160, 0, AQUA);    //Hexapodo fondo
  tft.drawBitmap(18, 93, Baile, 115, 115, GRAY);  //Hexapodo frente

  Escribir("Baile...", 40, 212, 2, GRAY);
}

void Coordenadas1() {
  //Posicion
  tft.fillRoundRect(0, 85, 165, 160, 0, AQUA);  //fondo
  //Ejes
  Escribir("X", 38, 120, 3, BLACK);
  Escribir("Y", 113, 120, 3, BLACK);

  //Cuadros
  tft.fillRoundRect(15, 160, 60, 35, 3, WHITE);        //Coordenada X
  tft.fillTriangle(35, 155, 45, 145, 55, 155, BLACK);  //Flecha arriba X
  tft.fillTriangle(35, 200, 45, 210, 55, 200, BLACK);  //Flecha abajo X

  tft.fillRoundRect(90, 160, 60, 35, 3, WHITE);           //Coordenada Y
  tft.fillTriangle(110, 155, 120, 145, 130, 155, BLACK);  //Flecha arriba Y
  tft.fillTriangle(110, 200, 120, 210, 130, 200, BLACK);  //Flecha abajo Y

  //Valores de la coordenadas
  EscribirVar(refX, 40, 172, 2, BLACK);
  EscribirVar(refY, 115, 172, 2, BLACK);
}
void Coordenadas2() {

  tft.fillRoundRect(0, 90, 165, 160, 0, AQUA);        //Hexapodo fondo
  tft.drawBitmap(18, 100, Hexapodo, 128, 100, GRAY);  //Hexapodo frente
  //LeerPatas2();                                        //Contacto de las patas
  //Estado
  //Escribir("Posicionado", 52, 215, 1, GRAY);
  Escribir("Trasladandose", 5, 205, 2, GRAY);
}

void Emotes(int opcion) {
  int x = 0;
  //Opciones fondo
  tft.fillRoundRect(0, 85, 165, 29, 0, GRAY);     //Opcion 1 fondo
  tft.fillRoundRect(0, 114, 165, 29, 0, YELLOW);  //Opcion 2 fondo
  tft.fillRoundRect(0, 143, 165, 29, 0, PINK);    //Opcion 3 fondo
  tft.fillRoundRect(0, 172, 165, 29, 0, BLUE);    //Opcion 4 fondo
  tft.fillRoundRect(0, 201, 165, 29, 0, RED);     //Opcion 5 fondo

  //Opciones Texto
  Escribir("Opcion 1", 10, 95, 2, BLACK);
  Escribir("Opcion 2", 10, 124, 2, BLACK);
  Escribir("Opcion 3", 10, 153, 2, BLACK);
  Escribir("Opcion 4", 10, 182, 2, BLACK);
  Escribir("Opcion 5", 10, 211, 2, BLACK);

  if (opcion == 1) {
    x = 0;
  } else if (opcion == 2) {
    x = x + 29;
  }

  else if (opcion == 3) {
    x = x + 58;
  }

  else if (opcion == 4) {
    x = x + 87;
  } else if (opcion == 5) {
    x = x + 116;
  }

  tft.fillTriangle(150, 90 + x, 130, 100 + x, 150, 110 + x, BLACK);
  tft.fillTriangle(145, 95 + x, 135, 100 + x, 145, 105 + x, WHITE);
}

void Lateral(float porcentaje) {
  tft.fillRoundRect(165, 90, 75, 160, 0, WHITE);  //Indicadores fondo

  bateriaView(180, 180, 5, porcentaje, BLACK, WHITE, GRAY);  //Bateria Hexapodo

  EscribirVar(porcentaje, 188, 187, 2, WHITE);
}

void selecModo(int select, int ubic) {
  tft.fillRoundRect(0, 0, 240, 230, 0, GRAY);  //
  tft.drawBitmap(75, 70, Pausa, 90, 90, BLACK);
  navBar(select, ubic);
}

void navBar(int modo) {
  double color1 = WHITE;
  double color2 = WHITE;
  double color3 = WHITE;
  double color4 = WHITE;
  double color5 = WHITE;
  double color6 = WHITE;
  int posi = 0;

  switch (modo) {
    case 1:
      color1 = CYAN;
      color2 = BLUE;
      //color modos
      color3 = WHITE;  //modo 1
      color4 = BLACK;  //modo 2
      color5 = BLACK;  //modo 3
      color6 = BLACK;  //modo 4
      //posicióncuadro selector
      posi = 10;
      break;
    case 2:
      color1 = PINK;
      color2 = PURPLE;
      //color modos
      color3 = BLACK;  //modo 1
      color4 = WHITE;  //modo 2
      color5 = BLACK;  //modo 3
      color6 = BLACK;  //modo 4
      //posicióncuadro selector
      posi = 65;
      break;
    case 3:
      color1 = ORANGE;
      color2 = COFFEE;
      //color modos
      color3 = BLACK;  //modo 1
      color4 = BLACK;  //modo 2
      color5 = WHITE;  //modo 3
      color6 = BLACK;  //modo 4
      //posicióncuadro selector
      posi = 125;
      break;
    case 4:
      color1 = YELLOW;
      color2 = ORANGE;
      //color modos
      color3 = BLACK;  //modo 1
      color4 = BLACK;  //modo 2
      color5 = BLACK;  //modo 3
      color6 = WHITE;  //modo 4
      //posicióncuadro selector
      posi = 180;
      break;
    default:
      break;
  }

  tft.fillRoundRect(0, 230, 240, 50, 0, color1);    //Banda inferior fondo
  tft.fillRoundRect(posi, 230, 50, 50, 5, color2);  //Cuadrado para indicar seleccion

  //Simbolos
  tft.drawBitmap(20, 240, Mapa, 34, 31, color3);  //Automap

  tft.drawBitmap(75, 240, Mando, 30, 30, color4);  //Manual

  tft.drawBitmap(135, 240, Balance, 35, 35, color5);  //Cordenadas

  tft.drawBitmap(190, 240, Paz, 27, 30, color6);  //Emotes
}

void navBar(int modo, int ubic) {
  double color1 = WHITE;
  double color2 = WHITE;
  double color3 = WHITE;
  double color4 = WHITE;
  double color5 = WHITE;
  double color6 = WHITE;
  int posi[] = { 10, 65, 125, 180 };

  switch (modo) {
    case 1:
      color1 = CYAN;
      color2 = BLUE;
      break;
    case 2:
      color1 = PINK;
      color2 = PURPLE;
      break;
    case 3:
      color1 = ORANGE;
      color2 = COFFEE;
      break;
    case 4:
      color1 = YELLOW;
      color2 = ORANGE;
      break;
    default:
      break;
  }

  tft.fillRoundRect(0, 230, 240, 50, 0, color1);  //Banda inferior fondo

  switch (ubic) {
    case 1:
      tft.fillRoundRect(posi[0], 230, 50, 50, 5, color2);  //Cuadrado para indicar seleccion
      //color modos
      color3 = WHITE;  //modo 1
      color4 = BLACK;  //modo 2
      color5 = BLACK;  //modo 3
      color6 = BLACK;  //modo 4
      break;
    case 2:
      tft.fillRoundRect(posi[1], 230, 50, 50, 5, color2);  //Cuadrado para indicar seleccion
      //color modos
      color3 = BLACK;  //modo 1
      color4 = WHITE;  //modo 2
      color5 = BLACK;  //modo 3
      color6 = BLACK;  //modo 4
      break;
    case 3:
      tft.fillRoundRect(posi[2], 230, 50, 50, 5, color2);  //Cuadrado para indicar seleccion
      //color modos
      color3 = BLACK;  //modo 1
      color4 = BLACK;  //modo 2
      color5 = WHITE;  //modo 3
      color6 = BLACK;  //modo 4
      break;
    case 4:
      tft.fillRoundRect(posi[3], 230, 50, 50, 5, color2);  //Cuadrado para indicar seleccion
      //color modos
      color3 = BLACK;  //modo 1
      color4 = BLACK;  //modo 2
      color5 = BLACK;  //modo 3
      color6 = WHITE;  //modo 4
      break;
  }


  //Simbolos
  tft.drawBitmap(20, 240, Mapa, 34, 31, color3);  //Automap

  tft.drawBitmap(75, 240, Mando, 30, 30, color4);  //Manual

  tft.drawBitmap(135, 240, Balance, 35, 35, color5);  //Cordenadas

  tft.drawBitmap(190, 240, Paz, 27, 30, color6);  //Emotes
}

// Generación de Vistas...
void vistas(int vista, int porcentBat1, int porcentBat2, bool state1, bool state2, bool pataState[], int num = 1) {
  stateBar(porcentBat1, state1, state2);  //Se le ingresan los valores a la barra de estado
  bandaTitulo(vista);                     //Se determina el título del modo
  //int num = num;
  //bool states[] = {true, true, true, true, true, true};
  if (vista == 1) {
    pisadasView(pataState);
  } else if (vista == 2) {
    pisadasView(pataState);
  } else if (vista == 3) {
    balanceFondo();
    //if (num == 2) Coordenadas2();
  } else if (vista == 4) {
    baile();
    //if (num == 2) Coordenadas2();
  }

  Lateral(porcentBat2);
  navBar(vista);
}

// Control...
void brilloPantalla(int pin, int porcentajeBrillo) {
  // int brillo;
  map(porcentajeBrillo, 0, 100, 0, 255);
  analogWrite(pin, porcentajeBrillo);
}
//---------------- FINAL: FUNCIONES PANTALLA TFT-------------------
//

String boolToString(bool value) {
  if (value) {
    return "true";
  } else {
    return "false";
  }
}
//
//
//
//
//
//---------------- INICIO: SETUP --------------------
void setup() {
  //ASIGNACIÓN DE MODOS DE LOS PINES
  pinMode(menu, INPUT);
  pinMode(ok, INPUT);
  pinMode(BRILLO, OUTPUT);  //Pin pwm del brillo

  //SE INICIALIZA EL MODULO PANTALLA TFT. use this initializer (uncomment) if using a 1.69" 280x240 TFT:
  tft.init(240, 280);  // Init ST7789 280x240


  //Se inicia el control de Vistas en el modo Manual
  lectura = map(analogRead(Battery), 0, 1023, 0, 100);
  if (lectura > 75) batMando = 100;
  if (lectura > 50 && lectura <= 75) batMando = 75;
  if (lectura > 25 && lectura <= 50) batMando = 50;
  if (lectura > 10 && lectura <= 25) batMando = 25;
  if (lectura > 0 && lectura <= 10) batMando = 10;


  vistas(actualVista, batMando, batHex, conectHex, conectHMI, states);
  brilloPantalla(BRILLO, 100);  //Control de brillo

  //INICIALIZACIÓN DE COMUNICACIONES
  Serial.begin(9600);
  BT.begin(9600);
}
//---------------- FIN: SETUP --------------------
//
//
//
//
void loop() {
  core();
  //Serial.println(actualVista);
}


void core() {
  //ENTRADA SERIAL: LEYENDO DATOS DEL HEXAPODO...
  if (BT.available() > 0) {
    String receivedData = BT.readStringUntil('\n');
    receiveArray(receivedData, arrayReceived, sizeReceive);
    /*for (int i = 0; i < 6; i++) {  -->> ACTIVAAAR!!
      Serial.print(arrayReceived[i]);
    }*/
    BT.flush();


    /*int bat2 = (arrayReceived[0]).toInt();
    int distance = (arrayReceived[1]).toInt();
    int angle = (arrayReceived[2]).toInt();
    int tiempoActivo = (arrayReceived[3]).toInt();*/
    //bool states[] = {false, true, false, true, true, false}; //Estados de la patas

    bool conectHex = true;
    bool conectHMI = true;


    // Imprimir el array recibido para verificar  -->>!!!!!!
    /*for (int i = 0; i < sizeReceive; i++) {
      Serial.println(arrayReceived[i]);
    }*/
  }
  //delay(1000);



  //Lectura de Joysticks
  long int xValue1 = analogRead(joyX1);  // Lee el valor analógico del eje X1
  long int yValue1 = analogRead(joyY1);  // Lee el valor analógico del eje Y1
  int xValue2 = analogRead(joyX2);       // Lee el valor analógico del eje X2
  int yValue2 = analogRead(joyY2);       // Lee el valor analógico del eje Y2

  lectura = map(analogRead(Battery), 0, 1023, 0, 100);
  if (lectura > 75) batMando = 100;
  if (lectura > 50 && lectura <= 75) batMando = 75;
  if (lectura > 25 && lectura <= 50) batMando = 50;
  if (lectura > 10 && lectura <= 25) batMando = 25;
  if (lectura > 0 && lectura <= 10) batMando = 10;
  //Serial.print("Bateria: ");
  //Serial.println(batMando);

  Serial.print("x: ");
  Serial.println(xValue2);
  //Serial.print("y: ");
  //Serial.println(yValue2);
  delay(1000);

  //Lectura de Batería
  //bateria = mandoBateria();




  //---------------- INICIO: CONTROLADOR DE VISTA --------------------
  //INICIO CAMBIO DE VISTA --> Se puede incluir en una función que se ejecute en una interupción
  if (digitalRead(menu) == HIGH) {
    delay(50);
    arrayToSend[0] = "true";
    sendArray(arrayToSend, sizeSend);

// Imprimir el array enviado para verificar
for (int i = 0; i < sizeSend; i++) {
  Serial.println(arrayToSend[i]);}


    int cambi, nonCambi = actualVista;
    //int nonCambi = actualVista;

    selecModo(actualVista, actualVista);
    while (digitalRead(menu) == HIGH) {}  // Bucle de anclaje (por si el usuario deja el botón oprimido una vez cambiada la vista)
    delay(50);
    while (digitalRead(ok) == LOW) {
      int xValue = analogRead(joyX1);  //// Lee el valor analógico del eje X1 en seleccion de modos
      if (izqJoy(xValue)) {
        cambi += 1;
        if (cambi >= 5) cambi = 1;
        selecModo(actualVista, cambi);
        delay(50);
      }
      if (derJoy(xValue)) {
        cambi -= 1;
        if (cambi <= 0) cambi = 4;
        selecModo(actualVista, cambi);
        delay(50);
      }
      if (digitalRead(menu) == HIGH) {
        cambi = nonCambi;
        //vistas(actualVista, 25, 70, true, true, states);
        delay(50);
        break;
      }

    }
    actualVista = cambi;
    changeMode = false;
    modo=cambi;
    vistas(actualVista, batMando, batHex, conectHex, conectHMI, states);
  }

  /*if (actualVista == 3) {  // INICIO --> INPUTS VISTA 3

    if (yValue1 <= 200 || yValue1 >= 800) {
      if (abaJoy(yValue1)) refX += 1;
      if (arrJoy(yValue1)) refX -= 1;
      vistas(actualVista, batMando, batHex, conectHex, conectHMI, states);
    }
    /*if (digitalRead(ok) == HIGH) {
      vistas(actualVista, bat1, bat2, conectHex, conectHMI, states, 2);
      refX = 0;
      refY = 0;
      delay(4000);
      vistas(actualVista, bat1, bat2, conectHex, conectHMI, states);
    }
    if (yValue2 <= 200 || yValue2 >= 800) {
      if (abaJoy(yValue2)) refY += 1;
      if (arrJoy(yValue2)) refY -= 1;
      vistas(actualVista, batMando, batHex, conectHex, conectHMI, states);
    }
    if (digitalRead(ok) == HIGH) {
      vistas(actualVista, batMando, batHex, conectHex, conectHMI, states, 2);
      refX = 0;
      refY = 0;
      delay(4000);
      vistas(actualVista, batMando, batHex, conectHex, conectHMI, states);
    }
  }*/
  // FIN --> INPUTS VISTA 3*/
  //FIN CAMBIO DE VISTA
  //---------------- FIN: CONTROLADOR DE VISTA --------------------

  //SALIDA SERIAL: ENVIADO DE VARIABLES AL HEXAPODO...
  arrayToSend[0] = boolToString(changeMode);
  arrayToSend[1] = String(modo);
  arrayToSend[2] = String(xValue1);
  arrayToSend[3] = String(yValue1);
  arrayToSend[4] = String(xValue2);
  arrayToSend[5] = String(yValue2);

  delay(10);
  // Imprimir el array enviado para verificar
  for (int i = 0; i < sizeSend; i++) {
    Serial.println(arrayToSend[i]);
  }
  sendArray(arrayToSend, sizeSend);
  BT.flush();  // Asegurarse de que el mensaje se ha enviado completamente
}
//
//
//
//
//
//---------------- INICIO: CONTROL DE BLUETOOTH --------------------

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
//---------------- FIN: CONTROL DE BLUETOOTH --------------------

