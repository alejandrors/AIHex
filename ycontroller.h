/** =========================================================
  * 
  * University of Alberta
  * Department of Computer Science
  * CMPUT 670 - Games, graphs and algorithms
  * 
  * Author:	Alejandro Ramirez Sanabria 
		(ramirezs@ualberta.ca)
  * Date:	2/FEB/2014
  * 
  * Abstract:	Assignment 2 - This is the controller class
  *		for the Y Board game. This file contains the
  * 		defintion only. 
  * ========================================================= 
*/ 

#ifndef Y_CONTROLLER_H
#define Y_CONTROLLER_H

#include <cstdio>
#include <cstdlib>
#include <list>
#include <typeinfo>
#include "ygui.h"
#include "ygamestate.h" 
#include "yexception.h"
#include "yconstants.h"
#include "ygamestate.h"
#include "yai.h"

/** CLASS MODEL 
  * 
  * The class YController controls the main execution flow, using
  * a model view controller (MVC) pattern.
  *
  * Model : YGameState
  * Controller : This class
  * View : YGUI 
  * 
  * This file defines the controller class for the Y Game. For detailed
  * comments, view the implementation file. 
*/

namespace ARS {

	class YController {

		//Class members
		private: 	
			YGUI * ui;	//> The UI subsystem.
			GameState * gs; //> The model/gamestate manager. 
			YAI * ai;  	//> The AI manager for second player moves. 

			bool interactive; 	//> Type of session running. 
			bool exit; 		//> If true, the game exits. 
			bool gameInProgress; 		//> If false, the game has not been initialized.
			CellType lastLegalMove; 	//> Records the player who made the last legal move.
			CellType player; 
			list<PlayerMove> * history; 	//> A history list with all the previous player moves. 
			bool (YController::*commands[CONST_NUM_COMMANDS]) (UICommand); 	//> A function array used to map UI actions to commands.
		private:
            UICommand generateCommAI(bool, CellType);
			bool checkIfInt(const char *);
			bool checkIfDouble(const char *);
			bool showboard(UICommand); 
			bool listCommands(UICommand); 
			bool boardsize(UICommand); 
			bool hex(UICommand); 
			bool play(UICommand); 
			bool result(UICommand); 
			bool undo(UICommand); 
			bool quit(UICommand); 
			bool aimove(UICommand); 			
			bool mainMenuCycle();
			bool playFull(UICommand); 
			bool trainAI(UICommand);
			bool manageAI(UICommand);   
			bool executeCommand(UICommand);
			//Miscellaneous AI functions.
			bool initializeAI(const char *);
			bool sendParamsToAI(const char *);
            bool tournament(const char *);
		public:
			YController();
			~YController();
			void startYGame(); 
	};
}

#endif


