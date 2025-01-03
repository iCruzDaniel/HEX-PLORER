//----------------Librerias----------------
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//----------------Pines----------------
#define CE_PIN 9
#define CSN_PIN 10

RF24 radio(CE_PIN, CSN_PIN);

//---------------- Control RF --------------------
const byte address[][6] = {"00001", "00002"};  // Direcciones para comunicar ambos Arduinos
const int vectorSizeSend = 4;         // Tamaño del vector de enviado
const int vectorSizeRecieve = 6;         // Tamaño del vector de recibido
int sendVector[vectorSizeSend];       // Vector para enviar
int receivedVector[vectorSizeRecieve];   // Vector para recibir


void setup() 
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address[0]);      // Dirección para enviar datos
  radio.openReadingPipe(1, address[1]);   // Dirección para recibir datos
  radio.setPALevel(RF24_PA_LOW);
}


void loop() 
{
  // Leer los valores del potenciómetro y llenar el vector
  for (int i = 0; i < vectorSize; i++) 
  {
    sendVector[i] = analogRead(potPin);
    sendVector[i] = map(sendVector[i], 0, 1023, 0, 180);
  }
  
  // Enviar el vector
  radio.stopListening();        // Detener la escucha para enviar datos
  bool success = radio.write(&sendVector, sizeof(sendVector));
  
  if (success) 
  {
    Serial.print("Enviado: ");
    for (int i = 0; i < vectorSize; i++) 
    {
      Serial.print(sendVector[i]);
      Serial.print(" ");
    }
    Serial.println();
  } 
  else 
  {
    Serial.println("Envio fallido");
  }

  // Esperar un poco antes de cambiar al modo de recepción
  delay(10);

  // Recibir el vector del otro Arduino
  radio.startListening();               // Iniciar la escucha para recibir datos
  unsigned long startTime = millis();   // Tiempo de inicio para el tiempo de espera
  
  while (!radio.available()) 
  {
    if (millis() - startTime > 500) 
    {  // Tiempo de espera de 500ms
      Serial.println("Tiempo de espera agotado");
      break;
    }
  }

  if (radio.available()) 
  {
    radio.read(&receivedVector, sizeof(receivedVector));
    Serial.print("Recibido: ");
    for (int i = 0; i < vectorSize; i++) 
    {
      Serial.print(receivedVector[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
  // Esperar antes de la siguiente iteración
  delay(1000);
}
