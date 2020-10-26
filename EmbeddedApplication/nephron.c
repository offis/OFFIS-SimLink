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
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "nephron.h"

void taskMessage(const char * severity, const char * pref, const char * message, ...)
{
	#ifdef VP_SIMULATION
		// during simulation we want to make "printf"s on simulation trace window
		va_list vargzeiger;
		va_start(vargzeiger, message);
		if (strcmp(severity, "I") == 0) {
			printf("Info ");
		} else if (strcmp(severity, "W") == 0) {
			printf("Warning ");
		} else if (strcmp(severity, "E") == 0) {
			printf("Error ");
		} else if (strcmp(severity, "F") == 0) {
			printf("Failure ");
		} else {
			printf("N.D. ");
		}
		printf("(%s) ", pref);
		vprintf(message, vargzeiger);
		printf("\n");
		va_end(vargzeiger);
	#else
		// Simple printf will not work on real HW. With VP we can just printf to cosole.
		// If we want to give a message with real HW we have to implement an according
		// functionality here. Remove warning once implementation is done.
		#warning(Makro "VP" not defined. Compiling for Nephron target actual HW. Target implementation missing here!)
	#endif
}
