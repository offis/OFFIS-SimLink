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

 
// simLink.h is included from several .c in different contexts
// depending on the context only parts of the code below is included
// These contexts are
// compile for PSE processor (included by simLink.c)
// compile for DLL for Windwos (included by peripheral_semihost.c)
// compile into Virtual Platform (included by platform.c)

 
#ifndef SIMLINK_H
	#define SIMLINK_H

	// BE AWARE to also adapt the FreeRTOS application code to use the
	// extra Actuator/Sensor if you increase NumerActuatorSensor or to NOT USE
	// missing Actuator/Sensor in case you reduce the count!
	#define NumberActuatorSensor 100

	
	#ifndef INCLUDEFROMVP
	
		#ifndef SEMIHOST_DLL
			#include "peripheral/impTypes.h"
		#else
			#include "hostapi/impTypes.h"
		#endif

		// if included from simLink.c (PSE model) than include this
		#ifdef PSE
			#include "peripheral/bhm.h"
			#include "peripheral/ppm.h"
			#define PREFIX  "PERIPHERAL_SimLink"
		#endif

		#include <stdio.h>



/////////////////////////// Register data declaration and port handles//////////////////////////

		typedef struct OFFIS_sl_RegisterS { 
			Uns32 actuatorValue[NumberActuatorSensor];
			Uns32 sensorValue[NumberActuatorSensor];
			Uns32 irq0VectAddr;
			Uns32 irq1VectAddr;
			Uns32 irq2VectAddr;
			Uns32 irq3VectAddr;
		} OFFIS_sl_RegisterT, *OFFIS_sl_RegisterPT;

		typedef struct handlesS {
			void *OFFIS_sl_sPort;         // OFFIS simLink handle to bus slave port interface			
			#ifdef PSE
				ppmNetHandle  IRQo0;
				ppmNetHandle  IRQo1;
				ppmNetHandle  IRQo2;
				ppmNetHandle  IRQo3;
			#endif
		} handles_type, *handles_pointerType;
	#endif
#endif
