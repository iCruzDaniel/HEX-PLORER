#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_PWMServoDriver.h"
#include "hexplorer.h"
#include "defines.h"
using namespace pod;

//-----SETUP -------

// Definición del constructor (CON inicialización de miembros):
Hexplorer::Hexplorer()
    : servosLeft(0x40), servosRight(0x41)
{
}

//----------Funcion----------
void Hexplorer::init()
{
  Wire.begin();
  servosLeft.begin();
  servosLeft.setPWMFreq(60);
  servosRight.begin();
  servosRight.setPWMFreq(60);
}

void Hexplorer::setServo(uint8_t n_servo, int angulo)
{
  int duty;
  duty = map(angulo, 0, 180, pos0, pos180);

  if (n_servo < 9)
  {
    servosLeft.setPWM(n_servo, 0, duty);
  }
  else
  {
    servosRight.setPWM((n_servo - 9), 0, duty);
  }
}

/**
 * @brief Esta función posiciona el hexápodo en su cero de maquina.
 * @param x posición inicial
 * @param di angulo de desplzamiento
 * @param dA cambio de angulo
 * @param v delay entre cambios
 * @param t tiempo de seguridad entre movimientos
 * @param band variable bandera para verificar continuidad
 * @return Void
 */
void Hexplorer::arranque(int x = 35, int t = 50) // t en ms entre cambio de angulos

{
  // PATAS IZQUIERDAS
  // Pata 1
  setServo(POD_1A, 90 + x);
  setServo(POD_1C, 90);

  // Pata 3
  setServo(POD_3A, 90);
  setServo(POD_3C, 90);

  // Pata 5
  setServo(POD_5A, 90 - x);
  setServo(POD_5C, 90);

  // PATAS DERECHAS
  // Pata 2
  setServo(POD_2A, 90 - x);
  setServo(POD_2C, 90);

  // Pata 4
  setServo(POD_4A, 90);
  setServo(POD_4C, 90);

  // Pata 6
  setServo(POD_6A, 90 + x);
  setServo(POD_6C, 90);

  for (int i = 0; i <= 70; i += 2)
  {
    // PATAS IZQUIERDAS
    // Pata 1
    setServo(POD_1B, 160 - i);

    // Pata 3
    setServo(POD_3B, 160 - i);

    // Pata 5
    setServo(POD_5B, 160 - i);

    // PATAS DERECHAS
    // Pata 2
    setServo(POD_2B, 160 - i);

    // Pata 4
    setServo(POD_4B, 160 - i);

    // Pata 6
    setServo(POD_6B, 160 - i);
    delay(t);
  }
}

/**
 * @brief Esta función ejecuta el desplazamiento hacia delante o hacia detrás del hexápodo.
 * @param x posición inicial
 * @param di angulo de desplzamiento
 * @param dA cambio de angulo
 * @param v delay entre cambios
 * @param t tiempo de seguridad entre movimientos
 * @param band variable bandera para verificar continuidad
 * @return Void
 */
