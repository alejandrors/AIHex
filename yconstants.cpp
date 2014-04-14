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
  * Abstract:	Assignment 2 - Constants file definition 
  * ========================================================= 
*/ 

#include "yconstants.h"

namespace ARS {


const char * const STR_COMMAND_NAMES [] = {"showboard", "list_commands", "boardsize", "hex", "play", "result", "undo", "quit", "ai", "interactive", "train_ai" , "manage_ai"};
const char * const STR_COMMAND_SHORT [] = {"s","l","b","h","p","r","u","q","a","i", "t", "m"};

const int INT_COMMAND_NUMARGS [] = {0,0,1,2,2,0,0,0,1,1,0,2};
const int INT_COMMAND_NUM = 12; 	//Remember to update the H file.
const int INT_MAX_BOARD_SIZE = 26;
const int INT_DEFAULT_NUMCORES = 1;
const int INT_MLMCTS_MINDIMENSION = 5;

const char * STR_EXCEPTION_COMMAND = {"Invalid command sequence or invalid syntax -- command aborted."}; 
const char * STR_EXCEPTION_ARGS = {"Invalid command sequence due to one or more invalid arguments -- command aborted."};
const char * STR_EXCEPTION_COORDS = {"Invalid coordinates for the current game board (cell taken or inexistent) -- command aborted."}; 
const char * STR_EXCEPTION_NOTPLAY = {"Invalid player specified; please select between 'b' and 'w' -- command aborted."}; 
const char * STR_EXCEPTION_GENERAL = {"Oops, unknown exception! Please verify that the commnand is well-formed and try again -- command aborted."};
const char * STR_EXCEPTION_MCTSTO  = {"MCTS Timeout -- finalized simulation in progress."};

const char * STR_WARNING_UNDO = {"There is no player move to undo -- command ignored."};
const char * STR_WARNING_MOVE = {"Wrong player moving (illegal move) -- command accepted."};
const char * STR_WARNING_AIPARAMS = {"The current AI manager is not conforming with this new board specification. Configure manually."};

const char * STR_ERROR_NOGAME = {"Game board has not been initialized -- command ignored."};
const char * STR_ERROR_INT = {"An interactive game is already in progress -- command ignored."};
const char * STR_ERROR_AIFAIL = {"The AI agent has reported that the learning process was not successful -- check log for details."};
const char * STR_ERROR_SERIALIZED = {"Error while (de)serializing -- check log for details."};
const char * STR_ERROR_TOURNAMENT = {"Errow while running tournament -- check log for details and make sure the opponents are compatible."};
const char * STR_UPDATE_PARAMITER = {"A value has been updated (number of iterations) with value = "};

const char * STR_GAME_WHITEWIN = {"White player has won the game."}; 
const char * STR_GAME_BLACKWIN = {"Black player has won the game."}; 
const char * STR_GAME_NOWIN = {"The current game has no winning position for any player."}; 
const char * STR_GAME_INT = {"Interactive mode enabled. In this mode '[p]lay' takes only one argument (coordinates)."};
const char * STR_GAME_AIMOVE = {"Please wait, the AI is making a move..."};
const char * STR_GAME_NOMOVE = {"There are no legal moves left."}; 
const char * STR_GAME_OVER = {"Game over: interactive mode disabled."};


const char * STR_INFO_AISTATS = {"*** AI Move and Related Statistics ***"};
const char * STR_INFO_AIMOVE = {"AI moves to cell %c%d. Processing time: %f seconds."};
const char * STR_INFO_AITRAIN_TIME = {"Learning time: %f seconds."};
const char * STR_INFO_MCTS_STATS = {"Nodes expanded: %d (using %d execution threads)."};
const char * STR_INFO_MLMCTS_STATS = {"Nodes expanded in all dimensions: %d (using %d execution threads)."};
const char * STR_INFO_RANDAI = {"A new AI agent type has been selected -- Uniform Random AI."};
const char * STR_INFO_RLAI = {"A new AI agent type has been selected -- Reinforcement Learning SARSA(Lambda)"};
const char * STR_INFO_MCTS = {"A new AI agent type has been selected (MTN-MCTS)-- Multi-Threaded Naive MCTS."};
const char * STR_INFO_MLMTCS = {"A new AI agent type has been selected (ML-MCTS)-- Multi-Level MT-MCTS."};
const char * STR_INFO_AITRAIN = {"A new AI training session has commenced based on current parameters -- this might take several minutes."};
const char * STR_INFO_AITRAINDONE = {"The AI training session has been successfully completed!  -- This module is now operational."};
const char * STR_INFO_SERIALIZED = {"The AI has been serialized successfully."};
const char * STR_INFO_DESERIALIZED = {"The AI has been deserialized successfully."};
const char * STR_INFO_LOGRUNSTATS = {"Execution time."};
const char * STR_INFO_LOGRUNERROR = {"RL Error value."};
const char * STR_INFO_TOURNAMENT = {"The tournament was completed successfully."};
const char * STR_INFO_TOUR_RESULTS = {"Win rate for the main agent: %f."};

const char * STR_INFO_RLAI_MSESTEP = {"Average reward per group of iterations."};
const char * STR_INFO_RLAI_LOGSTART = {"A new training session has been started"}; 
const char * STR_INFO_RLAI_LOGEND =  {"The training session has ended successfully."}; 
const char * STR_INFO_RLAI_LOGCONFIG = {"CONFIG >> Alpha: %f, Gamma: %f, Epsilon: %f, Lambda: %f, Boardsize: %d, Iterations: %d"};

const char * STR_FILENAME_RLAILOG = {"rlai_agent.log"};
const char * STR_FILENAME_RLAICSV = {"rlai_agent.csv"};
const char * STR_FILENAME_RLAIDATA = {"rlaidata"};

const char * STR_FILENAME_MCTSLOG = {"mcts_agent.log"};
const char * STR_FILENAME_MCTSCSV = {"mcts_agent.csv"};
const char * STR_FILENAME_MCTSDATA = {"mctsdata"};

const char * STR_PROMPT_INT = {"Do you want to start an interactive game"};

const unsigned int INT_RLAI_FEATSIZE = 5;
const unsigned int INT_DEFAULT_BOARDSIZE = 5;


const double DOUBLE_RLAI_LAMBDA = 1.0;
const double DOUBLE_RLAI_ALPHA = 0.0000001; //+2
const double DOUBLE_RLAI_GAMMA = 1.0;
const double DOUBLE_RLAI_EPSILON = 0.1;

/*
const double DOUBLE_RLAI_LAMBDA = 1.0;

const double DOUBLE_RLAI_ALPHA = 0.00000000001; //+6
const double DOUBLE_RLAI_GAMMA = 1.0;
const double DOUBLE_RLAI_EPSILON = 0.1;
*/
/*
const double DOUBLE_RLAI_LAMBDA = 0.99;
const double DOUBLE_RLAI_ALPHA = 0.000001;
const double DOUBLE_RLAI_GAMMA = 1;
const double DOUBLE_RLAI_EPSILON = 0.1;*/

const double DOUBLE_MCTS_SECSPERMOVE = 2.5;
const double DOUBLE_MCTS_BETA = 100;

/*
 *Values that worked for N = 4
 *
 *const double DOUBLE_RLAI_LAMBDA = 0.99;
const double DOUBLE_RLAI_ALPHA = 0.01;
const double DOUBLE_RLAI_GAMMA = 0.8;
const double DOUBLE_RLAI_EPSILON = 0.1;
*/

} 
