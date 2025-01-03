package com.hexaview;
import java.io.IOException;
import javafx.fxml.FXML;

public class AutomaticMappingController {

    @FXML
    //funciones de cambio de Vistas
    public void switchToAutomaticMapMode() throws IOException {

        App.setRoot("automatic-mapping-view");
    }

    public void switchToCoordinateInputMode() throws IOException {

        App.setRoot("coordinate-input-view");
    }

    public void switchToManualMode() throws IOException {

        App.setRoot("manual-mode-view");
    }
    public void switchToStatisticsMode() throws IOException {

        App.setRoot("statistics-view");
    }
}