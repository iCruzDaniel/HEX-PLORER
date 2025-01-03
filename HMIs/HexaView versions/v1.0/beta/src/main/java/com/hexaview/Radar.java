package com.hexaview;

import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.scene.text.TextAlignment;

public class Radar {
    //CONSTRUCTOR...
    private GraphicsContext gc;
    private int width;
    private int height;
    
    public Radar(GraphicsContext gc, int width, int height) {
        this.gc = gc;
        this.width = width;
        this.height = height;
    }
    
    private String data;
    private int index1;
    private float angle;
    private float distance;
    private int iAngle;
    private int iDistance;
    private float pixsDistance;
    
    public void drawRadar(int distance, int angle) {
        this.iAngle = angle;
        this.iDistance = distance;
        gc.clearRect(0, 0,  this.width, this.height);
        drawText(gc, this.width, this.height);
        drawLine(gc, this.width, this.height);
        drawObject(gc, this.width, this.height);
        drawArea(gc, this.width, this.height);
    }
    /*public void clearRadar() {
        // Limpia el Canvas
        gc.clearRect(0, 0, 100, canvas.getHeight());
    }*/

    /*public void serialEvent(String serialData) {
        data = serialData.substring(0, serialData.length() - 1);

        index1 = data.indexOf(",");
        angle = Float.parseFloat(data.substring(0, index1));
        distance = Float.parseFloat(data.substring(index1 + 1, data.length()));

        iAngle = (int) angle;
        iDistance = (int) distance;
    }*/


    public void drawArea(GraphicsContext gc, double width, double height) {
        gc.save();
        gc.translate(width / 2, height / 2);
        gc.setStroke(Color.rgb(98, 245, 31));
        gc.setLineWidth(2);
        gc.setTextAlign(TextAlignment.RIGHT);
        gc.setFont(javafx.scene.text.Font.font(12));
        
        // Calcular el radio del círculo
        double circleRadius = Math.max(width, height) / 2;

        // Dibujar círculo exterior
        gc.strokeOval(-circleRadius, -circleRadius, circleRadius * 2, circleRadius * 2);
        // Dibujar líneas horizontales para las distancias en el eje Y positivo
        for (int d = 50; d <= 300; d += 50) {
            double y = map(d, 0, 300, 0, height / 2);
            gc.strokeLine(-10, -y, 10, -y); // Línea corta para cada distancia
            gc.fillText(String.valueOf(d), -15, -y); // Etiqueta de la distancia
        }

        // Dibujar líneas verticales para las distancias en el eje X positivo
        for (int d = 50; d <= 300; d += 50) {
            double x = map(d, 0, 300, 0, width / 2);
            gc.strokeLine(x, -10, x, 10); // Línea corta para cada distancia
            gc.fillText(String.valueOf(d), x, -15); // Etiqueta de la distancia
        }
        // Dibujar círculos adicionales en distancias específicas
        drawCircleAtDistance(gc, 50, circleRadius);
        drawCircleAtDistance(gc, 100, circleRadius);
        drawCircleAtDistance(gc, 150, circleRadius);
        drawCircleAtDistance(gc, 200, circleRadius);
        drawCircleAtDistance(gc, 250, circleRadius);
        // Dibujar líneas para los ángulos principales (opcional)
        for (int i = 0; i < 360; i += 30) {
            double x = Math.cos(Math.toRadians(i)) * width ;
            double y = Math.sin(Math.toRadians(i)) * height ;
            gc.strokeLine(0, 0, x, y);
        }
        gc.restore();
    }

    private double map(double value, double start1, double stop1, double start2, double stop2) {
        return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
    }

    private void drawCircleAtDistance(GraphicsContext gc, int distance, double circleRadius) {
        double d = map(distance, 0, 300, 0, circleRadius);
        gc.strokeOval(-d, -d, d * 2, d * 2);
    }

    public void drawObject(GraphicsContext gc, double width, double height) {
        gc.save();
        gc.translate(width / 2, height / 2);
        gc.setStroke(Color.rgb(255, 10, 10));
        gc.setLineWidth(10);
        pixsDistance = iDistance * 10;

        if (iDistance < 50 && iDistance > 0) {
            double x = Math.cos(Math.toRadians(-iAngle)) * pixsDistance;
            double y = Math.sin(Math.toRadians(-iAngle)) * pixsDistance;
            gc.strokeLine(x, y, x, y);
        }
        gc.restore();
    }

    public void drawLine(GraphicsContext gc, double width, double height) {
        gc.save();
        gc.setStroke(Color.rgb(30, 250, 60));
        gc.setLineWidth(5);
        gc.translate(width / 2, height / 2);
        gc.strokeLine(0, 0, Math.cos(Math.toRadians(-iAngle)) * height, Math.sin(Math.toRadians(-iAngle)) * height);
        gc.restore();
    }

    public void drawText(GraphicsContext gc, double width, double height) {
        gc.setFill(Color.GREY);
        gc.fillRect(0, height - 50, width, height);
        gc.setFill(Color.rgb(98, 245, 31));
        gc.setFont(javafx.scene.text.Font.font(20));
        gc.setTextAlign(TextAlignment.LEFT);
        gc.fillText("Distance: " + (iDistance) + " cm", 20, height - 20);
    }
    
}
