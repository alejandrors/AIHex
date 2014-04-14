/** =========================================================
  * 
  * University of Alberta
  * Department of Computer Science
  * CMPUT 670 - Games, gRaphs and algorithms
  * 
  * Author:	Alejandro Ramirez Sanabria 
		(ramirezs@ualberta.ca)
  * Date:	2/FEB/2014
  * 
  * Abstract:	Assignment 2 - This is the implementation file
  *		for the YGame controller class defined in the
  *		header file. 
  * ========================================================= 
*/ 

#include "ycontroller.h"

using namespace ARS;

//Checks if a string contains an integer. 

bool YController::checkIfInt(const char * __str) {
	string _str = string(__str);
	string::iterator _temp = _str.begin(); 
	while (_temp!= _str.end() && isdigit(*_temp)) {
		_temp++;
	}
	return _temp==_str.end();
}

bool YController::checkIfDouble(const char * __str) {
	bool _dot = false;
	string _str = string(__str);
	string::iterator _temp = _str.begin(); 
	while ((_temp!= _str.end()) && (isdigit(*_temp) || (*_temp=='.' && !_dot))) {
		if(*_temp=='.') {
			_dot = true; 
		}
		_temp++;
	}
	return _temp==_str.end();
}
//Creates a command object for the AI
UICommand YController::generateCommAI(bool _interactive, CellType _aiColour) {
	UICommand _comm;
	ui->printInfo(STR_GAME_AIMOVE);
    char _buffer[128];
    struct timespec _start, _finish;
    //TODO : set to the correct colour
    //CellType _aiColour  = this->player == BLACK_CELL ? WHITE_CELL : BLACK_CELL;
	ai->setCurrentPlayer(_aiColour); 	

    clock_gettime(CLOCK_MONOTONIC, &_start);
    Coords _move = ai->makeMove(this->gs, this->history);
    clock_gettime(CLOCK_MONOTONIC, &_finish);
	if (_move.x == 0 || _move.y ==0) {
		throw YException(NoMoves);
	}
    double _time = YAI::getElapsedSeconds(_start, _finish);
    char _c = _move.x + 0x60;
    sprintf(&_buffer[0], STR_INFO_AIMOVE, _c, _move.y, _time);
    ui->printInfo(_buffer);
    ui->printInfo(STR_INFO_AISTATS);
    _comm.arg2[0] = _move.x + 0x60;
    if (_move.y >= 10) {
        _comm.arg2[1] = (_move.y / 10) + 0x30;
        _comm.arg2[2] = (_move.y % 10) + 0x30;
		_comm.arg2[3] = '\0';
	} else {
        _comm.arg2[1] = _move.y + 0x30;
		_comm.arg2[2] = '\0';
 	}
	if (_interactive) 
		_comm.arg1[0] = player == BLACK_CELL ? 'w' : 'b';
	return _comm;
}

// This method starts an interactive game session or continues 
//	an existing one. 

bool YController::playFull(UICommand _comm) {
	if (!gameInProgress) {
		ui->printErrMessage(STR_ERROR_NOGAME);
		return false;
	}
	if (interactive) {
		ui->printErrMessage(STR_ERROR_INT);
		return false; 
	}
	if (strlen(_comm.arg1) != 1) {
		throw YException(NotPlayer);
	} 
	if (!ui->promptMessage(STR_PROMPT_INT)) {
		return false; 
	}
	ui->printInfo(STR_GAME_INT);
	switch (tolower(_comm.arg1[0])) {
		if (strlen(_comm.arg1)>1) {
			throw YException(ArgsInvalid);
		}
		case 'b':
			player = BLACK_CELL;
			break;
		case 'w': 
			player = WHITE_CELL;
			break;
		default :
			YException e(NotPlayer);
			throw e;   
	}
	this->interactive = true;
	if (lastLegalMove==player) {
        CellType _aiColour = player == WHITE_CELL ? BLACK_CELL : WHITE_CELL;
        UICommand _comm = generateCommAI(interactive, _aiColour);
		bool _r = this->play(_comm);
		this->result(_comm); 
		this->showboard(_comm);
		return _r; 
	}
	return true; 
}

