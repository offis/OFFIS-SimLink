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

// VMI includes 
#include "vmi/vmiMessage.h" 
#include "vmi/vmiOSAttrs.h" 
#include "vmi/vmiOSLib.h" 
#include "vmi/vmiRt.h" 
#include "vmi/vmiTypes.h" 
#include "vmi/vmiVersion.h"  

#include <stdio.h>
#include <string.h>
#include <windows.h>
 
// Mathwork's Matlab engine interface API
#include "engine.h"

// Struct which contains all variables/data that is available inside all dll defined
// functions. 
typedef struct vmiosObjectS  {
	vmiRegInfoCP result;
	vmiRegInfoCP sp;
	memDomainP   pseDomain;
	// pointer to matlab session. Session will be started in dll constructor
	Engine *ep;
} vmiosObject;

// helper function to read function parameters from stack
void getArg(vmiProcessorP processor, vmiosObjectP object, Uns32 index, void *result);

// helper function for matlab communication
void sendCmd(vmiosObjectP object, char *cmd);
	
// helper function for matlab communication
bool simstatispaused(vmiosObjectP object);

bool simstatisstopped(vmiosObjectP object);

// helper function for matlab communication    
Uns32 getVariable(vmiosObjectP object, const char *buf);
	
// helper function for matlab communication
// This here leaves room for improvement. Actuators are implemented by changing the
// value (parameter) of a Simulink constant. This constant has to be already defined in
// the Simulink model and here it is necessary that it is included in the path of the
// submodel "uController".
void putConstant(vmiosObjectP object, const char *model, const char *actuator, Uns32 value);

void putVariable(vmiosObjectP object, const char *variable, Uns32 value);
