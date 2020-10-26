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
#define SEMIHOST_DLL
#include "../offisSimLink.h"
#include "peripheral_semihost_helperFuncs.h"

// prefix to be used during printf messages
#define PREFIX  "PERIPHERAL_SimLink_DLL"

int simulinkTime    = 0;
int simulinkTimeOld = 0;

static VMIOS_INTERCEPT_FN(startMatlab)
{
	// vp_: pointer in virtual address space of simulation. Need to copy from
	//      there to here on real host (DLL) address space.
	Uns32 vp_simulinkModelPath;
	Uns32 vp_simulinkModelName;
	Uns32 strlenSimulinkModelPath;
	Uns32 strlenSimulinkModelName;

	getArg(processor, object, 0, &strlenSimulinkModelPath);
	getArg(processor, object, 1, &vp_simulinkModelPath);
	getArg(processor, object, 2, &strlenSimulinkModelName);
	getArg(processor, object, 3, &vp_simulinkModelName);

	char * simulinkModelPath = malloc(strlenSimulinkModelPath);
	char * simulinkModelName = malloc(strlenSimulinkModelName);
	vmirtReadNByteDomain(object->pseDomain, vp_simulinkModelPath, simulinkModelPath, strlenSimulinkModelPath, 0, False);
	vmirtReadNByteDomain(object->pseDomain, vp_simulinkModelName, simulinkModelName, strlenSimulinkModelName, 0, False);

	// Starting Matlab
	vmiMessage("I", PREFIX, "Starting Matlab/Simulink session!");
	if(!(object->ep=engOpen("\0"))) {
		vmiMessage("F", PREFIX, "not able to start Matlab/Simulink environment!");
		vmiMessage("F", PREFIX, "If you are running Matlab/Simulink on a Windows system, you are able to");
		vmiMessage("F", PREFIX, "start Matlab manually, and this function call still fails:");
		vmiMessage("F", PREFIX, "Have a look at the Matlab user guide for function 'engOpen'.");
		vmiMessage("F", PREFIX, "There you will find that you have to execute the command 'matlab /regserver' once.");
		vmiMessage("F", PREFIX, "make sure to do this having system administrator rights!");
	}
	
	// Changing Matlab working directory to where simulink model is
	char cmd[1024];
	strcpy(cmd, "cd ");
	strcat(cmd, simulinkModelPath);
	sendCmd(object, cmd);
	sendCmd(object, "pwd");
	mxArray *matlarray = engGetVariable(object->ep, "ans");
	if (!mxIsChar(matlarray) || (mxGetM(matlarray) != 1)) {
		vmiMessage("F", PREFIX, "Matlab/Simulink environment did not return string value!");
	}
	mwSize buflen = mxGetN(matlarray)*sizeof(mxChar)+1;
	char *buf = (char*)mxMalloc(buflen);
	if (mxGetString(matlarray, buf, buflen) != 0) {
		vmiMessage("F", PREFIX, "could not convert Matlab/Simulink string!");
	} else {
		vmiMessage("I", PREFIX, "Matlab session started in path: %s", buf);
	}
	// execute startup.m to initialize simulation environment, like search paths etc.
	sendCmd(object, "startup");

	//opening simulink model
	vmiMessage("I", PREFIX, "Opening simulink model.");
	strcpy(cmd, "open('");
	strcat(cmd, simulinkModelName);
	strcat(cmd, "')");
	sendCmd(object, cmd);
	vmiMessage("I", PREFIX, "Successfully opened simulink model.");
}