void Hexplorer::desplazamiento(int x = 35, int di = 35, int dA = 5, int v = 50, int t, bool invert = false, bool band = false)
{
  int inv = invert ? -1 : 1;

  // ALZAR  TRI_1 (1, 4, 5)
  if (!band)
  {
    for (int i = 0; i <= di; i += dA)
    {

      setServo(POD_1B, 90 + i);
      setServo(POD_4B, 90 + i);
      setServo(POD_5B, 90 + i);
      delay(v);
    }
  }

  // DEZPLAZAMIENTO POSITIVO TRI_1 (1, 4, 5)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_1A, 90 + x + (i * inv));
    setServo(POD_4A, 90 + (-i * inv));
    setServo(POD_5A, 90 - x + (i * inv));
    delay(v);
  }
  // BAJAR  TRI_1 (1, 4, 5)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_1B, 90 + di - i);
    setServo(POD_4B, 90 + di - i);
    setServo(POD_5B, 90 + di - i);
    delay(v);
  }
  // --------------------------------------------
  delay(t);
  // --------------------------------------------

  // ALZAR  TRI_2 (2, 3, 6)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_2B, 90 + i);
    setServo(POD_3B, 90 + i);
    setServo(POD_6B, 90 + i);
    delay(v);
  }
  // --------------------------------------------
  delay(t);
  // --------------------------------------------

  // DEZPLAZAMIENTO NEGATIVO TRI_1 (1, 4, 5)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_1A, 90 + x + inv * (-i + di));
    setServo(POD_4A, 90 + inv * (i - di));
    setServo(POD_5A, 90 - x + inv * (-i + di));
    delay(v);
  }
  // DEZPLAZAMIENTO POSITIVO TRI_2 (2, 3, 6)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_2A, 90 - x + inv * (-i));
    setServo(POD_3A, 90 + inv * (i));
    setServo(POD_6A, 90 + x + inv * (-i));
    delay(v);
  }
  // BAJAR  TRI_2 (2, 3, 6)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_2B, 90 + di - i);
    setServo(POD_3B, 90 + di - i);
    setServo(POD_6B, 90 + di - i);
    delay(v);
  }
  // --------------------------------------------
  delay(t);
  // --------------------------------------------

  // ALZAR  TRI_1 (1, 4, 5)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_1B, 90 + i);
    setServo(POD_4B, 90 + i);
    setServo(POD_5B, 90 + i);
    delay(v);
  }
  // DEZPLAZAMIENTO NEGATIVO TRI_2 (2, 3, 6)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_2A, 90 - x + inv * (i - di));
    setServo(POD_3A, 90 + inv * (-i + di));
    setServo(POD_6A, 90 + x + inv * (i - di));
    delay(v);
  }
}

/**
 * @brief Esta función ejecuta el desplazamiento rotacional en ambos sentidos del hexápodo.
 * @param x posición inicial
 * @param di angulo de desplzamiento
 * @param dA cambio de angulo
 * @param v delay entre cambios
 * @param t tiempo de seguridad entre movimientos
 * @param band variable bandera para verificar continuidad
 * @return Void
 */
void Hexplorer::giro(int x = 35, int di = 35, int dA = 5, int v = 50, int t, bool invert = false, bool band = false)
{

  int inv = invert ? -1 : 1;

  // ALZAR  TRI_1 (1, 4, 5)
  if (!band)
  {
    for (int i = 0; i <= di; i += dA)
    {

      setServo(POD_1B, 90 + i);
      setServo(POD_4B, 90 + i);
      setServo(POD_5B, 90 + i);
      delay(v);
    }
  }

  // GIRO NEGATIVO TRI_1 (1, 4, 5)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_1A, 90 + x + inv * i);
    setServo(POD_4A, 90 + inv * i);
    setServo(POD_5A, 90 - x + inv * i);
    delay(v);
  }
  // BAJAR  TRI_1 (1, 4, 5)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_1B, 90 + di - i);
    setServo(POD_4B, 90 + di - i);
    setServo(POD_5B, 90 + di - i);
    delay(v);
  }
  // --------------------------------------------
  delay(t);
  // --------------------------------------------

  // ALZAR  TRI_2 (2, 3, 6)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_2B, 90 + i);
    setServo(POD_3B, 90 + i);
    setServo(POD_6B, 90 + i);
    delay(v);
  }
  // --------------------------------------------
  delay(t);
  // --------------------------------------------

  // GIRO POSITIVO TRI_1 (1, 4, 5)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_1A, 90 + x + inv * (-i + di));
    setServo(POD_4A, 90 + inv * (-i + di));
    setServo(POD_5A, 90 - x + inv * (-i + di));
    delay(v);
  }
  // GIRO NEGATIVO TRI_2 (2, 3, 6)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_2A, 90 - x + inv * i);
    setServo(POD_3A, 90 + inv * i);
    setServo(POD_6A, 90 + x + inv * i);
    delay(v);
  }
  // BAJAR  TRI_2 (2, 3, 6)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_2B, 90 + di - i);
    setServo(POD_3B, 90 + di - i);
    setServo(POD_6B, 90 + di - i);
    delay(v);
  }
  // --------------------------------------------
  delay(t);
  // --------------------------------------------

  // ALZAR  TRI_1 (1, 4, 5)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_1B, 90 + i);
    setServo(POD_4B, 90 + i);
    setServo(POD_5B, 90 + i);
    delay(v);
  }
  // GIRO POSITIVO TRI_2 (2, 3, 6)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_2A, 90 - x + inv * (-i + di));
    setServo(POD_3A, 90 + inv * (-i + di));
    setServo(POD_6A, 90 + x + inv * (-i + di));
    delay(v);
  }
}

