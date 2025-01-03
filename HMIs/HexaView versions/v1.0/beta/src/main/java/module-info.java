module com.hexaview {
    requires javafx.controls;
    requires javafx.fxml;
    requires PanamaHitek.Arduino;
    requires jssc;
    requires core;
    requires javafx.graphics;
    //requires processing.core;
    

    opens com.hexaview to javafx.fxml;
    exports com.hexaview;
}