static VMIOS_INTERCEPT_FN(syncMatlab)
{  
	// Retrieve values from OVP-Platform
	Uns32 p_sPort;
	OFFIS_sl_RegisterT OFFIS_sl_sPort;

	// Get the first argument from stack of virtual x86 (PSE)
	// which contains the pointer in the virtual processor memory space
	// to the struct to be synced with matlab
	// Then copy the struc from the virtual memory space into the
	// physical domain of this host dll
	getArg(processor, object, 0, &p_sPort);
	vmirtReadNByteDomain(object->pseDomain, p_sPort, (void *)&OFFIS_sl_sPort, sizeof(OFFIS_sl_sPort), 0, False);

	// The next two arguments are length of model name and pointer
	// to where the string can be found.
	Uns32 vp_simulinkModelName;
	Uns32 strlenSimulinkModelName;
	getArg(processor, object, 1, &strlenSimulinkModelName);
	getArg(processor, object, 2, &vp_simulinkModelName);
	char * simulinkModelName = malloc(strlenSimulinkModelName);
	vmirtReadNByteDomain(object->pseDomain, vp_simulinkModelName, simulinkModelName, strlenSimulinkModelName, 0, False);

	if (simstatisstopped(object)) {
		// Simulation was just initialised. Wall clock time is "0 seconds".
		// Do nothing, just proceed with the initial first sync at time 0.
		// NOTE!!!! There are two other cases when this state can be reached.
		// 1.) The user manually stopped Simulink.
		// 2.) Simulink is not configured to run infinite simulation time and reached end
		simulinkTime = 0;
		vmiMessage("I", PREFIX, "Initial Sync of OVP and Simulink.");   
	} else if (simstatispaused(object)) {
		// Simulation did run one sync step and is now waiting for OVP. 
		// Wall clock time is pased "0 seconds" (multiple of sync period)
		simulinkTime = getVariable(object, "time")+1;	//Simulink stops just before updates the time variable.
												// So even though wall clock time is +1 the matlab
												// Variable is not yet updated to see that.
		while (simulinkTimeOld == simulinkTime) {
			// OVP simulation was soooooo fast, that Simulink did not have the time
			// to even change status from previous paused to running. Will have to
			// wait for the lagging Simulink in this case.
			Sleep(100); // wait for 0.1 seconds
		}
		simulinkTimeOld = simulinkTime;
		// vmiMessage("I", PREFIX, "%d. Sync OVP with Simulink", simulinkTime);    
	} else {
		// Simulation is neither stopped nor paused. So we assume that it is
		// still running to finish its computation for its sync step. So we have to
		// wait for it to finish.
		int nLoops = 0;
		while(!simstatispaused(object)) {   
			Sleep(100); // wait for 0.1 seconds
			nLoops++;
		}
		vmiMessage("I", PREFIX, "Waited %f seconds for Simulink to catch up.", (0.1*nLoops) ); 
	}

	// Read the irq status register. If the value is not 0, an IRQ is issued. Clear value in Simulink environment after read
	OFFIS_sl_sPort.irq0VectAddr = getVariable(object, "irq0VectAddr");
	OFFIS_sl_sPort.irq1VectAddr = getVariable(object, "irq1VectAddr");
	OFFIS_sl_sPort.irq2VectAddr = getVariable(object, "irq2VectAddr");
	OFFIS_sl_sPort.irq3VectAddr = getVariable(object, "irq3VectAddr");
	sendCmd(object, "irq0VectAddr=0");
	sendCmd(object, "irq1VectAddr=0");
	sendCmd(object, "irq2VectAddr=0");
	sendCmd(object, "irq3VectAddr=0");
	
	int i = 0;
	for (i = 0; i < NumberActuatorSensor; i++) {
		char i2a[64], name[64];
		itoa(i, i2a, 10);
		// Retrieve the sensor values from Matlab/Simulink
		strcpy(name, "Sensor");
		strcat(name, i2a);
		OFFIS_sl_sPort.sensorValue[i] = getVariable(object, name);
		// Send Actuator values. 
		strcpy(name, "Actuator");
		strcat(name, i2a);
		putVariable(object, name, OFFIS_sl_sPort.actuatorValue[i]);
	}
	
	// Send values to OVP-platform
	// Before at the beginning of this functions we copied the struc to be synced from virtual platfrom PSE processor
	// into the memory space of this Windows DLL. Just now we finished syncing these values with Simulink. So we now
	// have to write the struc back to PSE processor.
	vmirtWriteNByteDomain(object->pseDomain, p_sPort, (void *)&OFFIS_sl_sPort, sizeof(OFFIS_sl_sPort), 0, False);

	// Sync done - continue simulation
	if (simstatisstopped(object)) {
		// Start freshly initialised simulation
		sendCmd(object, "set_param(bdroot,'SimulationCommand','start')");
	} else {
		// Simulation must be paused. Now continue Matlab/Simulink simulation next step.
		sendCmd(object, "set_param(bdroot,'SimulationCommand','continue')");
	}
}

// Usually we would not need this function. The code could be copy pasted into
// the destructur below. But for unknown reasons the code hangs if done so.
// I strongly suspect an error in the OVP/VMI interface.
static VMIOS_INTERCEPT_FN(closeMatlab)
{
	vmiMessage("I", PREFIX, "Clossing Matlab session!\n");
	int status = engClose(object->ep);
	if (status != 0) {
		vmiMessage("E", PREFIX, "Not able to close Matlab/Simulink environment! Already closed?\n");
	} 
}

// Constructor
static VMIOS_CONSTRUCTOR_FN(constructor)
{
	object->result       = vmiosGetRegDesc(processor, "eax");
	object->sp           = vmiosGetRegDesc(processor, "esp");
	object->pseDomain    = vmirtGetProcessorDataDomain(processor);
}

// Destructor
static VMIOS_DESTRUCTOR_FN(destructor)
{
}

vmiosAttr modelAttrs = { 

	////////////////////////////////////////////////////////////////////////
	// VERSION
	////////////////////////////////////////////////////////////////////////

	VMI_VERSION,                // version string (THIS MUST BE FIRST)
	VMI_INTERCEPT_LIBRARY,      // type
	"semiHostPeripheral",       // description 
	sizeof(vmiosObject),        // size in bytes of object 

	////////////////////////////////////////////////////////////////////////
	// CONSTRUCTOR/DESTRUCTOR ROUTINES
	////////////////////////////////////////////////////////////////////////
  
	constructor,                // object constructor 
	destructor,                 // object destructor 

	////////////////////////////////////////////////////////////////////////
	// INSTRUCTION INTERCEPT ROUTINES
	////////////////////////////////////////////////////////////////////////

	0,                          // morph callback 
	0,                          // get next instruction address
	0,                          // disassemble instruction 
  
	// ADDRESS INTERCEPT DEFINITIONS 
	// -------------------   ----------- ------ ----------------- 
	// Name                  Address     Opaque Callback 
	// -------------------   ----------- ------ ----------------- 
	{
		{"startMatlab", 0, True, startMatlab},
		{"closeMatlab", 0, True, closeMatlab},
		{"syncMatlab",  0, True, syncMatlab},
		{ 0 }
	}
}; 
