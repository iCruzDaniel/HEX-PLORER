package com.hexaview;

import java.io.IOException;

import com.hexaview.Radar;
//import com.hexaview.Radar2;
import com.panamahitek.ArduinoException;
import com.panamahitek.PanamaHitek_Arduino;
import com.panamahitek.PanamaHitek_MultiMessage;

import javafx.animation.AnimationTimer;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.Pane;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;
import processing.core.PApplet;
import javafx.scene.Node;
import processing.javafx.PSurfaceFX;

//import processing.javafx.PSurfaceFX;

public class DefaultController {
    // Variables
    public int modo = 0;
    public boolean conexion = false;

    Receptor receptor = new Receptor();

    PanamaHitek_Arduino ino = new PanamaHitek_Arduino();

    @FXML
    private AnchorPane RADAR;
    //////////////////

    // Crear un canvas para dibujar el radar
    @FXML // Referencia al Canvas
    private Canvas canvas = new Canvas(500, 274);

    @FXML
    private Circle paw1;
    @FXML
    private Circle paw2;
    @FXML
    private Circle paw3;
    @FXML
    private Circle paw4;
    @FXML
    private Circle paw5;
    @FXML
    private Circle paw6;

    @FXML
    private ComboBox<String> COMPorts;

    @FXML
    private AnchorPane mode1, mode2, mode3, mode4;

    // Botones de la barra de navegación
    @FXML
    private Button manualModeButton, statisticsButton, automaticMapButton, coordinateInputButton;

    // Textos modificables
    @FXML
    private Label deviceName, modeName, activeTime, servosVel, distanceTravel, totalSteps, conexionStateMessage;

    // Texto Sensores
    @FXML
    private Label tempUCValue, lidarValue;

    // Estados
    @FXML
    private Pane frameControllState, frameBatHex, frameBatControll;

