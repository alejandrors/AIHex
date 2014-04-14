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
  * Abstract:	This class contains the implementation for the 
		GUI for the Y GAME
  * ========================================================= 
*/ 

#include "ygui.h"

using namespace ARS; 

//> This function draws the board based on the game state. 

void YGUI::drawBoard(GameState * _gs) {
	unsigned int _size = _gs->getBoardSize();
	cout << "\n" << "      **** Current Y Board Position [Board Size = " << _size << "] ****" << "\n\n"; 
	string pattern1 = " / \\";
	string pattern2 = "| . ";
	string pattern3 = " \\ /";
	string patterns [] = {pattern1, pattern2, pattern3};
	char columnChar = 0x41; //The first character for the columns. 
	unsigned rowChar = 1;	//The first row number.
	unsigned _currentRow = 1;
	for (unsigned row =0; row<_size+1; row++) { //We assume that we have a size i board. 
		unsigned _iPattern = row > 1 ? 1 : 0; //For the first iteration we need to draw all cells; for the rest, we only need the bottom parttern.
		if (row==0) {
			cout << "       ";
			for (unsigned i = row; i<_size; i++) {
				cout << columnChar++ << "   ";
			}
			cout << endl << endl; 
			cout.flush();
		}
		else {
			for (; _iPattern < 3; _iPattern++) {
				for (unsigned i=0; i< 2*row; i++) {
					cout << " "; 
				}
				if (_iPattern == 1) {
					cout  <<  rowChar++ << "   "; 
				}
				else {
					cout << "    "; 
				}
				if (rowChar > 10 && _iPattern == 1)
					cout << "\b"; 
				for (unsigned i = row; i<_size+1; i++) {
					char temp [64];
					strcpy(temp, patterns[_iPattern].c_str());
					if (_iPattern == 1 && _currentRow <= _size) {
						unsigned k = 0;
						while(k<strlen(temp)) {
							if (temp[k]=='.') {
								unsigned _x =  i-_currentRow+1;
								if (_x <= _size) {
									switch(_gs->getCell(_x,_currentRow)) {
										case WHITE_CELL: 
											temp[k] = 'W';
											break; 
										case BLACK_CELL:
											temp[k] = 'B';
											break;
										default: 
											break;
										}
									}
								}
							k++;
						}
					}
					cout << temp;
				}	
				if (_iPattern ==1) {
					cout << "|";
				}
				if (_iPattern ==1) {
					_currentRow++;
				}
				cout << endl; 
			}
		}
	}
	cout.flush();
}

//> Prints the complete command list. 

void YGUI::printCommands() {
	cout << "\n\n\t\t\t\t------------ Commands -------------" << endl << endl; 
	cout << " [a]i {b,w} -> Makes an AI moves for a player. \t\t[p]lay {b,w} {coords,?} -> Makes a player or AI move." << endl;
    cout << " [b]oardsize n -> Sets a new game of size n in [1,26]. \t[q]uit -> Quits the game." << endl;
	cout << " [h]ex x y -> Sets the board as a {x,y} HEX. \t\t[r]esult -> Shows if current position wins." << endl;
	cout << " [i]nteractive {b,w} -> Starts a 1 player game.\t\t[s]howboard -> Prints the current game board." << endl;
	cout << " [l]ist_commands -> Prints this menu. \t\t\t[t]rain_ai -> Trains a specific AI agent based on defaults." << endl;
	cout << " [m]anage_ai -> Sends a command to the AI agent. \t[u]ndo -> Undoes the last move by any player." << endl;
	cout.flush();
}

//> prints the welcome screen. 

void YGUI::printWelcome() {
	cout  << "\n\n\n\t\t    __  __        ______     ______     __    __     ______" << endl;
	cout  << "\t\t   /\\ \\_\\ \\      /\\  ___\\   /\\  __ \\   /\\ \"-./  \\   /\\  ___\\" << endl;
	cout  << "\t\t   \\ \\____ \\     \\ \\ \\__ \\  \\ \\  __ \\  \\ \\ \\-./\\ \\  \\ \\  __\\" << endl;
	cout  << "\t\t    \\/\\_____\\     \\ \\_____\\  \\ \\_\\ \\_\\  \\ \\_\\ \\ \\_\\  \\ \\_____\\" << endl; 
	cout  << "\t\t     \\/_____/      \\/_____/   \\/_/\\/_/   \\/_/  \\/_/   \\/_____/" << endl;
	cout  << "\n\n\t\t   2013 (C) Alejandro Ramirez-Sanabria - Course Project for CMPUT 670" << endl;
	cout << "\t\t   For more information, please read the accompanying documentation."; 
}

