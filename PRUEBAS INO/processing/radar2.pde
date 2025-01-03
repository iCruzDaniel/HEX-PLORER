import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

Serial myPort;
String angle="";
String distance="";
String data="";
String noObject;
float pixsDistance;
int iAngle, iDistance;
int index1=0;
int index2=0;
PFont orcFont;

void setup() {
  size(800, 800);
  smooth();
  myPort = new Serial(this,"COM12", 9600); 
  myPort.bufferUntil('.');
  orcFont = loadFont("Arial-Black-20.vlw");
}

void draw() {
  fill(98, 245, 31);
  textFont(orcFont);
  noStroke();
  fill(0, 4);
  rect(0, 0, width, height);

  fill(98, 245, 31);
  drawRadar();
  drawLine();
  drawObject();
  drawText();
}

void serialEvent(Serial myPort) {
  data = myPort.readStringUntil('.');
  data = data.substring(0, data.length() - 1);

  index1 = data.indexOf(",");
  angle = data.substring(0, index1);
  distance = data.substring(index1 + 1, data.length());

  iAngle = int(angle);
  iDistance = int(distance);
}

void drawRadar() {
  pushMatrix();
  translate(width/2, height/2);
  noFill();
  strokeWeight(2);
  stroke(98, 245, 31);
  textSize(12);
  // Calcular el radio del círculo
  float circleRadius = max(width, height) / 2;

  // Dibujar círculo exterior
  ellipse(0, 0, circleRadius * 2, circleRadius * 2);
  // Dibujar líneas horizontales para las distancias en el eje Y positivo
  for (int d = 50; d <= 300; d += 50) {
    float y = map(d, 0, 300, 0, height/2);
    line(-10, -y, 10, -y); // Línea corta para cada distancia
    textAlign(RIGHT, CENTER);
    text(d, -15, -y); // Etiqueta de la distancia
  }

  // Dibujar líneas verticales para las distancias en el eje X positivo
  for (int d = 50; d <= 300; d += 50) {
    float x = map(d, 0, 300, 0, width/2);
    line(x, -10, x, 10); // Línea corta para cada distancia
    textAlign(CENTER, TOP);
    text(d, x, -15); // Etiqueta de la distancia
  }
  // Dibujar círculos adicionales en distancias específicas
  drawCircleAtDistance(50, circleRadius);
  drawCircleAtDistance(100, circleRadius);
  drawCircleAtDistance(150, circleRadius);
  drawCircleAtDistance(200, circleRadius);
  drawCircleAtDistance(250, circleRadius);
  // Dibujar líneas para los ángulos principales (opcional)
  for (int i = 0; i < 360; i += 30) {
    float x = cos(radians(i)) * width/2;
    float y = sin(radians(i)) * height/2;
    line(0, 0, x, y);
  }
  popMatrix();
}
void drawCircleAtDistance(int distance, float circleRadius) {
  float d = map(distance, 0, 300, 0, circleRadius);
  ellipse(0, 0, d * 2, d * 2);
}

void drawObject() {
  pushMatrix();
  translate(width/2, height/2);
  strokeWeight(10);
  stroke(255, 10, 10);
  pixsDistance = iDistance * 10;

  if (iDistance < 50 && iDistance > 0) {
    float x = cos(radians(-iAngle)) * pixsDistance;
    float y = sin(radians(-iAngle)) * pixsDistance;
    point(x, y);
  }
  popMatrix();
}

void drawLine() {
  pushMatrix();
  strokeWeight(5);
  stroke(30, 250, 60);
  translate(width/2, height/2);
  line(0, 0, cos(radians(-iAngle)) * height / 2, sin(radians(-iAngle)) * height / 2);
  popMatrix();
}

void drawText() {
  pushMatrix();
  fill(0);
  noStroke();
  rect(0, height - 50, width, height);
  fill(98, 245, 31);
  textSize(20);
  textAlign(LEFT);
  text("Distance: " + iDistance + " cm", 20, height - 20);
  popMatrix();
}
