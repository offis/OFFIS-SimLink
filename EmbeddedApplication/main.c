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

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*-----------------------------------------------------------*/

#define SIMLINKBASE 0x80010000
// Even though defined here, the addresses might not be
// available! This depends on the preprocessor variable
// "NumberActuatorSensor" defined during VP compilation.
// e.g. if this variable is 10 only ACTUATOR0-9 and SENSOR0-9 will be
// accessible.

#define IRQ0VECTADDR SIMLINKBASE
#define IRQ1VECTADDR (SIMLINKBASE+4)
#define IRQ2VECTADDR (SIMLINKBASE+8)
#define IRQ3VECTADDR (SIMLINKBASE+12)

#define ACTUATOR(n) (SIMLINKBASE + (8*(n)+16) )
#define SENSOR(n)   (SIMLINKBASE + (8*(n)+20) )

#define REG32 (volatile unsigned int*)
  
int main( void )
{
	printf("##############################################################\n");
	printf("#                                                            #\n");
	printf("#             ######  #####  #####  ####  #####              #\n");
	printf("#             ##  ##  ##     ##      ##   ###                #\n");
	printf("#             ##  ##  ####   ####    ##     ###              #\n");
	printf("#             ######  ##     ##     ####  #####              #\n");
	printf("#                                                            #\n");
	printf("#  Dipl.-Inform. Frank Poppen                                #\n");
	printf("#  Escherweg 2                                               #\n");
	printf("#  26121 Oldenburg                                           #\n");
	printf("#  Germany                                                   #\n");
	printf("#                                                            #\n");
	printf("# Demonstrating OFFIS interface for OVP to Matlab/Simulink   #\n");
	printf("#                                                            #\n");
	printf("##############################################################\n");
	
	int i = 0, j = 0;
	int s0 = 0, s1 = 0, s2 = 0, s3 = 0;
	int polling = 0;
	
	for(i=0;i<1000;i++)
	{
		*(REG32 ACTUATOR(0)) = i;
		*(REG32 ACTUATOR(1)) = i+1;
		printf("A0 = %d\n", i);
		printf("A1 = %d\n", i+1);
		while(polling == s2)
		// would be nicer of course to use IRQ of OffisSimLink
		// But we want to keep this demo simple!
		{
			// Busy wait!
			s0 = ((int)(*(REG32 SENSOR(0))));
			s1 = ((int)(*(REG32 SENSOR(1))));
			s2 = ((int)(*(REG32 SENSOR(2))));
			s3 = ((int)(*(REG32 SENSOR(3))));
		}
		polling = s2;
		printf("S0 = %d\n", s0);
		printf("S1 = %d\n", s1);
		printf("S2 = %d\n", s2);
		printf("S3 = %d\n", s3);
	}
	
	return(0);
}