//This method ends the Y program.

bool YController::quit(UICommand _comm) {
	this->exit = true; 
	return true; 
}

//This method prints the list of commands available.

bool YController::listCommands(UICommand _comm) {
	ui->printCommands();
	return true; 
}

//This method creates a x times y HEX board. If there is a game
//	in progress, it stars a new one on a same-sized board. 

bool YController::hex(UICommand __comm) {
	if (gameInProgress) {
		int _x, _y; 
		try {
			if (!checkIfInt(__comm.arg1) && !checkIfInt(__comm.arg2)) {
				throw YException(ArgsInvalid);
			}
			_x = atoi(__comm.arg1);
			_y = atoi(__comm.arg2); 
		} catch (exception &e) {
			throw YException(ArgsInvalid); 
		} 
		gs->setBoard(gs->getBoardSize()); 
		gs->setHex(_x,_y); 
		return true;
	} else {
		ui->printErrMessage(STR_ERROR_NOGAME);
		return false;
	} 
}

//This method executes the play command, one move at a time. 
//	It receives a UICommand object with the player and 
//	desired coordinates. 
//	--> Works both for UI and human!

bool YController::play(UICommand _comm)  {
	unsigned int _x;
	unsigned int _y;
	if (gameInProgress) {
		//Validate that that we have a set of coordinates xy, x \in [a,z] and y integer.
		try {
			if (tolower(_comm.arg2[0]) > 0x60 && tolower(_comm.arg2[0]) < 0x7B) {
                _x = (int)(_comm.arg2[0] - 0x60); //Transform ascii letters to integers
				if (!checkIfInt(&_comm.arg2[1])) {
					throw YException(ArgsInvalid);
				}
                _y = atoi(&_comm.arg2[1]);
			}
			else if (tolower(_comm.arg2[0])=='?') {
				//AI move
                Coords _coords = ai->makeMove(this->gs, this->history);
				if (_coords.x == 0 || _coords.y ==0) {
					ui->printInfo(STR_GAME_NOMOVE); 
					return false; 
				}
				_x = _coords.x;
				_y = _coords.y;
			} else {
				throw exception();
			}
		} catch (exception &e) {
			throw YException(ArgsInvalid);
		}
		//Check if cell is empty and proceed to assign it.
		//cout << "X: " << _x << "Y: " << _y; 
		if (gs->getCell(_x,_y)==EMPTY_CELL) {
			CellType _player = EMPTY_CELL; 
			if (!interactive || (interactive && player==lastLegalMove)) {
				if (strlen(_comm.arg1)>1) {
					throw YException(ArgsInvalid);
				}
				switch (tolower(_comm.arg1[0])) {
					case 'b':
						_player = BLACK_CELL;
						break;
					case 'w': 
						_player = WHITE_CELL;
						break;
					default :
						YException e(NotPlayer);
						throw e;   
				}
			} 
			else {
				_player = player; 
			}
			if (lastLegalMove==_player) {
				ui->printWarning(STR_WARNING_MOVE);
			}
			// Changes the last legal move accordingly and adds the move to the history. 
			// Marks the cell. 
			this->lastLegalMove = _player; 
			gs->markCell(_x,_y,_player);
			PlayerMove _move;
			_move.x = _x;
			_move.y =  _y;
			_move.type = _player;
			history->push_front(_move);
		}
		else {
			YException e(BadCoordinates);
			throw e;
		}
		if (interactive && lastLegalMove==player) { // If this was a player move, we need to play with the AI.
			//ui->printInfo(STR_GAME_AIMOVE);
            CellType _aiColour  = this->player == BLACK_CELL ? WHITE_CELL : BLACK_CELL;
            UICommand _comm = generateCommAI(interactive, _aiColour);
			bool _r = this->play(_comm);
			this->result(_comm);
			this->showboard(_comm);
			return _r;
		}
		return true; 
	}
	else {
		ui->printErrMessage(STR_ERROR_NOGAME);
		return false;  
	}
}

// This method undoes the last action. 
// It uses the history list to check the color of player and
// the position.  

