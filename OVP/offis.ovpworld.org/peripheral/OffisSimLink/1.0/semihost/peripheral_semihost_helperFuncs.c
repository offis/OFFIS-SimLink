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

#include "peripheral_semihost_helperFuncs.h" 
 
#define PREFIX  "PERIPHERAL_SimLink_DLL_HelperFunc"
 
// helper function to read function parameters from stack
void getArg(vmiProcessorP processor, vmiosObjectP object, Uns32 index, void *result)
{
	Uns32 argSize   = 4;
	Uns32 argOffset = (index+1)*argSize;
	Uns32 spAddr;
  
	// get the stack
	vmiosRegRead(processor, object->sp, &spAddr);
  
	// read argument value
	vmirtReadNByteDomain(object->pseDomain, spAddr+argOffset, result, argSize, 0, False);
} 
 
// helper function for matlab communication
// the string variable "cmd" contains the matlab shell command to be executed within the matlab workspace.
void sendCmd(vmiosObjectP object, char *cmd)
{
	int status = engEvalString(object->ep, cmd);
	if (status != 0) {
		vmiMessage("F", PREFIX, "not able to communicate with Matlab/Simulink environment!\n");
	} 
}

// helper function for matlab communication
bool simstatispaused(vmiosObjectP object)
{
	// Let us see if Simulink is waiting for us. Status has to be "paused")
	sendCmd(object, "simstat = strcmp(get_param(bdroot,'SimulationStatus'), 'paused')");      
	mxArray *matsimstat = engGetVariable(object->ep, "simstat");
	if (matsimstat == NULL) {
		vmiMessage("F", PREFIX, "Was not able to receive variable 'simstat' from matlab workspace!");
	}
	bool simstat = mxGetScalar(matsimstat);
	mxDestroyArray(matsimstat);
	return (simstat);
}

// helper function for matlab communication
bool simstatisstopped(vmiosObjectP object)
{
	// Let us see if Simulink was started
	sendCmd(object, "simstat = strcmp(get_param(bdroot,'SimulationStatus'), 'stopped')");      
	mxArray *matsimstat = engGetVariable(object->ep, "simstat");
	if (matsimstat == NULL) {
		vmiMessage("F", PREFIX, "Was not able to receive variable 'simstat' from matlab workspace!");
	}
	bool simstat = mxGetScalar(matsimstat);
	mxDestroyArray(matsimstat);
	return (simstat);
}
	
// helper function for matlab communication    
Uns32 getVariable(vmiosObjectP object, const char *buf)
{
	// String that builds the commands send to matlab
	char cmd[1024];

	mxArray *matlvar = engGetVariable(object->ep, buf);
	if (matlvar == NULL) {
		vmiMessage("W", PREFIX, "Variable %s not defined in matlab workspace! Sending cmd %s=0", buf, buf);
		strcpy(cmd, buf);
		strcat(cmd, "=0");
		sendCmd(object, cmd);
		return(0);
	}
	Uns32 tmp = mxGetScalar(matlvar);
	mxDestroyArray(matlvar);
	return(tmp);
}
	
// helper function for matlab communication
// This here leaves room for improvement. Actuators are implemented by changing the
// value (parameter) of a Simulink constant. This constant has to be already defined in
// the Simulink model and here it is necessary that it is included in the path of the
// submodel "uController".
void putConstant(vmiosObjectP object, const char *model, const char *actuator, Uns32 value)
{
	// String that builds the commands send to matlab
	char cmd[1024];

	char tmp[1024];
	itoa(value, tmp, 10);
	strcpy(cmd, "set_param('");
	strcat(cmd, model);
	strcat(cmd, "/uController/");
	strcat(cmd, actuator);
	strcat(cmd, "', 'Value', '");
	strcat(cmd, tmp);
	strcat(cmd, "')");
	sendCmd(object, cmd);
}
void putVariable(vmiosObjectP object, const char *variable, Uns32 value)
{
	// String that builds the commands send to matlab
	char cmd[1024];

	char tmp[1024];
	itoa(value, tmp, 10);
	
	strcpy(cmd, variable);
	strcat(cmd, "=");
	strcat(cmd, tmp);
	sendCmd(object, cmd);
}

