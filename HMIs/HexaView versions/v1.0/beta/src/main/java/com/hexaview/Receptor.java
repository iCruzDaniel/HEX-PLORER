package com.hexaview;

class Receptor {

    int mode;
    int batControll;
    int batHex;
    int steps;
    boolean[] stateSteps = new boolean[6] ;
    /*
     * bool
     * bool
     * bool
     * bool
     * bool
     * boll
     */
    int lidarDistance;
    int lidarAngle;


/*
	MODO: int (0 - 1 - 2 - 3 - 4),
	BATMANDO: int (10 - 25 - 50 - 74 - 100),
	BATHEX: int (10 - 25 - 50 - 74 - 100),
	PATAS: Bool {T-F, T-F, T-F, T-F, T-F, T-F},
	//TEMPHEX: int (0 - … - 99),
	LIDAR: int (0 - … - 999),
	GRADOS: int (0 - … 180),
	STEPS: int (0 - … ),
	TIEMPO_ACTIVO: String (… ),
	INCLx: float (0   ,
	INCLy: float (0   , 
 */
}