......................................................................................................................................................................................................................................................
PROJECT TITLE:	NFV Simulation of Evolved Packet Core - MME, S-GW AND P-GW

Author:		Sadagopan N S
				Master of Technology in Computer Science and Engineering
				Dept. of Computer Science and Engineering
				IIT Bombay
......................................................................................................................................................................................................................................................
......................................................................................................................................................................................................................................................
Coding Convention followed:

 	-All data member and function declarations are placed in header files and their definitions are placed in corresponding cpp files
	-Default indentation and spacing rules are followed in all parts of the source code
 
Naming: (Followed Google's conventions except in cases of const, functions, enum and class variables)
 
 	Files
 		All files are in lower case and are separated by underscore if they have more than one word
	Variables 
		All variable names are in lower case and are separated by underscore if they have more than one word
	Global variables 
		Only global variables have "g_" prepended with their names
	Type names(Classes, structs, etc.)
		All type names start with a capital letter and are capitalized for each new word with no underscores	
	Macros
		All macro names are capitalized and are separated by underscore if they have more than one word
	Functions
		All function names follow the naming of variable names
	All other names(if not given above) should follow the naming of variables
......................................................................................................................................................................................................................................................
......................................................................................................................................................................................................................................................
List of program files:

SERVER:	server.cpp server.h
CLIENT:	client.cpp client.h
DATABASE:	db.cpp db.h
USER EQUIPMENT:	ue.cpp ue.h
UTILITY FUNCTIONS:	utils.cpp utils.h
TRAFFIC GENERATION:	ran.cpp ran.h
MOBILITY MANAGEMENT ENTITY:	mme.cpp mme.h
HOME SUBSCRIBER SERVER:	hss.cpp hss.h
......................................................................................................................................................................................................................................................
......................................................................................................................................................................................................................................................
List of executables:

TRAFFIC GENERATION:	ran
MOBILITY MANAGEMENT ENTITY:	mme
HOME SUBSCRIBER SERVER:	hss
......................................................................................................................................................................................................................................................