bool YController::undo(UICommand _comm)  {
	if (gameInProgress && !history->empty()) {
		if (!history->empty()) {
			PlayerMove _last = *history->begin();
			history->pop_front();
			gs->markCell(_last.x,_last.y, EMPTY_CELL); 
			switch(_last.type) {
				case WHITE_CELL: 
					this->lastLegalMove = WHITE_CELL;
					break;
				case BLACK_CELL:
					this->lastLegalMove = BLACK_CELL;
					break;
				case EMPTY_CELL:
					break; 
			}		
		}
		else {
			ui->printWarning(STR_WARNING_UNDO);
			return false;  
		}
	} else {
		ui->printErrMessage(STR_ERROR_NOGAME);
		return false;  
	}
	return true; 
}

// Checks if the current game position is a win for 
// any player via Schedsten reduction.

bool YController::result(UICommand _comm) {
	const char * _str;
	if (this->gameInProgress) {
		switch (gs->fullReduce()) {
			case WHITE_CELL:
				_str = STR_GAME_WHITEWIN;
				if (interactive) ui->printInfo(STR_GAME_OVER); 
				interactive = false; 
				break; 
			case BLACK_CELL:
				_str = STR_GAME_BLACKWIN;
				if (interactive) ui->printInfo(STR_GAME_OVER); 
				interactive = false; 
				break;
			case EMPTY_CELL:
				_str = STR_GAME_NOWIN;
				break;
		}
		ui->printInfo(_str); 
		return true;
	} else {
		ui->printErrMessage(STR_ERROR_NOGAME);
		return false;
	} 
}


// Sets up a new board with size arg1 \in [1,15].
// Cleans and resets all data structures. 

bool YController::boardsize(UICommand _comm) {
	unsigned int _size; 
	//Validate the parameters first.
	try {
		if (!checkIfInt(_comm.arg1)) {
			throw YException(ArgsInvalid);
		}
        //Accept boards of the correct size only.
		_size = atoi(_comm.arg1);
        if (_size <= 0 || _size > INT_MAX_BOARD_SIZE) {
			throw exception();
		}
	} catch (exception &e) {
		throw YException(ArgsInvalid);
	}
	gs->setBoard(_size);	//Set a new board. 
	if (history != NULL)	
		delete history; 
	history = new list<PlayerMove>(); 
	this->lastLegalMove = BLACK_CELL;
	this->gameInProgress = true;
	this->interactive = false;
	this->player = EMPTY_CELL;
    if (!this->ai->setParams(YAI::BoardSize, _size) ) {
        ui->printWarning(STR_WARNING_AIPARAMS);
    }
	return true; 
}

// Performs a default AI move.  

bool YController::aimove(UICommand _comm) {
	if (gameInProgress) {
        CellType _aiColour;
        if (strlen(_comm.arg1)>1) {
            throw YException(ArgsInvalid);
        }
        switch (tolower(_comm.arg1[0])) {
            case 'b':
                _aiColour = BLACK_CELL;
                break;
            case 'w':
                _aiColour = WHITE_CELL;
                break;
            default :
                YException e(NotPlayer);
                throw e;
        }
        UICommand _comm2 = generateCommAI(false, _aiColour);
		strcpy(_comm.arg2, _comm2.arg2);
        if (this->play(_comm)) {
            YAI::RunInfo _info = ai->getRunInfo();
            if (_info.isValid()) {
                ui->printInfo(_info.getMessage());
            }
        }
        return false;
        //this->result(_comm);
        //this->showboard(_comm);
	} else {
		ui->printErrMessage(STR_ERROR_NOGAME);
		return false;
	}	
}

// Prints the current board (position) on the screen. 

bool YController::showboard(UICommand _comm) {
	if(this->gameInProgress) {
		ui->drawBoard(this->gs);
		return true; 	
	}
	else {
		ui->printErrMessage(STR_ERROR_NOGAME);
		return false;  
	}
}

//This method sends the AI manager different actions based on the CL input.  

