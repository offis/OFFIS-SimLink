# OFFIS SimLink: Open Virtual Platform - Matlab/Simulink co-simulation

# Introduction

The development of personal computer SW is a task that can be completely handled on a PC. For embedded systems there is a different story to tell. Embedded SW is not developed on the embedded system itself. A PC and a cross-compiler are required. To execute the program it needs to be downloaded to the embedded system first and this is where evaluation and verification can become laborious. “Embedded” means that these systems are part of a bigger maybe very complex environment that they should monitor and control. During verification and validation (debugging) it can be impossible to control the environment in such a way that repeatable regression tests are observable and thereby can be debugged. OFFIS chosen strategy relies on intensive system simulations. The system and environment are implemented as executable models. If the abstraction of simulation it is still accurate enough to depict the relevant aspects of the system, it has many advantages:

* **Reproducible Test Scenarios** - A simulated system and environment is completely determined. Any situation and test can clearly be defined to allow the implementation of regression tests for best effort embedded code coverage, verification and validation.
* **Fast Execution** - The virtual platform simulates about a factor of six faster than real time. A one day test scenario can thereby be evaluated within four hours of computation time. This enables to run through a wider spectrum of test cases in less time.
* **Shortening Implementation Time** - Instead of waiting for the hardware to be developed, manufactured and delivered first, the embedded software can be implemented by means of the virtual platform following a hardware-software co-design methodology. This enables the very early start of embedded software implementation together with system and hardware design, shortening the time period to software release.
* **Safety** - The installation of unverified and unvalidated software under development can be a hazard to expensive first prototype hardware, potentially damaging it or even be a thread to personal well-being. Extensive simulation of many defined scenarios helps to find flaws and to start prototype evaluations with an already well matured software basis.

# Technology