/**
 * @brief Esta función detiene el hexápodo.
 * @return Void
 */
void Hexplorer::detener(int di = 35, int dA = 5, int v = 50)
{
  // BAJAR  TRI_1 (1, 4, 5)
  for (int i = 0; i <= di; i += dA)
  {
    setServo(POD_1B, 90 + di - i);
    setServo(POD_4B, 90 + di - i);
    setServo(POD_5B, 90 + di - i);
    delay(v);
  }
}

// void Hexplorer::saludo(int x = 35, int di = 50, int dA = 1, int v = 30, int t = 200)
// {
//   // subir y avanzar patas 1 y 2
//   for (int i = 0; i <= di; i += dA)
//   {

//     setServo(POD_1A, 120 + x + i);
//     setServo(POD_1B, 90 + i);

//     setServo(POD_2A, 60 - x - i);
//     setServo(POD_2B, 90 + i);
//     delay(v);
//   }
//   delay(t);
//   // bajar patas 1 y 2
//   for (int i = 0; i <= di; i += dA)
//   {

//     setServo(POD_1B, 90 - i + di);

//     setServo(POD_2B, 90 - i + di);
//     delay(v);
//   }
//   delay(t);
//   // subir patas y avanzar 3 y 4
//   for (int i = 0; i <= di; i += dA)
//   {

//     setServo(POD_3A, 90 + i);
//     setServo(POD_3B, 90 + i);

//     setServo(POD_4A, 90 - i);
//     setServo(POD_4B, 90 + i);
//     delay(v);
//   }
//   delay(t);
//   // bajar patas 3 y 4
//   for (int i = 0; i <= di; i += dA)
//   {

//     setServo(POD_3B, 90 - i + di);

//     setServo(POD_4B, 90 - i + di);
//     delay(v);
//   }
//   delay(t);

//   // subir patas 1 y 2
//   for (int i = 0; i <= di; i += dA)
//   {

//     setServo(POD_1B, 90 + i);
//     setServo(POD_2B, 90 + i);
//     delay(v);
//   }
//   delay(t);

//   // --------- saludar con patas 1 y 2
//   for (int i = 0; i <= 5; i++)
//   {
//     for (int i = 0; i <= di; i += 10)
//     {
//       setServo(POD_1A, 120 + x - i + di);
//       setServo(POD_2A, 60 - x + i - di);
//       delay(50);
//     }
//     for (int i = 0; i <= di; i += 10)
//     {
//       setServo(POD_1A, 120 + x + i);
//       setServo(POD_2A, 60 - x - i);
//       delay(50);
//     }
//   }
//   delay(t);

//   // bajar patas 1 y 2
//   for (int i = 0; i <= di; i += dA)
//   {
//     setServo(POD_1B, 90 - i + di);

//     setServo(POD_2B, 90 - i + di);
//     delay(v);
//   }
//   delay(t);

//   // subir y devolver patas 3 y 4
//   for (int i = 0; i <= di; i += dA)
//   {

//     setServo(POD_3A, 90 - i + di);
//     setServo(POD_3B, 90 + i);

//     setServo(POD_4A, 90 + i - di);
//     setServo(POD_4B, 90 + i);
//     delay(v);
//   }
//   delay(t);
//   // bajar patas 3 y 4
//   for (int i = 0; i <= di; i += dA)
//   {

//     setServo(POD_3B, 90 - i + di);

//     setServo(POD_4B, 90 - i + di);
//     delay(v);
//   }
//   delay(t);

//   // subir y devoler patas 1 y 2
//   for (int i = 0; i <= di; i += dA)
//   {

//     setServo(POD_1A, 120 + x - i + di);
//     setServo(POD_1B, 90 + i);

//     setServo(POD_2A, 60 - x + i - di);
//     setServo(POD_2B, 90 + i);
//     delay(v);
//   }
//   delay(t);

//   // bajar patas 1 y 2
//   for (int i = 0; i <= di; i += dA)
//   {
//     setServo(POD_1B, 90 - i + di);

//     setServo(POD_2B, 90 - i + di);
//     delay(v);
//   }
//   delay(t);
// }

Hexplorer hexplorer = Hexplorer();