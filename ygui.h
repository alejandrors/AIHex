/** =========================================================
  * 
  * University of Alberta
  * Department of Computer Science
  * CMPUT 670 - Games, graphs and algorithms
  * 
  * Author:	Alejandro Ramirez Sanabria 
		(ramirezs@ualberta.ca)
  * Date:	30/JAN/2014
  * 
  * Abstract:	The header file for the GUI - Y GAME	
  * ========================================================= 
*/ 

	 
#ifndef Y_GAME_GUI
#define Y_GAME_GUI

#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include "yconstants.h"
#include "yexception.h"
#include "ygamestate.h"


using namespace std; 

/** CLASS MODEL
  * 
  * UI manager. Uses standard in/out for all input output. 
  * 
***/

namespace ARS { 

	//> A structure representing user commands read from the prompt and sent to 
	//	the controller. 
	struct UICommand {
		UICommand () : commandNo(-1) { arg1[0] = '\0'; arg2[0] = '\0';}
		int commandNo;
		char arg1[64];
		char arg2[64]; 
	};

	class YGUI {
		private: 
		bool initialized; 	//> True if the UI has been initialized.  

		public:
		void printCommands();	
		UICommand readCommand(bool) throw (YException&); 
		void drawBoard(GameState *);
		void printWelcome();  
		void printInfo(const char *); 
		void printErrMessage(const char *); 
		void printWarning(const char *); 
		bool promptMessage(const char *);
		public: 	
		YGUI() {}
		~YGUI(); 
	};

}

#endif 