    // CORE DEL PROGRAMA
    void core() {
        // CONFIGURACIONES INICIALES
        initComboBox(); // Inicializar el ComboBox
        ComboBoxCOM(); // Desplegar en el ComboBox los COM disponibles y ejecuta comunicación serial
        deviceNameText("TROTAMONTAÑAS"); // Establecer nombre del HEXAPOD
        initRadar(); // Inicializamos el radar

        // LOOP DE REFRESCO DE VARIABLES
        // INICIO: Se inicializa el hilo correspondiente al graficador del Radar
        new Thread(() -> {

            int actualVista = receptor.mode;
            while (true) {
                // this.receptor.lidarAngle += 10;
                // this.receptor.lidarDistance += 2;
                try {
                    Thread.sleep(100);
                    if (receptor.mode != actualVista) {
                        Platform.runLater(() -> selectMode(receptor.mode));
                        actualVista = receptor.mode;
                    }

                    batControll(receptor.batControll);
                    batHex(receptor.batHex);
                    Platform.runLater(() ->lidar(receptor.lidarDistance));
                    pawState();

                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }

        }).start(); // FIN: Ejecución del graficador

    }

    // FUNCION DE INICIALIZACIÓN...
    @FXML
    private void initialize() {
        core();

        // Dibujar el radar en el canvas
        // radar.drawRadar(canvas.getGraphicsContext2D(), 500, 274);
        // radar.drawText(, 500, 274);
        // radar.drawObject(canvas.getGraphicsContext2D(), 500, 274);

        // System.out.println(arduino.getSerialPorts());
        // COMPorts.setItems((ObservableList<String>) arduino.getSerialPorts());

        // rectifica la conexión del Arduino

        // controllConected(true);
        // batHex(67);
        // batControll(5);
        // distanceTravelText("67");
        // tempUC("89");
        // selectMode(this.modo);
        // System.out.println("EJECUTADO");
        // initComm();
        // beginSerialComm(COMPort);
        // beginSerialComm("COM4");

        // batHex(40);
    }

    ////////////
    public void initComboBox() {
        // Agregar un ChangeListener al ComboBox para detectar cambios en la selección
        COMPorts.getSelectionModel().selectedItemProperty()
                .addListener((ChangeListener<? super String>) new ChangeListener<String>() {
                    @Override
                    public void changed(ObservableValue<? extends String> observable, String oldValue, String COMPort) {
                        // Aquí se maneja el evento de cambio de selección aquí
                        System.out.println("Nuevo puerto seleccionado: " + COMPort);
                        beginSerialComm(COMPort); // Asignamos el Puerto COM seleccionado
                        controllConected(true); // Establecer estadod e conectado

                    }
                });
    }

    // @FXML
    private void showAlert() {
        // Crear una instancia de Alert
        Alert alert = new Alert(AlertType.INFORMATION);
        alert.setTitle("CONEXIÓN FALLIDA");
        alert.setHeaderText(null);
        alert.setContentText("Es posible que el puerto COM esté ocupado");

        // Mostrar la alerta y esperar a que el usuario la cierre
        alert.showAndWait();
    }

    // Comunicación serial con el Micro
    public void beginSerialComm(String COM) { // Hay que ponerle un argumento para el puerto COM
        this.conexion = true;
        new Thread(() -> {

            // Inicializar la conexión con el Arduino
            // PanamaHitek_Arduino ino = new PanamaHitek_Arduino();
            PanamaHitek_MultiMessage multi = new PanamaHitek_MultiMessage(12, ino);
            SerialPortEventListener listener = new SerialPortEventListener() {
                @Override
                public void serialEvent(SerialPortEvent spe) {

                    try {
                        if (multi.dataReceptionCompleted()) {
                            // modo
                            receptor.mode = Integer.parseInt(multi.getMessage(0));
                            System.out.println(multi.getMessage(0));

                            // Batería Control
                            receptor.batControll = Integer.parseInt(multi.getMessage(1));
                            System.out.println(multi.getMessage(1));

                            // Bateria Hexapodo
                            receptor.batHex = Integer.parseInt(multi.getMessage(2));
                            System.out.println(multi.getMessage(2));

                            // Número de pisadas
                            receptor.steps = Integer.parseInt(multi.getMessage(3));
                            System.out.println(multi.getMessage(3));
                            // Estado patas
                            receptor.stateSteps[0] = Boolean.parseBoolean(multi.getMessage(4));
                            System.out.println(receptor.stateSteps[0]);
                            receptor.stateSteps[1] = Boolean.parseBoolean(multi.getMessage(5));
                            System.out.println(multi.getMessage(5));
                            receptor.stateSteps[2] = Boolean.parseBoolean(multi.getMessage(6));
                            System.out.println(multi.getMessage(6));
                            receptor.stateSteps[3] = Boolean.parseBoolean(multi.getMessage(7));
                            System.out.println(multi.getMessage(7));
                            receptor.stateSteps[4] = Boolean.parseBoolean(multi.getMessage(8));
                            System.out.println(multi.getMessage(8));
                            receptor.stateSteps[5] = Boolean.parseBoolean(multi.getMessage(9));
                            System.out.println(multi.getMessage(9));

                            // Distanci y Angulo del lidar.
                            receptor.lidarDistance = Integer.parseInt(multi.getMessage(10));
                            System.out.println(multi.getMessage(10));
                            receptor.lidarAngle = Integer.parseInt(multi.getMessage(11));
                            System.out.println(multi.getMessage(11));
                            multi.flushBuffer();
                        }
                    } catch (ArduinoException | SerialPortException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }

                    /*
                     * try {
                     * if (ino.isMessageAvailable()) {
                     * 
                     * // updateData();
                     * 
                     * int valor = Integer.parseInt(ino.printMessage());
                     * batControll(valor);
                     * System.out.println(valor);
                     * }
                     * } catch (SerialPortException | ArduinoException e) {
                     * // TODO Auto-generated catch block
                     * e.printStackTrace();
                     * }
                     */
                }

            };

            try {
                ino.arduinoRX(COM, 9600, listener);
                multi.flushBuffer();
                ino.flushSerialPort();
            } catch (ArduinoException | SerialPortException e) {
                // TODO Auto-generated catch block
                Platform.runLater(() -> showAlert());
                e.printStackTrace();
            }
        }).start();
    }

    public void ComboBoxCOM() {
        // Crear una lista observable para almacenar los nombres de los puertos
        ObservableList<String> portNames = FXCollections.observableArrayList();
        portNames.addAll(ino.getSerialPorts()); // Agregar puertos COM
        COMPorts.setItems(portNames);
    }

    // Cargar Radar
    public void initRadar() {

        // Crear un canvas para dibujar el radar
        Canvas canvas = new Canvas(500, 310);
        RADAR.getChildren().add(canvas);

        // Crear una instancia de la clase Radar
        Radar radar = new Radar(canvas.getGraphicsContext2D(), 500, 310);

        AnimationTimer timer = new AnimationTimer() {

            @Override
            public void handle(long now) {

                // Lógica de actualización del Canvas
                // Por ejemplo, dibujar algo en el Canvas
                try {
                    radar.drawRadar(receptor.lidarDistance , receptor.lidarAngle);
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        };
        timer.start(); // Inicio de Hilo de animación

    }

    // Selección de modos
    public void get1() throws IOException {
        this.modo = 1;
        selectMode(this.modo);
    }

    public void get2() throws IOException {
        this.modo = 2;
        selectMode(this.modo);
    }

    public void get3() throws IOException {
        this.modo = 3;
        selectMode(this.modo);
    }

    public void get4() throws IOException {
        this.modo = 4;
        selectMode(this.modo);
    }

    public void selectMode(int mode) {
        manualModeButton.setStyle(
                "-fx-background-color: #5E5E5; -fx-border-color: #BA009C; -fx-background-radius:  15 15 15 15; -fx-border-radius: 15 15 15 15; -fx-border-width: 3");
        statisticsButton.setStyle(
                "-fx-background-color: #5E5E5; -fx-border-color: #BA009C; -fx-background-radius: 15 15 15 15; -fx-border-radius: 15 15 15 15; -fx-border-width: 3");
        automaticMapButton.setStyle(
                "-fx-background-color: #5E5E5; -fx-border-color: #BA009C; -fx-background-radius:  15 15 15 15; -fx-border-radius: 15 15 15 15; -fx-border-width: 3");
        coordinateInputButton.setStyle(
                "-fx-background-color: #5E5E5; -fx-border-color: #BA009C; -fx-background-radius:  15 15 15 15; -fx-border-radius: 15 15 15 15; -fx-border-width: 3");

        mode1.setVisible(false);
        mode2.setVisible(false);
        mode3.setVisible(false);
        mode4.setVisible(false);
        switch (mode) {
            case 1:
                mode1.setVisible(true);
                modeName.setText("MANUAL");
                manualModeButton.setStyle(
                        "-fx-background-color: #BA009C; -fx-border-color: #BA009C; -fx-background-radius:  15 15 15 15; -fx-border-radius: 15 15 15 15; -fx-border-width: 3");
                break;
            case 2:
                mode2.setVisible(true);
                modeName.setText("ESTADISTICAS GENERALES");
                statisticsButton.setStyle(
                        "-fx-background-color: #BA009C; -fx-border-color: #BA009C; -fx-background-radius:  15 15 15 15; -fx-border-radius: 15 15 15 15; -fx-border-width: 3");
                break;
            case 3:
                mode3.setVisible(true);
                modeName.setText("MAPEO AUTOMÁTICO");
                automaticMapButton.setStyle(
                        "-fx-background-color: #BA009C; -fx-border-color: #BA009C; -fx-background-radius:  15 15 15 15; -fx-border-radius: 15 15 15 15; -fx-border-width: 3");
                break;
            case 4:
                mode4.setVisible(true);
                modeName.setText("COORDENADAS");
                coordinateInputButton.setStyle(
                        "-fx-background-color: #BA009C; -fx-border-color: #BA009C; -fx-background-radius:  15 15 15 15; -fx-border-radius: 15 15 15 15; -fx-border-width: 3");
                break;

            default:
                break;
        }
    }

    // funciones de cambio de Vistas
    public void switchToAutomaticMapMode() throws IOException {
        // App.setRoot("automatic-mapping-view");

    }

    public void switchToCoordinateInputMode() throws IOException {
        // App.setRoot("coordinate-input-view");
    }

    public void switchToManualMode() throws IOException {
        // App.setRoot("manual-mode-view");
    }

    public void switchToStatisticsMode() throws IOException {
        // App.setRoot("statistics-view");
    }

    // funciones de estado: Estado de conexión del mando, Batería Hexapodo, Batería
    // Mando,
    public void controllConected(boolean state) {
        if (state) {
            conexionMessage(state);
            frameControllState.setStyle(
                    "-fx-border-color: #0EE2FF; -fx-background-radius:  8 8 8 8; -fx-border-radius:  8 8 8 8; -fx-border-width: 3");
        } else {
            conexionMessage(state);
            frameControllState.setStyle(
                    "-fx-border-color: grey; -fx-background-radius:  8 8 8 8; -fx-border-radius:  8 8 8 8; -fx-border-width: 3");
        }
    }

    public void batHex(int porcent) {

        if (this.conexion) {

            if (porcent <= 15) {
                frameBatHex.setStyle(
                        "-fx-border-color: red; -fx-background-radius:  8 8 8 8; -fx-border-radius:  8 8 8 8; -fx-border-width: 3");
            } else if ((porcent > 15) && (porcent <= 50)) {
                frameBatHex.setStyle(
                        "-fx-border-color: yellow; -fx-background-radius:  8 8 8 8; -fx-border-radius:  8 8 8 8; -fx-border-width: 3");
            } else if ((porcent > 50) && (porcent <= 100)) {
                frameBatHex.setStyle(
                        "-fx-border-color: #30D94B; -fx-background-radius:  8 8 8 8; -fx-border-radius:  8 8 8 8; -fx-border-width: 3");
            }
        } else {
            frameBatHex.setStyle(
                    "-fx-border-color: grey; -fx-background-radius:  8 8 8 8; -fx-border-radius:  8 8 8 8; -fx-border-width: 3");

        }
    }

    public void batControll(int porcent) {
        if (this.conexion) {

            if (porcent <= 15) {
                frameBatControll.setStyle(
                        "-fx-border-color: red; -fx-background-radius:  8 8 8 8; -fx-border-radius:  8 8 8 8; -fx-border-width: 3");
            } else if ((porcent > 15) && (porcent <= 50)) {
                frameBatControll.setStyle(
                        "-fx-border-color: yellow; -fx-background-radius:  8 8 8 8; -fx-border-radius:  8 8 8 8; -fx-border-width: 3");
            } else if ((porcent > 50) && (porcent <= 100)) {
                frameBatControll.setStyle(
                        "-fx-border-color: #30D94B; -fx-background-radius:  8 8 8 8; -fx-border-radius:  8 8 8 8; -fx-border-width: 3");
            }
        } else {
            frameBatControll.setStyle(
                    "-fx-border-color: grey; -fx-background-radius:  8 8 8 8; -fx-border-radius:  8 8 8 8; -fx-border-width: 3");
        }
    }

    public void conexionMessage(boolean state) {
        ///
        if (state) {
            conexionStateMessage.setText("CONECTADO");
        } else {
            conexionStateMessage.setText("DESCONECTADO");
        }
    }

    // funciones de Info general
    public void deviceNameText(String name) {
        deviceName.setText(name);
    }

    public void modeNameText(String text) {
        modeName.setText(text);
    }

    public void activeTimeText(String text) {
        activeTime.setText(text + "s");
    }

    public void servosVelText(String text) {
        servosVel.setText(text + "%");
    }

    public void distanceTravelText(String text) {
        distanceTravel.setText(text + "m");
    }

    public void totalStepsText(String text) {
        totalSteps.setText(text);
    }

    // Info de los Sensores...
    public void tempUC(String temp) {
        tempUCValue.setText(temp + " °C");
    }

    public void lidar(int distance) {
        if(distance <= 35) {
            lidarValue.setTextFill(Color.RED);
            lidarValue.setText(distance + " cm");
        }
        else {
            lidarValue.setTextFill(Color.WHITE);
            lidarValue.setText(distance + " cm");
        }
        
    }

    public void pawState() {

        if (this.conexion) {
            if (receptor.stateSteps[0]) {
                paw1.setFill(Color.GREEN);
            } else {
                paw1.setFill(Color.RED);
            }
            if (receptor.stateSteps[1]) {
                paw2.setFill(Color.GREEN);
            } else {
                paw2.setFill(Color.RED);
            }
            if (receptor.stateSteps[2]) {
                paw3.setFill(Color.GREEN);
            } else {
                paw3.setFill(Color.RED);
            }
            if (receptor.stateSteps[3]) {
                paw4.setFill(Color.GREEN);
            } else {
                paw4.setFill(Color.RED);
            }
            if (receptor.stateSteps[4]) {
                paw5.setFill(Color.GREEN);
            } else {
                paw5.setFill(Color.RED);
            }
            if (receptor.stateSteps[5]) {
                paw6.setFill(Color.GREEN);
            } else {
                paw6.setFill(Color.RED);
            }
        } else {
            paw1.setFill(Color.GREY);
            paw2.setFill(Color.GREY);
            paw3.setFill(Color.GREY);
            paw4.setFill(Color.GREY);
            paw5.setFill(Color.GREY);
            paw6.setFill(Color.GREY);
        }
    }
}