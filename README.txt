PROJECT TITLE:	NFV Simulation of Evolved Packet Core - MME, S-GW AND P-GW

Author:		Sadagopan N S
				Master of Technology in Computer Science and Engineering
				Dept. of Computer Science and Engineering
				IIT Bombay

Coding style - Traditional Coding style followed

- Opening brace on the same line as the operation that initiates it. Closing brace on its own line
- Space after commas, semicolon, operators
- Space around binary operator except operators "->" and ".". No space around unary operator
- Spaces after keywords like "if", "while" eg. if (condition)
- Space before opening brace eg. "if () {" and NOT "if (){"
- No spaces between function names and opening parenthesis eg. "func()" and NOT "func ()"
- Blank lines to separate units of functionality
- Use 3spaces for each tab
- [PENDING] Make code to fit in 80-character column width

Coding Convention followed:

 	-All data member and function declarations are placed in header files and their definitions are placed in corresponding cpp files
	-Default indentation and spacing rules are followed in all parts of the source code
 
Naming: (Followed Google's code convention except in cases of const, functions, enum and class variables)

	Variables 
		All variable names are in lower case and are separated by underscore if they have more than one word (multiple words can be combined into a single word and name shortening is allowed)
		Global variables: Only global variables have "g_" prepended with their names
 	Files
 		Follow the naming of Variables
	Type names(Classes, structs, etc.)
		All type names start with a capital letter and are capitalized for each new word with no underscores. Special cases like acronyms and abbreviations are all written in full caps. Eg: TCP. 
	Macro
		All macro names follow the naming of variables except that they are capitalized
	Functions
		Follow the naming of Variables
	All other names(if not given above) should follow the naming of variables

List of program files:

SERVER:	server.cpp server.h
CLIENT:	client.cpp client.h
DATABASE:	db.cpp db.h
USER EQUIPMENT:	ue.cpp ue.h
UTILITY FUNCTIONS:	utils.cpp utils.h
TRAFFIC GENERATION:	ran.cpp ran.h
MOBILITY MANAGEMENT ENTITY:	mme.cpp mme.h
HOME SUBSCRIBER SERVER:	hss.cpp hss.h


List of executables:

TRAFFIC GENERATION:	ran
MOBILITY MANAGEMENT ENTITY:	mme
HOME SUBSCRIBER SERVER:	hss