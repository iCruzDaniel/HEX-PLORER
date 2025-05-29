#ifndef hexplorer_h
#define hexplorer_h

#include "Adafruit_PWMServoDriver.h"

class Hexplorer
{
public:
	Hexplorer();
	void init();
	void setServo(uint8_t n_servo, int angulo);
	void arranque(int x = 35, int t = 50);
	void desplazamiento(int x = 35, int di = 35, int dA = 5, int v = 50, int t=200, bool invert = false, bool band = false);
	void giro(int x = 35, int di = 35, int dA = 5, int v = 50, int t=200, bool invert = false, bool band = false);
	void saludo(int x = 35, int di = 50, int dA = 1, int v = 30, int t = 200);
	void detener(int di = 35, int dA = 5, int v = 50);

private:
	Adafruit_PWMServoDriver servosLeft;
	Adafruit_PWMServoDriver servosRight;

	static const uint16_t pos0 = 172;
	static const uint16_t pos180 = 565;

	// static const uint16_t SERVO_MIN = 150;
	// static const uint16_t SERVO_MAX = 600;
};

extern Hexplorer hexplorer; // Declaración de la instancia externa

#endif