bool YController::manageAI(UICommand _comm) {
	try {
		bool _error = false;
		if(strlen(_comm.arg1) > 1) {
			_error = true;
		} else {
			switch(tolower(_comm.arg1[0])) {
				case 'n':	//Set new AI 
					_error = !initializeAI(_comm.arg2); 
					break;
				case 'p':
					_error = !sendParamsToAI(_comm.arg2);
					break;
				case 's':
					_error = !ai->serializeData();
					if (!_error) 
						ui->printInfo(STR_INFO_SERIALIZED); 
					else
						ui->printErrMessage(STR_ERROR_SERIALIZED); 
					break;	
				case 'd': 
					_error = !ai->deSerializeData();
					if (!_error) 
						ui->printInfo(STR_INFO_DESERIALIZED); 
					else 
						ui->printErrMessage(STR_ERROR_SERIALIZED); 
                    break;
                case 't':
                    _error = !tournament(_comm.arg2);
                    if (!_error)
                        ui->printInfo(STR_INFO_TOURNAMENT);
                    else
                        ui->printErrMessage(STR_ERROR_TOURNAMENT);
                    break;
				default:
					_error = true; 
			}
		}
		if (_error) throw YException();
	}
	catch(exception &e) {
		throw YException(ArgsInvalid); 
	}
	return true;
}

//This method invokes directly the corresponding AI method to set certain parameters. 

bool YController::sendParamsToAI(const char * _param) {
	switch(tolower(_param[0])) {
		case 'i':
			if (checkIfDouble(&_param[1])) 
				return ai->setParams(YAI::Iterations, atof(&_param[1]));
			return false; 
		case 'b': 
			if (checkIfInt(&_param[1]))
				return ai->setParams(YAI::BoardSize, atof(&_param[1]));
			return false;
		case 'd':
			if (checkIfInt(&_param[1]))
				return ai->setParams(YAI::Id, atof(&_param[1])); 
			return false; 
        case 'o':
            if (checkIfInt(&_param[1]))
                return ai->setParams(YAI::Opponent, atof(&_param[1]));
            return false;
        case 'c':
            if (checkIfInt(&_param[1]))
                return ai->setParams(YAI::Cores, atof(&_param[1]));
            return false;
        case 's':
            if(checkIfDouble(&_param[1])) {
                return ai->setParams(YAI::ExecutionTime, atof(&_param[1]));
            }
            return false;
        case 'm':
            if(checkIfInt(&_param[1])) {
                return ai->setParams(YAI::Mode, atof(&_param[1]));
            }
            return false;
	}
	return true;
}

/// This method initializes a new AI for the current board based on the type specified by the user.  

bool YController::initializeAI(const char * _aiType) {
	CellType _aiColour  = this->player == BLACK_CELL ? WHITE_CELL : BLACK_CELL;  
	unsigned _bs = this->gs != NULL ? gs->getBoardSize() : INT_DEFAULT_BOARDSIZE;  
	if(strlen(_aiType)>1) {
		return false; 
	}
    try {
        switch(_aiType[0]) {
            case 'r':
                if (this->ai) delete this->ai;
                ai = new YAI_Random();
                ui->printInfo(STR_INFO_RANDAI);
                return true;
            case 's':
                if (this->ai) delete this->ai;
                ai = new YAI_SARSA_RL(_aiColour, _bs);
                ui->printInfo(STR_INFO_RLAI);
                return true;
            case 'm':
                if (this->ai) delete this->ai;
                ai = new YAI_MCTS(_aiColour, _bs);
                ui->printInfo(STR_INFO_MCTS);
                return true;
            case 'l':
                if (this->ai) delete this->ai;
                ai = new YAI_MLMCTS(_aiColour, _bs);
                ui->printInfo(STR_INFO_MLMTCS);
                return true;
            default:
                return false;
        }
    }
    catch (exception) {
        ai = new YAI_Random(_aiColour, _bs);
        return false;
    }
}