[[https://github.com/offis/OFFIS-SimLink/blob/master/images/Fig01_virtual_prototype_in_the_loop_simulation.png|Virtual prototype in the loop simulation]]
_**Figure 1: Virtual prototype in the loop simulation.**_

The implementation we present enables a methodology called virtual platform in the loop simulation (Figure 1), which is an intermediate step towards hardware in the loop simulation. The challenge is here to synchronize two independent simulations so that the simulated time does not diverge and data can be exchanged at the right instant of time. OffisSimLink is a bus peripheral model for the commercial [Open Virtual Platform](https://www.ovpworld.org/) which enables simulating embedded systems running real application code. Attached to the simulated bus of an instruction set simulator it enables the exchange of data with a [Matlab/Simulink](https://de.mathworks.com/) model via memory mapped IO. Figure 2 depicts the architecture of the implementation.

[[https://github.com/offis/OFFIS-SimLink/blob/master/images/Fig02_ovp-simulink_co-simulation_architecture.png|OVP-Simulink co-simulation architecture]]

_**Figure 2: OVP-Simulink co-simulation architecture.**_

OVP and Matlab/Simulink are executed in parallel on a Host PC. This can either be one or two individual machines connected via network. OffisSimLink occupies address space in the OVP simulated system on chip. Any read/write transaction to this address space on the bus is forwarded to Matlab/Simulink using Mathworks engine.h API which was used to implement a semihost.dll for data exchange. Both simulations run in parallel and synchronize every defined time period. Matlab/Simulink automatically STOPs when it reaches a synchronization point and will be continued by OffisSimLink after synchronization. The advantage of this synchronization scheme is the full parallel execution of both simulators which delivers best simulation performance in comparison to a mutual exclusive approach. The disadvantage is a loss in simulation accuracy since changes in one of the simulations will be available in the other only every synchronization interval. Since this interval is configurable the disadvantage can be minimized to a tolerable influence on the simulation results depending on the simulation use case.

# Example

An example including the OffisSimLink bus peripheral model and its sources are available for download here. The example, as well as OffisSimLink are outcome of OFFIS research projects. The download is not to be mistaken for a product release. Its use is for publishing our concept only and supporting you with your own adaptations and development. OFFIS does not guarantee that the download will run out of the box on your system, will compile without modifications depending on your system or fulfills any standards of quality.

## System Requirements

The downloadable sources where developed on a Windows 7 system. It is assumed that you have [Open Virtual Platform](https://www.ovpworld.org/) and [Matlab/Simulink](https://de.mathworks.com/) installed correctly. The downloadable sources and makefiles will require (easy) modification according to your installation path and versions of these tools.

## Directory Structure

Please have a look at the following directory structure. You will be required to call “_**make**_” in several branches of the directory structure.

### EmbeddedApplication

Contains the source code (main.c) of the embedded application to be executed on the instruction set simulator. This part resembles to the white box in Figure 2 being labeled “Hello.c”. Calling “_**make**_” will cross-compile the source and generate an ELF (OffisSimLinkDemo.elf). Cross-compilation requires that your [Open Virtual Platform](https://www.ovpworld.org/) includes the ARM tool suite.

### MatlabSimulink

Contains the demo Simulink model. It can be opened and modified using your installed [Matlab/Simulink](https://de.mathworks.com/). “mySimulink.mdl” resembles to the green box within Figure 2.

### OVP/offis.ovpworld.org/peripheral/OffisSimLink/1.0/semihost

Contains the source code for the “Semihost.dll” as shown in Figure 2. Calling “make” will generate the “model.dll”. You will need to modify the file “Makefile” according to your tool installation paths.

### OVP/offis.ovpworld.org/peripheral/OffisSimLink/1.0

Contains the source code for the OVP bus peripheral model (OffisSimLink (x86 ISS)) as shown in Figure 2. Calling “_**make**_” will generate the “pse.pse” which will be included by the OVP system on chip simulation.

### OVP

Contains the source code for the OVP Model (compare with Figure 2). The example “platform.c” contains the instances of one ARM instruction set simulator that will execute the “OffisSimLinkDemo.elf” of the directory “EmbeddedApplication” and one instance of OffissimLink (“pse.pse” of previously mentioned directory). Calling “_**make**_” will generate the executable “offisVP.exe”.

## Starting the Example

Calling “offisVP.exe” in the directory “OVP” will initialize the example platform by loading the ELF into the instruction set simulator, starting Matlab and opening the Simulink model. You will experience an “embedded SW for-loop” incrementing actuator values in Simulink. Simulink increments these actuator values after which they are read back by the embedded SW. This demonstrates both directions of data transmission.

# Acknowledgements

OFFIS acknowledges the support by the 7th Framework Program ICT-4-5.1 for Personal Health Systems of the European Community (grant agreement: 248261) for the [Nephron+](https://nephronplus.eu/) project for which the OfisSimLink peripheral was implemented. In the same context OFFIS has to recognize the willingness of [Imperas](https://www.imperas.com) to provide the OVPsim environment under a research license.

# Copyright

Copyright (c) 2010     OFFIS Institute for Information Technology,     
                       26121 Oldenburg, Germany                        
                                                                       
All rights reserved.                                                   
                                                                       
This file is directly or indirectly part of the OFFIS Virtual Platform.
A simualtion of an ARM7tdmi micro processor with OVP (a), FreeRTOS     
operating system (b) and user defined tasks.                           
                                                                       
(a) http://www.ovpworld.org/                                           
(b) http://www.freertos.org/                                           
                                                                       
Created for the European projects: Nephron+ (1)                        
                                                                       
1) http://www.nephronplus.eu/                                          
                                                                       
A list of authors and contributors can be found in the accompanying    
AUTHORS file.  For detailed copyright information, please refer        
to the COPYING file.                                                   
                                                                       
Contact information:                                                   
 OFFIS                                                                 
   Institute for Information Technology                                
 Dipl.-Inform. Frank Poppen                                            
   Escherweg 2                                                         
   D-26121 Oldenburg                                                   
   Germany                                                             
 www  : http://www.offis.de/                                           
 phone: +49 (441) 9722-230                                             
 fax  : +49 (441) 9722-128      
