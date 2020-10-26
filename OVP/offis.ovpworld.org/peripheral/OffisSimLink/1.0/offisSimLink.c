/* -----------------------------------------------------------------------
 * Copyright (c) 2010     OFFIS Institute for Information Technology,
 *                        26121 Oldenburg, Germany
 *
 * All rights reserved.
 *
 * This file is directly or indirectly part of the OFFIS Virtual Platform.
 * A simualtion of an ARM7tdmi micro processor with OVP (a), FreeRTOS
 * operating system (b) and user defined tasks.
 *
 * (a) http://www.ovpworld.org/
 * (b) http://www.freertos.org/
 *
 * Created for the European projects: Nephron+ (1)
 *
 * 1) http://www.nephronplus.eu/
 *
 * A list of authors and contributors can be found in the accompanying
 * AUTHORS file.  For detailed copyright information, please refer
 * to the COPYING file.
 *
 * -----------------------------------------------------------------------
 * Contact information:
 *  OFFIS
 *    Institute for Information Technology
 *  Dipl.-Inform. Frank Poppen
 *    Escherweg 2
 *    D-26121 Oldenburg
 *    Germany
 *  www  : http://www.offis.de/
 *  phone: +49 (441) 9722-230
 *  fax  : +49 (441) 9722-128
 * -----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>

// tell simLink.h if we compile for PSE and not for semihost (Windwos DLL)
#define PSE
#include "offisSimLink.h"

//define no inline attribute for intercepted functions 
#define NOINLINE __attribute__((noinline)) 


////////////////////// Declaration of peripheral wide data structures ///////////////////////////////

OFFIS_sl_RegisterT OFFIS_sl_sPort; // structure with all public slave port registers
handles_type handles;              // structure of handles to ports and nets

//////////////////////////////// Bus Slave Ports ///////////////////////////////

// Offset | Regsiter
// =============================
// 0x0000 | irq0VectAddr (IRQ from Real Time Board)
// 0x0004 | irq1VectAddr (IRQ from Main Board)
// 0x0008 | irq2VectAddr (IRQ from Communication Board)
// 0x000c | irq3VectAddr (IRQ from Power Board)
// 0x0010 | actuator0
// 0x0014 | sensor0
// 0x0018 | actuator1
// 0x001c | sensor1
//   ...  |   ...

static void installSlavePorts(void)
{
	handles.OFFIS_sl_sPort = ppmCreateSlaveBusPort("sp", (2*NumberActuatorSensor*4)+16);  // size in bytes +4*4 for IRQ vectors
}

/////////////////////////////////// Net Ports //////////////////////////////////

static void installNetPorts(void)
{
	// To write to this net, use ppmWriteNet(handles.IRQ*, value);
	handles.IRQo0  = ppmOpenNetPort("IRQo0"); // (IRQ from Real Time Board)
	handles.IRQo1  = ppmOpenNetPort("IRQo1"); // (IRQ from Main Board, UI)
	handles.IRQo2  = ppmOpenNetPort("IRQo2"); // (IRQ from Communication Board)
	handles.IRQo3  = ppmOpenNetPort("IRQo3"); // (IRQ from Power Board)
}

// When simulation is started without Matlab/Simulink available, we will
// not use the intercepted function below to connect to Simulink. Instead
// we use this dummy function to generate some slopes for testing sensor
// readout.
void syncMatlabDUMMY(OFFIS_sl_RegisterT *pOFFIS_sl_sPort) {
	int i = 0;
	static int j = 0;
	for (i = 0; i < NumberActuatorSensor; i++) {
		pOFFIS_sl_sPort->sensorValue[i] = j;
	}
	// Build a nice rising slope on Sensor0 which is Na+
	// Starting at 132 we should see a warning, go away and come back.
	pOFFIS_sl_sPort->sensorValue[0] = 1320000+j*166;
	// Build a nice rising slope on Sensor1 which is K+
	// Starting at 1 we should see a warning, go away and come back.
	pOFFIS_sl_sPort->sensorValue[1] = 10000+j*83;
	// Build a nice rising slope on Sensor2 which is Urea
	// Starting at 0 we should see a warning, go away and come back.
	pOFFIS_sl_sPort->sensorValue[2] = 0+j*166;
	j++;
} 

///////////////////////////// Interface functions to host native DLL  ////////////////////////////

NOINLINE void syncMatlab(Uns32 p_sPort, Uns32 strlenSimulinkModelName, char * simulinkModelName) { 
  // fatal error to reach this point 
  bhmMessage("F", PREFIX, "OFFIS: syncMatlab should be intercepted"); 
} 

NOINLINE void startMatlab(Uns32 strlenSimulinkModelPath, char * simulinkModelPath, Uns32 strlenSimulinkModelName, char * simulinkModelName)
{
  // fatal error to reach this point 
  bhmMessage("F", PREFIX, "OFFIS: startMatlab should be intercepted"); 
}

NOINLINE void closeMatlab()
{ 
  // fatal error to reach this point 
  bhmMessage("F", PREFIX, "OFFIS: closeMatlab should be intercepted"); 
} 

///////////////////////////// MMR Generic callbacks ////////////////////////////

static PPM_VIEW_CB(view32) {  *(Uns32*)data = *(Uns32*)user; }

PPM_REG_WRITE_CB(wcb_irq0VectAddr) {
	// Writing to register irqVectAddr does not set the value for future reads from it.
	// Rather, this register should be written near the end of an ISR, to clear the reset.
	ppmWriteNet(handles.IRQo0, 0);
}
PPM_REG_WRITE_CB(wcb_irq1VectAddr) {
	// Writing to register irqVectAddr does not set the value for future reads from it.
	// Rather, this register should be written near the end of an ISR, to clear the reset.
	ppmWriteNet(handles.IRQo1, 0);
}
PPM_REG_WRITE_CB(wcb_irq2VectAddr) {
	// Writing to register irqVectAddr does not set the value for future reads from it.
	// Rather, this register should be written near the end of an ISR, to clear the reset.
	ppmWriteNet(handles.IRQo2, 0);
}
PPM_REG_WRITE_CB(wcb_irq3VectAddr) {
	// Writing to register irqVectAddr does not set the value for future reads from it.
	// Rather, this register should be written near the end of an ISR, to clear the reset.
	ppmWriteNet(handles.IRQo3, 0);
}

PPM_REG_READ_CB(rcb_irqVectAddr) {
  // bhmMessage("W", PREFIX, "Reading from OFFIsLink IRQ address!"); 
  return *(Uns32*)user;
}

PPM_REG_READ_CB(rcb_sensor) {
  return *(Uns32*)user;
}

PPM_REG_WRITE_CB(wcb_sensor) {
  bhmMessage("F", PREFIX, "Writing to sensor address not defined!"); 
}

PPM_REG_READ_CB(rcb_actuator) {
  bhmMessage("W", PREFIX, "Reading from actuator address!"); 
  return *(Uns32*)user;
}

PPM_REG_WRITE_CB(wcb_actuator) {
  *(Uns32*)user = data;
}

PPM_CONSTRUCTOR_CB(periphConstructor);

PPM_CONSTRUCTOR_CB(constructor) {
  // YOUR CODE HERE (pre constructor)
  periphConstructor();
  // YOUR CODE HERE (post constructor)
}

PPM_DESTRUCTOR_CB(destructor) {
	// YOUR CODE HERE (destructor)
	
	Uns32 strlenSimulinkModelPath;
	if (bhmIntegerAttribute("strlenSimulinkModelPath", &strlenSimulinkModelPath)) {
		// bhmMessage("I", PREFIX, "The model path has #characters: %d", strlenSimulinkModelPath); 
	} else {
		bhmMessage("F", PREFIX, "Number characters for \"strlenSimulinkModelPath\" could not be read!"); 
	}
	Uns32 strlenSimulinkModelName;
	if (bhmIntegerAttribute("strlenSimulinkModelName", &strlenSimulinkModelName)) {
		// bhmMessage("I", PREFIX, "The model Name has #characters: %d", strlenSimulinkModelName); 
	} else {
		bhmMessage("F", PREFIX, "Number characters for \"strlenSimulinkModelName\" could not be read!"); 
	}
	char *simulinkModelPath = malloc(strlenSimulinkModelPath);
	if (bhmStringAttribute("simulinkModelPath", simulinkModelPath, strlenSimulinkModelPath)) { 
		// bhmMessage("I", PREFIX, "The model path is %s", simulinkModelPath); 
	} else{
		bhmMessage("F", PREFIX, "No model path to simulink model defined!"); 
	}
	char *simulinkModelName = malloc(strlenSimulinkModelName);
	if (bhmStringAttribute("simulinkModelName", simulinkModelName, strlenSimulinkModelName)) { 
		// bhmMessage("I", PREFIX, "The model Name is %s", simulinkModelName); 
	} else{
		bhmMessage("F", PREFIX, "No model Name to simulink model defined!"); 
	}
	if ((strcmp(simulinkModelName, "DUMMY")==0) && (strcmp(simulinkModelPath, "DUMMY")==0)) {
		// No Matlab/Simulink model has been defined. Cosimulation is disabled.
		// so no need to do anything. Else close opened Matlab session.
	} else {
		closeMatlab();
	}
}





//////////////////////////// Memory mapped registers ///////////////////////////

static void installRegisters(void) {

	// small helper function to convert integer values to strings
	char* itoa(int val, int base){	
		static char buf[32] = {0};
		int i = 30;
		do {
			buf[i--] = "0123456789abcdef"[val % base];
			} while( ( val /= base ) && i );
		return &buf[i+1];
	}

	char name[128];
	char *i2a;
	int i = 0;
	
	// First address on OffisSimLink bus interafce is the IRQ status register
	// whenever this register is not 0 OffisSimLink creates an IRQ on its output
	ppmCreateRegister(	"irq0VectAddr",						// name
						"irq0VectAddr",						// description
						handles.OFFIS_sl_sPort,				// WINDOW base
						0,									// port offset in bytes
						4,									// register size in bytes
						rcb_irqVectAddr,					// Read call back function
						wcb_irq0VectAddr,					// write call back function
						view32,								// debug view ????
						&(OFFIS_sl_sPort.irq0VectAddr),		// register in my data structure
						True
					);
	ppmCreateRegister(	"irq1VectAddr",						// name
						"irq1VectAddr",						// description
						handles.OFFIS_sl_sPort,				// WINDOW base
						4,									// port offset in bytes
						4,									// register size in bytes
						rcb_irqVectAddr,					// Read call back function
						wcb_irq1VectAddr,					// write call back function
						view32,								// debug view ????
						&(OFFIS_sl_sPort.irq1VectAddr),		// register in my data structure
						True
					);
	ppmCreateRegister(	"irq2VectAddr",						// name
						"irq2VectAddr",						// description
						handles.OFFIS_sl_sPort,				// WINDOW base
						8,									// port offset in bytes
						4,									// register size in bytes
						rcb_irqVectAddr,					// Read call back function
						wcb_irq2VectAddr,					// write call back function
						view32,								// debug view ????
						&(OFFIS_sl_sPort.irq2VectAddr),		// register in my data structure
						True
					);
	ppmCreateRegister(	"irq3VectAddr",						// name
						"irq3VectAddr",						// description
						handles.OFFIS_sl_sPort,				// WINDOW base
						12,									// port offset in bytes
						4,									// register size in bytes
						rcb_irqVectAddr,					// Read call back function
						wcb_irq3VectAddr,					// write call back function
						view32,								// debug view ????
						&(OFFIS_sl_sPort.irq3VectAddr),		// register in my data structure
						True
					);
	// depending on the defined number of Actuator-Sensor-Pairs that many regsiter are created.
	for (i = 0; i < NumberActuatorSensor; i++) {
		// defining the actuators
		i2a = itoa(i, 10);
		strcpy(name, "actuator");
		strcat(name, i2a);
		// printf(name); printf(" OFFSET:%x\n", (i*8));
		ppmCreateRegister(	name,								// name
							name,								// description
							handles.OFFIS_sl_sPort,				// WINDOW base
							((i*8)+16),							// port offset in bytes
							4,									// register size in bytes
							rcb_actuator,						// Read call back function
							wcb_actuator,						// write call back function
							view32,								// debug view ????
							&(OFFIS_sl_sPort.actuatorValue[i]),	// register in my data structure
							True
						);
		// defining the sensors
		strcpy(name, "sensor");
		strcat(name, i2a);
		// printf(name); printf(" OFFSET:%x,\n", (i*8)+4);
		ppmCreateRegister(	name,								// name
							name,								// description
							handles.OFFIS_sl_sPort,				// WINDOW base
							((i*8)+20),							// port offset in bytes
							4,									// register size in bytes
							rcb_sensor,							// Read call back function
							wcb_sensor,							// write call back function
							view32,								// debug view ????
							&(OFFIS_sl_sPort.sensorValue[i]),	// register in my data structure
							True
						);		
	}
}

////////////////////////////////// Constructor /////////////////////////////////

PPM_CONSTRUCTOR_CB(periphConstructor) {
	installSlavePorts();
	installRegisters();
	installNetPorts();

	Uns32 strlenSimulinkModelPath;
	if (bhmIntegerAttribute("strlenSimulinkModelPath", &strlenSimulinkModelPath)) {
		// bhmMessage("I", PREFIX, "The model path has #characters: %d", strlenSimulinkModelPath); 
	} else {
		bhmMessage("F", PREFIX, "Number characters for \"strlenSimulinkModelPath\" could not be read!"); 
	}
	Uns32 strlenSimulinkModelName;
	if (bhmIntegerAttribute("strlenSimulinkModelName", &strlenSimulinkModelName)) {
		// bhmMessage("I", PREFIX, "The model Name has #characters: %d", strlenSimulinkModelName); 
	} else {
		bhmMessage("F", PREFIX, "Number characters for \"strlenSimulinkModelName\" could not be read!"); 
	}
	char *simulinkModelPath = malloc(strlenSimulinkModelPath);
	if (bhmStringAttribute("simulinkModelPath", simulinkModelPath, strlenSimulinkModelPath)) { 
		// bhmMessage("I", PREFIX, "The model path is %s", simulinkModelPath); 
	} else{
		bhmMessage("F", PREFIX, "No model path to simulink model defined!"); 
	}
	char *simulinkModelName = malloc(strlenSimulinkModelName);
	if (bhmStringAttribute("simulinkModelName", simulinkModelName, strlenSimulinkModelName)) { 
		// bhmMessage("I", PREFIX, "The model Name is %s", simulinkModelName); 
	} else{
		bhmMessage("F", PREFIX, "No model Name to simulink model defined!"); 
	}
  	if ((strcmp(simulinkModelName, "DUMMY")==0) && (strcmp(simulinkModelPath, "DUMMY")==0)) {
		bhmMessage("W", PREFIX, "No Matlab/Simulink model has been defined. Cosimulation is disabled."); 
		bhmMessage("W", PREFIX, "Instead OffisSimLink peripheral will generate just some dummy values!");
	} else {
		// Matlab/Simulink is available. Go for it!
		startMatlab(strlenSimulinkModelPath, simulinkModelPath, strlenSimulinkModelName, simulinkModelName);
	}
}

////////////////////////////////// Create Threads////////////////////////////////
#define size (32*2048)

char stackA[size]; 

void myThread(void *user)
{
	// Read SyncInterval, ModelName and ModelPath from SimLink Peripheral Attribute List
	Uns32 simSyncInterval;
	if (bhmIntegerAttribute("simSyncInterval", &simSyncInterval)) {
		// bhmMessage("I", PREFIX, "The model Name has #characters: %d", strlenSimulinkModelName); 
	} else {
		bhmMessage("F", PREFIX, "\"simSyncInterval\" could not be read!"); 
	}
	Uns32 strlenSimulinkModelPath;
	if (bhmIntegerAttribute("strlenSimulinkModelPath", &strlenSimulinkModelPath)) {
		// bhmMessage("I", PREFIX, "The model path has #characters: %d", strlenSimulinkModelPath); 
	} else {
		bhmMessage("F", PREFIX, "Number characters for \"strlenSimulinkModelPath\" could not be read!"); 
	}
	Uns32 strlenSimulinkModelName;
	if (bhmIntegerAttribute("strlenSimulinkModelName", &strlenSimulinkModelName)) {
		// bhmMessage("I", PREFIX, "The model Name has #characters: %d", strlenSimulinkModelName); 
	} else {
		bhmMessage("F", PREFIX, "Number characters for \"strlenSimulinkModelName\" could not be read!"); 
	}
	char *simulinkModelPath = malloc(strlenSimulinkModelPath);
	if (bhmStringAttribute("simulinkModelPath", simulinkModelPath, strlenSimulinkModelPath)) { 
		// bhmMessage("I", PREFIX, "The model path is %s", simulinkModelPath); 
	} else{
		bhmMessage("F", PREFIX, "No model path to simulink model defined!"); 
	}
	char *simulinkModelName = malloc(strlenSimulinkModelName);
	if (bhmStringAttribute("simulinkModelName", simulinkModelName, strlenSimulinkModelName)) { 
		// bhmMessage("I", PREFIX, "The model Name is %s", simulinkModelName); 
	} else{
		bhmMessage("F", PREFIX, "No model Name to simulink model defined!"); 
	}
  
	if ((strcmp(simulinkModelName, "DUMMY")==0) && (strcmp(simulinkModelPath, "DUMMY")==0)) {
		// Starting endless loop with dummy generation inside
		// OffisSimLink is disabled!
		while(1) {
			syncMatlabDUMMY(&OFFIS_sl_sPort);
			bhmWaitDelay(simSyncInterval*1000);  // time to wait in micro seconds
		}
	} else {
		// Starting endless loop with intercepted function to communicate with matlab simulink.
		while(1) {
			syncMatlab((Uns32)&OFFIS_sl_sPort, strlenSimulinkModelName, simulinkModelName);
			if (OFFIS_sl_sPort.irq0VectAddr != 0) {
				// Interrupt from Simulink detected!
				ppmWriteNet(handles.IRQo0, 1);
			}
			if (OFFIS_sl_sPort.irq1VectAddr != 0) {
				// Interrupt from Simulink detected!
				ppmWriteNet(handles.IRQo1, 1);
			}
			if (OFFIS_sl_sPort.irq2VectAddr != 0) {
				// Interrupt from Simulink detected!
				ppmWriteNet(handles.IRQo2, 1);
			}
			if (OFFIS_sl_sPort.irq3VectAddr != 0) {
				// Interrupt from Simulink detected!
				ppmWriteNet(handles.IRQo3, 1);
			}
			bhmWaitDelay(simSyncInterval*1000);  // time to wait in micro seconds
		}
	}
}

void userInit(void)
{
  struct myThreadcontext { Uns32 myThreadData1; Uns32 myThreadData2; } contextA;
  bhmCreateThread(myThread, &contextA, "threadA", &stackA[size]);
}

///////////////////////////////////// Main /////////////////////////////////////

int main(int argc, char *argv[]) {
  constructor();
  userInit();
  bhmWaitEvent(bhmGetSystemEvent(BHM_SE_END_OF_SIMULATION));
  destructor();
  return 0;
}