//> Prints a prompt (yes/no), reads and returns the answer. 

bool YGUI::promptMessage(const char * __message) {
	string _commandLine;
	cout << "\n\n << Prompt >> " << __message << "? (Y/N) : " ;
	cout.flush();
	getline(cin, _commandLine);  
	transform(_commandLine.begin(), _commandLine.end(), _commandLine.begin(), ::tolower);
	if (strcmp(_commandLine.c_str(), "yes")==0 || strcmp(_commandLine.c_str(), "y")==0) {
		return true; 
	}
	if (strcmp(_commandLine.c_str(), "no")==0 || strcmp(_commandLine.c_str(), "n") ==0) {
		return false; 
	}
	throw YException(CommandInvalid); 
}

//> Reads a command from the prompt. The command is also validated for correctness
//	in terms of number of arguments and valid syntax. 

UICommand YGUI::readCommand(bool __interactive) throw (YException&) {
	string _commandLine, _command;
	char _c = '\0';
	int _result = -1; 
	char *_temp; 
	char _args [2][16];
	unsigned int _nCommands = 0, _nCommRead =0; 
 	bool _error = false; 
	//Get the command line.
	cout << "\n Command:> ";
	getline(cin,_commandLine);
	if (!_commandLine.size()) {
		throw YException(CommandInvalid);
	}
	transform(_commandLine.begin(), _commandLine.end(), _commandLine.begin(), ::tolower);	
	//Split it into tokens.
	_temp = strtok ((char*)_commandLine.c_str()," ");
	_command = string(_temp); 
	//Get the arguments (if any, max = 2)
	while(_temp) {
		_temp = strtok (NULL," ");
		if (_temp) {
			if (_nCommands < 2) {
				strcpy(_args[_nCommands],_temp);
			}	 
			_nCommands++;
		}
	}
	if (_command.length() > 1) { // If the command is in long version.
		int i = 0;
		while (i<INT_COMMAND_NUM && _command.compare(STR_COMMAND_NAMES[i++]));
		if (i==INT_COMMAND_NUM && _command.compare(STR_COMMAND_NAMES[i-1])) {
			_error = true;
		}
		else {
			_result = i-1; 
			_c = STR_COMMAND_SHORT[i-1][0]; 
			_nCommRead = INT_COMMAND_NUMARGS[i-1];
		}
	}
	else {
		_c = _command[0];
		int i = 0; 
		while(i<INT_COMMAND_NUM && STR_COMMAND_SHORT[i++][0] != _c);
		if (i==INT_COMMAND_NUM && STR_COMMAND_SHORT[i-1][0] != _c) {
			_error = true; 
		}
		else {
			_result = i-1; 
			_nCommRead = INT_COMMAND_NUMARGS[i-1];
		}
	}
	//Exception handling
	if (_error) {
		throw YException(CommandInvalid); 
	}
	if ((__interactive && _result ==4 && _nCommands != 1) || 
		(!__interactive && _nCommands != _nCommRead && _result == 4) || 
		(_result!=4 && _nCommRead != _nCommands)) { //We allow play to have one argument in some cases
		throw YException(CommandNumArgs); 
	}
	if (__interactive && _result == 4 && _nCommands ==1) {
		strcpy(_args[1],_args[0]);
	}
	//Return the command for the controller. 
	UICommand _comm; 
	_comm.commandNo = _result; 
	strcpy(_comm.arg1, _args[0]);
	strcpy(_comm.arg2, _args[1]);
	return _comm; 

}


void YGUI::printErrMessage(const char * _errMessage) {
	cerr << "\n << Error >> " << _errMessage << endl; 
	cerr.flush();  
}

void YGUI::printWarning(const char * _errMessage) {
	cout << "\n << Warning >> " << _errMessage << endl; 
	cout.flush();  
}

void YGUI::printInfo(const char * _info) {
	cout << "\n << Info >> " << _info << endl; 
	cout.flush();  
}

YGUI::~YGUI() {
	
}