bool YController::tournament(const char * _aiType) {
    CellType _aiColour  = this->ai->getCurrentPlayer() == BLACK_CELL ? WHITE_CELL : BLACK_CELL;
    unsigned _bs = this->gs != NULL ? gs->getBoardSize() : INT_DEFAULT_BOARDSIZE;
    if(!isalpha(_aiType[0]) && !checkIfInt(&_aiType[1])) {
        return false;
    }
    double _winRate = -1;
    switch(tolower(_aiType[0])) {
        case 'r':
    {
            YAI_Random _tempai(_aiColour, _bs);
            _winRate =ai->testAgainst(&_tempai, atoi(&_aiType[1]));
            break;
    }
    case 's':
    {       YAI_SARSA_RL _tempai(_aiColour, _bs);
            _tempai.setId(100); //Id for testing
            _tempai.deSerializeData();
            _winRate = ai->testAgainst(&_tempai, atoi(&_aiType[1]));
            break;
    }
    case 'm':
    {       YAI_MCTS _tempai(_aiColour, _bs);
            //_tempai.setId(100); //Id for testing
            //_tempai.deSerializeData();
            _tempai.setPersistence(false);
            _winRate = ai->testAgainst(&_tempai, atoi(&_aiType[1]));
            break;
    }
    default:
            return false;
    }
    if(_winRate>=0) {
        char _buffer[1024];
        sprintf(&_buffer[0], STR_INFO_TOUR_RESULTS, _winRate*100);
        ui->printInfo(_buffer);
        return true;
    }
    else {
        return false;
    }
}

bool YController::trainAI(UICommand _comm) {
	bool _result;
	char _buffer[128];
    struct timespec _start, _finish;
	_buffer[0] = '\0';
	strcat(_buffer, STR_INFO_AITRAIN);

	ui->printInfo(_buffer);

    clock_gettime(CLOCK_MONOTONIC, &_start);



    if((_result = ai->train())) {
        clock_gettime(CLOCK_MONOTONIC, &_finish);

        double _time = YAI::getElapsedSeconds(_start, _finish);
        ui->printInfo(STR_INFO_AITRAINDONE);

        sprintf(&_buffer[0], STR_INFO_AITRAIN_TIME, _time);
        ui->printInfo(_buffer);
		return true; 
	} else {
		ui->printErrMessage(STR_ERROR_AIFAIL);
		return false; 
	}
	
}

// Class destructor.

YController::~YController() {
	if (ui) delete ui;
	if (gs) delete gs;
	if (ai) delete ai;
	if (history)
		delete history;  
}

// Constructor: Initializes all data structures, the UI, AI and the gamestate. 

YController::YController() {
	this->history = NULL; 
	this->lastLegalMove = EMPTY_CELL;
	this->gameInProgress = false;  
	this->exit = false;
	this->player = EMPTY_CELL;
	this->interactive = false; 
	ui = new YGUI();
	gs = new GameState(); 
	ai = new YAI_Random();		//Default AI
	//Set the array of commands. 
	commands[0] = &YController::showboard;
	commands[1] = &YController::listCommands; 
	commands[2] = &YController::boardsize;
	commands[3] = &YController::hex; 
	commands[4] = &YController::play;
	commands[5] = &YController::result; 
	commands[6] = &YController::undo;
	commands[7] = &YController::quit; 
	commands[8] = &YController::aimove;
	commands[9] = &YController::playFull;
	commands[10] = &YController::trainAI; 
	commands[11] = &YController::manageAI;
}

// Public method, starts the game engine. 

void YController::startYGame() {
	this->mainMenuCycle();
}

// Loop that executes until the player decides to quit.
// Read commands and sends them to the appropriate handler. 

bool YController::mainMenuCycle() {
        ui->printWelcome();
        ui->printCommands();
        UICommand _command;
        do {
            try {
                _command = ui->readCommand(interactive);
                (*this.*commands[_command.commandNo])(_command);	//Calls the appropriate command.
            }
            catch (YException&e) {
                if (interactive && e.getType()==NoMoves) {
                    this->result(_command);
                    this->showboard(_command);
                }
                ui->printErrMessage(e.what());
            }
            catch (exception &e) {
                ui->printErrMessage(STR_EXCEPTION_GENERAL);
            }
        }
        while (!exit);
        return true;
}
