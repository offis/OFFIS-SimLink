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

#ifndef ENVIO_H
#define ENVIO_H

void getPathToApplicationToRunOnPlatform(char * buffer, const int length)
// copies string of environment variable $OFFIS_ELF into buffer if $OFFIS_ELF is defined
// else the constant string "../FreeRTOSV6.1.0/rtosdemo.elf" will be copyed instead.
// If in any case the size of the buffer is not large enough, a fatal message is issued.
{
  const char * const defElf = "../FreeRTOSV6.1.0/rtosdemo.elf";
  if (getenv("OFFIS_ELF") == 0) {
    if (length > strlen(defElf)) {
      strcpy(buffer, defElf);
      icmMessage("W", PREFIX, "$OFFIS_ELF is not defined. Defaulting to %s", buffer);
    } else {
      icmMessage("F", PREFIX, "getPathToApplicationToRunOnPlatform(char * buffer, uint length): buffer too small!");
    }
  } else {
    if (length > strlen(getenv("OFFIS_ELF"))) {
      strcpy(buffer, getenv("OFFIS_ELF"));
      icmMessage("I", PREFIX, "Reading $OFFIS_ELF: %s", buffer);
    } else {
      icmMessage("F", PREFIX, "getPathToApplicationToRunOnPlatform(char * buffer, uint length): buffer too small!");
    }
  }  
}

// if $OFFIS_SIMEND is defined, function converts string of environment variable
// $OFFIS_SIMEND to integer and returns value that resembles the simulation duration in
// seconds (simulated wall clock time). If not defined it will return a default value
// of 5 minutes which are 300 seconds.
int getSimulationEndTime()
{
  if (getenv("OFFIS_SIMEND") == 0) {
    icmMessage("W", PREFIX, "$OFFIS_SIMEND is not defined. Defaulting to 5 minutes wall clock time.");
    return(300);
  } else {
    int simend = atoi(getenv("OFFIS_SIMEND"));
    icmMessage("I", PREFIX, "Reading $OFFIS_SIMEND: %i seconds.", simend);
    return(simend);
  }
}

// if $OFFIS_SIMSTEP is de fined, function converts string of environment variable
// to integer and returns the value, if it is not larger than the targeted overall
// wall clock simulation time defined by the parameter "simend". If it is larger 
// the value of "simend" will be returned. if $OFFIS_SIMSTEP is undefined it will do
// the same (return "simend").
int getSimulationStepFeedback(const int simend)
{
  if (getenv("OFFIS_SIMSTEP") == 0) {
    icmMessage("I", PREFIX, "$OFFIS_SIMSTEP is not defined. Turning off wall clock time proceed information.");
    return(simend);
  } else {
    int simstep = atoi(getenv("OFFIS_SIMSTEP"));
    if (simstep > simend) {
      icmMessage("W", PREFIX, "$OFFIS_SIMSTEP larger $OFFIS_SIMEND not meaningfull. Setting $OFFIS_SIMSTEP=$OFFIS_SIMEND=%i seconds.", simend);
      return(simend);
    } else {
      icmMessage("I", PREFIX, "Reading $OFFIS_SIMSTEP: %i seconds.", simstep);
      return(simstep);
    }
  }
}

// function reads the environment variable $OFFIS_SIMDETAIL if defined and returns
// integer converted value. If not defined, it will return 0.
int getSimulationDebugDetail()
{
  if (getenv("OFFIS_SIMDETAIL") == 0) {
    icmMessage("I", PREFIX, "$OFFIS_SIMDETAIL is not defined. Defaulting to debug information level 0.");
    return(0);
  } else {
    int simdetail = atoi(getenv("OFFIS_SIMDETAIL"));
    icmMessage("I", PREFIX, "Reading $OFFIS_SIMDETAIL: %i", simdetail);
    if (simdetail != 0) {
      icmMessage("W", PREFIX, "Change in simdetail value not implemented yet. Behavior mostly the same as with '0'.");
    }
    return(simdetail);
  }
}

// function reads the environment variable $OFFIS_SIMDETAIL if defined and returns
// integer converted value. If not defined, it will return 0.
int getSimulationDebug()
{
  if (getenv("OFFIS_SIMDEBUG") == 0) {
    icmMessage("I", PREFIX, "$OFFIS_SIMDEBUG is not defined. Defaulting to debugging turned off.");
    return(0);
  } else {
    int simdebug = atoi(getenv("OFFIS_SIMDEBUG"));
    icmMessage("I", PREFIX, "Reading $OFFIS_SIMDEBUG: %i", simdebug);
    return(simdebug);
  }
}

// function reads the environment variable $OFFIS_SIMULINKSYNCINTERVAL_MS if defined and returns
// integer converted value. If not defined, it will return 1000.
int getSimulinkSyncIntervall()
{
  if (getenv("OFFIS_SIMULINKSYNCINTERVAL_MS") == 0) {
    icmMessage("W", PREFIX, "$OFFIS_SIMULINKSYNCINTERVAL_MS is not defined. Defaulting to 1 second sync intervall.");
    return(1000);
  } else {
    int simSyncInterval = atoi(getenv("OFFIS_SIMULINKSYNCINTERVAL_MS"));
    icmMessage("I", PREFIX, "Reading $OFFIS_SIMULINKSYNCINTERVAL_MS: %i ms.", simSyncInterval);
    return(simSyncInterval);
  }
}

