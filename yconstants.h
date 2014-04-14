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
  * Abstract:	Assignment 2 - Constants file  
  * ========================================================= 
*/ 

#ifndef Y_CONSTANTS_H
#define Y_CONSTANTS_H

#define DEFAULT_SIZE 8 	//> Default board size. 
#define CONST_NUM_COMMANDS 12 //>MUST agree with the INT_NUM_COMMANDS

//> This is a simple constant file, define in the corresponding CPP. 

namespace ARS {
	extern const char * const STR_COMMAND_NAMES []; //> The long names of the commands. 
	extern const char * const STR_COMMAND_SHORT [];	//> The short names of the commands.
	
	extern const int INT_COMMAND_NUMARGS []; 	//> The numnber of arguments each command takes.
	extern const int INT_COMMAND_NUM;		//> The number of commands available.
    extern const int INT_MAX_BOARD_SIZE;
    extern const int INT_DEFAULT_NUMCORES;
    extern const int INT_MLMCTS_MINDIMENSION;
	
	extern const char * STR_EXCEPTION_COMMAND; 	//> Exception messages.
    extern const char * STR_EXCEPTION_ARGS;
	extern const char * STR_EXCEPTION_NOTPLAY;
	extern const char * STR_EXCEPTION_COORDS;
    //extern const char * STR_EXCEPTION_ARGSINVALID;
	extern const char * STR_EXCEPTION_GENERAL; 
    extern const char * STR_EXCEPTION_MCTSTO;
	
	extern const char * STR_WARNING_MOVE;		//> Warning and error messages. 
	extern const char * STR_WARNING_UNDO; 
    extern const char * STR_WARNING_AIPARAMS;
	extern const char * STR_ERROR_NOGAME;
	extern const char * STR_ERROR_INT; 
	extern const char * STR_ERROR_AIFAIL; 
	extern const char * STR_ERROR_SERIALIZED; 
    extern const char * STR_ERROR_TOURNAMENT;

	extern const char * STR_UPDATE_PARAMITER; 
		
	extern const char * STR_GAME_WHITEWIN;		//> Win/lose messages. 
	extern const char * STR_GAME_BLACKWIN;
	extern const char * STR_GAME_NOWIN;
	extern const char * STR_GAME_INT;
	extern const char * STR_GAME_AIMOVE; 
	extern const char * STR_GAME_NOMOVE;
	extern const char * STR_GAME_OVER; 

	extern const char * STR_INFO_RANDAI; 
	extern const char * STR_INFO_RLAI; 
    extern const char * STR_INFO_MCTS;
	extern const char * STR_INFO_AITRAIN; 
	extern const char * STR_INFO_AITRAINDONE; 
	extern const char * STR_INFO_SERIALIZED; 
	extern const char * STR_INFO_LOGRUNSTATS; 
	extern const char * STR_INFO_LOGRUNERROR; 
	extern const char * STR_INFO_DESERIALIZED; 

    extern const char * STR_INFO_AIMOVE;
    extern const char * STR_INFO_AITRAIN_TIME ;
    extern const char * STR_INFO_AISTATS;
    extern const char * STR_INFO_MCTS_STATS;
    extern const char * STR_INFO_MLMCTS_STATS;
    extern const char * STR_INFO_MLMTCS;
	extern const char * STR_INFO_RLAI_MSESTEP; 
	extern const char * STR_INFO_RLAI_LOGSTART; 
	extern const char * STR_INFO_RLAI_LOGEND;
	extern const char * STR_INFO_RLAI_LOGCONFIG; 
    extern const char * STR_INFO_TOURNAMENT;
    extern const char * STR_INFO_TOUR_RESULTS;

	extern const char * STR_FILENAME_RLAILOG; 
	extern const char * STR_FILENAME_RLAICSV; 
    extern const char * STR_FILENAME_RLAIDATA;

    extern const char * STR_FILENAME_MCTSLOG;
    extern const char * STR_FILENAME_MCTSCSV;
    extern const char * STR_FILENAME_MCTSDATA;

	extern const char * STR_PROMPT_INT; 		//> Prompt messages. 

	extern const unsigned int INT_RLAI_FEATSIZE;
	extern const unsigned int INT_DEFAULT_BOARDSIZE;

	extern const double DOUBLE_RLAI_LAMBDA;
	extern const double DOUBLE_RLAI_ALPHA;
	extern const double DOUBLE_RLAI_GAMMA;
	extern const double DOUBLE_RLAI_EPSILON;
    extern const double DOUBLE_MCTS_BETA;

    extern const double DOUBLE_MCTS_SECSPERMOVE;
}

#endif 

