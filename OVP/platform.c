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

// standard string array length
#define SLEN 1024
// PREFIX being used for all messages of main platform code
#define PREFIX  "OFFIS HW Platform"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <Windows.h>

#include "icm/icmCpuManager.h"

#define INCLUDEFROMVP

// Helper functions used by platform to get simulation configuration from environment variables
#include "envIO.h"



int main(int argc, char **argv, char *envp[]) {


	Bool   promptAtExit = False;
	char   anyKey[64];

	////////////////////////////////////////////////////////////////////////////
	// Get application .elf file and other simulation configuration infos.
	// Most practical way is to use environtment variables.
	// The following function come from "envIO.h"
	////////////////////////////////////////////////////////////////////////////
  
	char   appName[SLEN]; getPathToApplicationToRunOnPlatform(appName, SLEN);
	int    simend    = 1; //getSimulationEndTime();
	int    simstep   = 1; //getSimulationStepFeedback(simend);
	int    simdetail = 0; //getSimulationDebugDetail();
	int    simdebug  = 0; //getSimulationDebug();

	// int    cosimEnabled = 1;      //getSimulinkModel() will set to 1 if it is successfull
	char   simulinkModel[SLEN]="../MatlabSimulink/mySimulink.mdl"; // getSimulinkModel(simulinkModel, SLEN, &cosimEnabled);
	char   simulinkModelName[SLEN]; 
	char   simulinkModelPath[SLEN];
	getSimulinkModelName(simulinkModelName, SLEN, simulinkModel);
	getSimulinkModelPath(simulinkModelPath, SLEN, simulinkModel);
	int    simSyncInterval = 1; // every milisec sync

	#define NumberActuatorSensor 100
  
	// initialize OVPsim, enabling verbose mode to get statistics at end of execution
	if (simdebug == 1) {
		icmInit(True, "localhost", 12345);
	} else {
		icmInit(ICM_VERBOSE|ICM_STOP_ON_CTRLC, 0, 0);
	}
  


	////////////////////////////////////////////////////////////////////////////
	// Memory Mapping
	// Vector IRQ Controler reaches from 0xffff f000 to 0xffff f23f
	// SimLink              reaches from 0x8001 0000 to depends on Numer A/S pirs
	// TC                   reaches from 0x8000 0000 to 0x8000 3fff
	// On Chip Static Ram   reaches from 0x4000 0000 to 0x4000 ffff ( 64K)
	// On Chip Non Volatile reaches from 0x0000 0000 to 0x0001 ffff (128K)
	////////////////////////////////////////////////////////////////////////////

	// BE CAREFULL: The address range of SimLink depends on the defined preprocessor
	// variable NumberActuatorSensor! The value of NumberActuatorSensor defines the number of
	// registers to be used to link to Simulink. e.g. NumberActuatorSensor 10 means 10 Sensor regs
	// plus 10 Actuator regs = 20 regs (each reg 4 Bytes!).
  
	// create the processor bus
	icmBusP bus = icmNewBus("busMain", 32);

	// create the two memory regions for Flash and Static Ram and connect to bus
	icmMemoryP OnChipNonVolatile = icmNewMemory("OnChipNonVolatile", ICM_PRIV_RX,  0x0001ffff);
	icmMemoryP OnChipStaticRam   = icmNewMemory("OnChipStaticRam",   ICM_PRIV_RWX, 0x0000ffff);
	icmConnectMemoryToBus(bus, "sp", OnChipNonVolatile, 0x00000000);
	icmConnectMemoryToBus(bus, "sp", OnChipStaticRam,   0x40000000);




	////////////////////////////////////////////////////////////////////////////
	// create nets and net monitors
	////////////////////////////////////////////////////////////////////////////

	// icmNetP irq_tc2vic		= icmNewNet("irq_tc2vic");
	icmNetP irq_simRTB2vic	= icmNewNet("irq_simRTB2vic");
	icmNetP irq_simMB2vic	= icmNewNet("irq_simMB2vic");
	icmNetP irq_simCB2vic	= icmNewNet("irq_simCB2vic");
	icmNetP irq_simPB2vic	= icmNewNet("irq_simPB2vic");
	// icmNetP irq_vic2arm		= icmNewNet("irq_vic2arm");

	NET_WRITE_FN(intNetWritten) { 
		icmPrintf("Event on net!\n"); 
	}

	// icmAddNetCallback(irq_simMB2vic, intNetWritten, NULL);


	////////////////////////////////////////////////////////////////////////////
	// create a processor instance
	////////////////////////////////////////////////////////////////////////////
	
	
	
	const char *vlnvRoot     = NULL; //When NULL use default library 
	const char *arm7Model    = icmGetVlnvString(vlnvRoot,		\
												"arm.ovpworld.org",	\
												"processor",		\
												"arm",			\
												"1.0",			\
												"model");
	const char *arm7Semihost = icmGetVlnvString(vlnvRoot,			\
												"arm.ovpworld.org",	\
												"semihosting",		\
												"armNewlib",		\
												"1.0",			\
												"model");
	icmAttrListP icmAttr = icmNewAttrList();
	icmAddStringAttr(icmAttr, "variant", "ARM7TDMI");
	icmAddStringAttr(icmAttr, "compatibility", "ISA");
	icmAddStringAttr(icmAttr, "showHiddenRegs", "0");
	icmAddStringAttr(icmAttr, "UAL", "0");
	icmAddStringAttr(icmAttr, "endian", "little");

	// enable relaxed scheduling for maximum performance
	// #define SIM_ATTRS (ICM_ATTR_RELAXED_SCHED)
	// #define SIM_ATTRS (ICM_ATTR_TRACE)
	// #define SIM_ATTRS (ICM_ATTR_TRACE_REGS_AFTER)

	int SIM_ATTRS = 0;
	switch (simdetail) {
		case 1:
			SIM_ATTRS = ICM_ATTR_TRACE;
			break;
		case 2:
			SIM_ATTRS = ICM_ATTR_TRACE_ICOUNT;
			break;
		case 3:
			SIM_ATTRS = ICM_ATTR_TRACE_REGS_BEFORE;
			break;
		case 4:
			SIM_ATTRS = ICM_ATTR_TRACE_BUFFER;
			break;
		default:
			SIM_ATTRS = ICM_ATTR_DEFAULT;      
			break;
    }

	icmProcessorP processor = icmNewProcessor(
					    "CPU1",             // CPU name
					    "arm",              // CPU type
					    0,                  // CPU cpuId
					    0,                  // CPU model flags
					    32,                 // address bits
					    arm7Model   ,       // model file
					    "modelAttrs",       // morpher attributes
					    SIM_ATTRS,          // attributes
					    icmAttr,            // user-defined attributes
					    arm7Semihost,       // semi-hosting file
					    "modelAttrs"        // semi-hosting attributes
					    );
  
	// connect the processor
	icmConnectProcessorBusses(processor, bus, bus);
	// icmConnectProcessorNet(processor, irq_vic2arm, "irq", ICM_INPUT);

	// load the application executable file into processor memory space
	if(!icmLoadProcessorMemory(processor, "../EmbeddedApplication/OffisSimLinkDemo.elf", True, False, True)) {
		return -1;
	}

	////////////////////////////////////////////////////////////////////////////
	// OFFIS SimLink
	// for co-simulation of OVP with matlab/simulink
	////////////////////////////////////////////////////////////////////////////  
 
	icmAttrListP icmSimLinkAttr = icmNewAttrList();
    icmAddUns64Attr(icmSimLinkAttr, "strlenSimulinkModelPath", strlen(simulinkModelPath)+1 );
    icmAddStringAttr(icmSimLinkAttr, "simulinkModelPath", simulinkModelPath);
    icmAddUns64Attr(icmSimLinkAttr, "strlenSimulinkModelName", strlen(simulinkModelName)+1 );
    icmAddStringAttr(icmSimLinkAttr, "simulinkModelName", simulinkModelName);
    icmAddUns64Attr(icmSimLinkAttr, "simSyncInterval", simSyncInterval);
    
    const char *path_OFFIS_simLink = icmGetVlnvString(".",	
						      "offis.ovpworld.org", 
						      "peripheral",	
						      "OffisSimLink",	
						      "1.0", "pse");
	icmPseP OFFIS_simLink;
	OFFIS_simLink = icmNewPSE("OFFIS_simLink",	
					path_OFFIS_simLink,	
					icmSimLinkAttr,                   
					"offis.ovpworld.org\\peripheral\\OffisSimLink\\1.0\\semihost\\model.dll", 
					"modelAttrs");


    // WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING //
    //
    // for the function "icmConnectPSEBus" make sure not to use "any" name for the port
    // but the one as defined in the PSE model. If we name it different then "sp"
    // we wont see a direct error message, but reading/writing from the address space 
    // throws a processor exception without any further hint what went wrong!
    // icmConnectPSEBus(OFFIS_simLink, bus, "sp", False, 0x80010000, 0x8001004f);
	//printf("Range: %x\n", 0x80010000+(2*NumberActuatorSensor*4)-1);
    icmConnectPSEBus(OFFIS_simLink, bus, "sp", False, 0x80010000, (0x80010000+(2*NumberActuatorSensor*4)-1+(4*4))); //+4*4 because of 4 IRQ-address
	icmConnectPSENet(OFFIS_simLink, irq_simRTB2vic,	"IRQo0", ICM_OUTPUT); // IRQ from real Time Board
	icmConnectPSENet(OFFIS_simLink, irq_simMB2vic,	"IRQo1", ICM_OUTPUT); // IRQ from Main Board e.g. User Interface
	icmConnectPSENet(OFFIS_simLink, irq_simCB2vic,	"IRQo2", ICM_OUTPUT); // IRQ from Communication Board
	icmConnectPSENet(OFFIS_simLink, irq_simPB2vic,	"IRQo3", ICM_OUTPUT); // IRQ from Power Board








  ////////////////////////////////////////////////////////////////////////////
  // Simulation of Platform
  ////////////////////////////////////////////////////////////////////////////
  icmMessage("I", PREFIX, "##############################################################");
  icmMessage("I", PREFIX, "#                                                            #");
  icmMessage("I", PREFIX, "#             ######  #####  #####  ####  #####              #");
  icmMessage("I", PREFIX, "#             ##  ##  ##     ##      ##   ###                #");
  icmMessage("I", PREFIX, "#             ##  ##  ####   ####    ##     ###              #");
  icmMessage("I", PREFIX, "#             ######  ##     ##     ####  #####              #");
  icmMessage("I", PREFIX, "#                                                            #");
  icmMessage("I", PREFIX, "#  Dipl.-Inform. Frank Poppen                                #");
  icmMessage("I", PREFIX, "#  Escherweg 2                                               #");
  icmMessage("I", PREFIX, "#  26121 Oldenburg                                           #");
  icmMessage("I", PREFIX, "#  Germany                                                   #");
  icmMessage("I", PREFIX, "#                                                            #");
  icmMessage("I", PREFIX, "# Nephron+ mainboard simulation using Open Virtual Platform. #");
  icmMessage("I", PREFIX, "#                                                            #");
  icmMessage("I", PREFIX, "# Starting HW Platform simulation.                           #");
  icmMessage("I", PREFIX, "#                                                            #");
  icmMessage("I", PREFIX, "##############################################################");


  icmProcessorP final;
  
  double  i;
  
  for (i=simstep; i<=simend; i=i+simstep){
    icmSetSimulationStopTime(i);
    final = icmSimulatePlatform();
    icmMessage("I", PREFIX, "OFFIS HW Platform: Simulated %f seconds!", i);
	// icmWriteNet(irq_open2vic, 1); 
	// icmWriteNet(irq_open2vic, 0); 
  }

  // was simulation interrupted or did it complete
  if(final && (icmGetStopReason(final)==ICM_SR_INTERRUPT)) {
    icmMessage("W", PREFIX,"OFFIS HW Platform: *** simulation interrupted!");
  }
  
  // free the processor
  icmFreeProcessor(processor);
  
  // wait for key press before terminating
  if(promptAtExit) {
    icmPrintf("OFFIS HW Platform: Press enter to exit demo > ");
    fgets(anyKey, sizeof(anyKey), stdin);
  }
  
  return 0;
}