// getSimulinkModel reads the environment variable $OFFIS_SIMULINKMODEL and if defined
// will copy the string it contains into buffer. It checks if buffer has sufficient size
// which is defined by second parameter length.
// It will then be checked if buffer contains the link to an existing file. On success
// third parameter cosimEnabled will be set to 1. If file does not exist or if $OFFIS_SIMULINKMODEL
// is not defined, cosimEnabled will be 0.
void getSimulinkModel (char * buffer, const int length, int * const cosimEnabled)
{
  if (getenv("OFFIS_SIMULINKMODEL") == 0) {
    icmMessage("W", PREFIX, "$OFFIS_SIMULINKMODEL is not defined. Disabling OVP-Simulink-cosimulation.");
    *cosimEnabled = 0;
  } else {
    if (length > strlen(getenv("OFFIS_SIMULINKMODEL"))) {
      // string will fit in buffer. Copy
      strcpy(buffer,getenv("OFFIS_SIMULINKMODEL"));
      FILE *file_p;
      if( (file_p=fopen(buffer,"r")) == NULL ) {
	icmMessage("F", PREFIX, "File not found! Check environment variable $OFFIS_SIMULINKMODEL.");
	*cosimEnabled = 0;
      } else {
	fclose(file_p);
	*cosimEnabled = 1;
	icmMessage("I", PREFIX, "Reading $OFFIS_SIMULINKMODEL: %s", buffer);
      }
    } else {
      icmMessage("F", PREFIX, "getSimulinkModel (char * buffer, const int length, int * const cosimEnabled): buffer too small!");
    }
  }
}

// getSimulinkModelName extracts file name from third parameter "simulinkModel"
// and copies result into "buffer". So path information in front of name will be
// removed, as well as the model ending ".mdl"
void getSimulinkModelName(char * buffer, const int length, char * simulinkModel)
{
  if (simulinkModel == NULL) {
    icmMessage("F", PREFIX, "getSimulinkModelName(char * buffer, const int length, const char * const simulinkModel): no input!");
  } else {
    char * findName = strrchr(simulinkModel, '/');
    if (findName == NULL) {
      // apparantly no path information in string. We already have the start of the name.
      findName = simulinkModel;
    } else {
      findName++; // we want to point one character past path that ends with '/'
    }
    if (length > strlen(findName)-4) {
      strncpy(buffer, findName, strlen(findName)-4);
      buffer[strlen(findName)-4]= '\0';  // since we did not copy string termination symbol attach now!
    } else {
      icmMessage("F", PREFIX, "getSimulinkModelName((char * buffer, const int length, const char * const simulinkModel): buffer too small!");
    }
    icmMessage("I", PREFIX, "Name of Simulink model: %s", buffer);
  }
}

// getSimulinkModelPath copies path information of string simulinkModel into buffer
// removing the file name. If the path is of relative style (./ or ../) it will be
// converted to absolut path by putting the current path (PWD) in front of it.
// If string simulinkModel contains just a filename without path information, just
// PWD will be copied to buffer.
void getSimulinkModelPath(char * buffer, const int length, const char * simulinkModel)
{
  if (simulinkModel == NULL) {
    icmMessage("F", PREFIX, "getSimulinkModelPath(char * buffer, const int length, const char * const simulinkModel): no input!");
  } else {
    char * findEnd = strrchr(simulinkModel, '/');
    if (findEnd == NULL) {
      // apparently simulinkModel just contains model name, no path at all
      // return(getenv("PWD"));
      if (length > strlen(getenv("PWD"))) {
	strcpy(buffer, getenv("PWD"));
      } else {
	icmMessage("F", PREFIX, "getSimulinkModelPath(char * buffer, const int length, const char * simulinkModel): buffer too small!");
      }
    } else {  
      int pathLength = (int)findEnd-(int)simulinkModel+1;
      if (simulinkModel[0] == '.') {
	// user used relative path to specify path, convert to absolute
	if (length > (strlen(getenv("PWD")) + 1 + pathLength)) {
	  strcpy(buffer, getenv("PWD"));
	  strcat(buffer, "/");
	  strncat(buffer, simulinkModel, pathLength);
	  buffer[strlen(getenv("PWD")) + 1 + pathLength] = '\0';
	} else {
	  icmMessage("F", PREFIX, "getSimulinkModelPath(char * buffer, const int length, const char * simulinkModel): buffer too small!");
	}
      } else {
	// user used absolute path
	if (length > pathLength) {
	  strncpy(buffer, simulinkModel, pathLength);
	  buffer[pathLength] = '\0';
	} else {
	  icmMessage("F", PREFIX, "getSimulinkModelPath(char * buffer, const int length, const char * simulinkModel): buffer too small!");
	}
      }
    }
  }
  icmMessage("I", PREFIX, "Path to Simulink model: %s", buffer);
}

#endif
