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
  * Abstract:	Assignment 2 - This file contains the 
  *		implementation for the GameState class. 
  * ========================================================= 
*/ 

#include "ygamestate.h"

using namespace ARS; 

// Class destructor

GameState::~GameState() {
	if (gameBoard) {
		delete [] gameBoard; 
	}
}

//Returns a list with all the empty cells in the board.

list<Coords> GameState::getEmptyCells() const {
    list<Coords> _result(this->freeMoves);
    return _result;
}

list<Coords> *GameState::getEmptyCellsPtr() {
    return &this->freeMoves;
}

//Constructor. Receives the size of the board. 

GameState::GameState(unsigned int __size) {
    this->initialize(__size, true);
}

//Constructor - copy.

GameState::GameState(const GameState &__state) {
    this->initialize(__state.boardSize, true);
	int _temp = (boardSize*(boardSize+1)) / 8; 
	_temp += (boardSize * (boardSize+1)) % 8 ? 1 : 0;
	memcpy((void*)this->gameBoard, (void*)__state.gameBoard, _temp);
    this->freeMoves.clear();
    this->freeMoves= __state.getEmptyCells();

    //this->freeMoves.splice(freeMoves.begin(), temp);
}


//This function crops a sub-area (triangle) of the board
GameState * GameState::crop(Coords _startCell, unsigned __boardSize) {
    GameState * _result = new GameState(__boardSize);
    //Validate that the dimensions are OK
    if(((_startCell.x + (__boardSize-1)) > (this->boardSize)) || ((_startCell.y + (__boardSize-1)) > (this->boardSize))) {
        return NULL;
    }
    unsigned _newX = 1, _newY = 1;
    for (int y = _startCell.y; y<= (_startCell.y+__boardSize-1); ++y) {
        _newX = 1;
        for (int x = _startCell.x ; x<= (_startCell.x + __boardSize - _newY); ++x) {
            _result->markCell(_newX, _newY, this->getCell(x,y));
            _newX++;
        }
        _newY++;
    }
    return _result;
}


//Carries a Y reduction at level X

GameState * GameState::reduceAtLevel(unsigned __level) {
    int _iReduc = boardSize;
    GameState * _current = new GameState(*this);
    GameState * _new = new GameState(_iReduc-1);
    while (--_iReduc >= __level) {
        for (int i = 1; i<= _iReduc; i++) {
            for (int j = 1; j<= _iReduc -i+1; j++) {
                CellType _result = EMPTY_CELL, _a1, _a2, _a3;
                _a1 = _current->getCell(i,j);
                _a2 = _current->getCell(i,j+1);
                _a3 = _current->getCell(i+1,j);
                if (_a1==WHITE_CELL) {
                    if(_a2 == WHITE_CELL || _a3 == WHITE_CELL) {
                        _result = WHITE_CELL;
                    }
                    else if (_a2==BLACK_CELL && _a3 == BLACK_CELL) {
                        _result = BLACK_CELL;
                    }
                }
                else if (_a1 == BLACK_CELL) {
                    if(_a2 == BLACK_CELL || _a3 == BLACK_CELL) {
                        _result = BLACK_CELL;
                    }
                    else if (_a2==WHITE_CELL && _a3 == WHITE_CELL) {
                        _result = WHITE_CELL;
                    }
                } else {
                    if(_a2 == BLACK_CELL &&  _a3 == BLACK_CELL) {
                        _result = BLACK_CELL;
                    }
                    else if (_a2==WHITE_CELL && _a3 == WHITE_CELL) {
                        _result = WHITE_CELL;
                    }
                }
                _new->markCell(i,j,_result);
            }
        }

        delete _current;
        _current = _new;
        _new = new GameState(_iReduc-1);
    }
    delete _new;
    return _current;
}

/*!
 * Adding this functionality to test the multi level paradigm
 */
/*
GameState * GameState::reduceTo5() {
    GameState * _new = new GameState(5);
    int _yoffset = 1;
    for(int j =1; j<=5; ++j) {
        int _xoffset = 1;

        for (int i = 1; i<=5-j; ++i) {
            CellType _result = EMPTY_CELL, _a1, _a2, _a3;
            if (j%2) {
                //Odd row, 1,3,5
                _a1 = this->getCell(_xoffset,_yoffset);
                _a2 = this->getCell(_xoffset+1,_yoffset);
                _a3 = this->getCell(_xoffset,_yoffset+1);
            }
            else {
                //Even rows 0,2,4
                _a1 = this->getCell(_xoffset+1,_yoffset-1);
                _a2 = this->getCell(_xoffset,1+_yoffset-1);
                _a3 = this->getCell(1+_xoffset,_yoffset);
            }
            _xoffset +=2;
            if (_a1==WHITE_CELL) {
                if(_a2 == WHITE_CELL || _a3 == WHITE_CELL) {
                    _result = WHITE_CELL;
                }
                else if (_a2==BLACK_CELL && _a3 == BLACK_CELL) {
                    _result = BLACK_CELL;
                }
            }
            else if (_a1 == BLACK_CELL) {
                if(_a2 == BLACK_CELL || _a3 == BLACK_CELL) {
                    _result = BLACK_CELL;
                }
                else if (_a2==WHITE_CELL && _a3 == WHITE_CELL) {
                    _result = WHITE_CELL;
                }
            } else {
                if(_a2 == BLACK_CELL &&  _a3 == BLACK_CELL) {
                    _result = BLACK_CELL;
                }
                else if (_a2==WHITE_CELL && _a3 == WHITE_CELL) {
                    _result = WHITE_CELL;
                }
            }
            _new->markCell(i,j,_result);
        }
        _yoffset +=2;
    }
    return _new;
}
*/

//More efficient reduce with no additional heap allocation.

CellType GameState::fullReduce() {
    int _iReduc = boardSize;
    CellType _result = this->getCell(1,1);
    list<Coords> _toMarkWhite, _toMarkBlack, _toMarkEmpty;
    //GameState * _new = new GameState(_iReduc-1);
    GameState _playground(*this);
    while (--_iReduc >= 1) {

        for (int x = 1; x<= _iReduc; x++) {
            for (int y = 1; y<= _iReduc -x+1; y++) {
                CellType _result = EMPTY_CELL, _a1, _a2, _a3;
                _a1 = _playground.getCell(x,y);
                _a2 = _playground.getCell(x,y+1);
                _a3 = _playground.getCell(x+1,y);
                if (_a1==WHITE_CELL) {
                    if(_a2 == WHITE_CELL || _a3 == WHITE_CELL) {
                        _result = WHITE_CELL;
                    }
                    else if (_a2==BLACK_CELL && _a3 == BLACK_CELL) {
                        _result = BLACK_CELL;
                    }
                }
                else if (_a1 == BLACK_CELL) {
                    if(_a2 == BLACK_CELL || _a3 == BLACK_CELL) {
                        _result = BLACK_CELL;
                    }
                    else if (_a2==WHITE_CELL && _a3 == WHITE_CELL) {
                        _result = WHITE_CELL;
                    }
                } else {
                    if(_a2 == BLACK_CELL &&  _a3 == BLACK_CELL) {
                        _result = BLACK_CELL;
                    }
                    else if (_a2==WHITE_CELL && _a3 == WHITE_CELL) {
                        _result = WHITE_CELL;
                    }
                }
                if (_result == BLACK_CELL) {
                    _toMarkBlack.push_front(Coords(x,y));
                } else if (_result == WHITE_CELL){
                    _toMarkWhite.push_front(Coords(x,y));
                } else {
                    _toMarkEmpty.push_front(Coords(x,y));
                }
            }
        }
        for(list<Coords>::iterator I = _toMarkEmpty.begin(), E= _toMarkEmpty.end(); I!=E; ++I) {
            _playground.markCell((*I).x, (*I).y, EMPTY_CELL);
        }
        _toMarkEmpty.clear();
        for(list<Coords>::iterator I = _toMarkWhite.begin(), E= _toMarkWhite.end(); I!=E; ++I) {
            _playground.markCell((*I).x, (*I).y, WHITE_CELL);
        }
        _toMarkWhite.clear();
        for(list<Coords>::iterator I = _toMarkBlack.begin(), E= _toMarkBlack.end(); I!=E; ++I) {
            _playground.markCell((*I).x, (*I).y, BLACK_CELL);
        }
        _toMarkBlack.clear();
    }
    return _playground.getCell(1,1);
}

//Initializes the GameState data structures to a default size = 8.
//	The byte vector is empty.
//	The list of free cells has all cells at this time. 

void GameState::initialize(unsigned int __size, bool __constructor) {
	int _temp = (__size*(__size+1)) / 8; 
	_temp += (__size * (__size+1)) % 8 ? 1 : 0;
    if (!__constructor) delete [] this->gameBoard;
	this->gameBoard = new byte[_temp]; 
	this->boardSize = __size;
    freeMoves.clear();
	for (int i =0; i<_temp; i++) {
		gameBoard[i] &= 0x00;  
	}
	for (unsigned i =1; i<=__size; i++) {
		for (unsigned j =1; j<=__size-i+1; j++) {
			Coords _coords; 
			_coords.x= i;
			_coords.y = j;
            freeMoves.push_front(_coords);
		}
	}
}


//Transforms a bidimensional set of coordinates to a unidimensional set.
//	This also validates that the coordinates are valid in the board.
//  Both x and y are 1-based, but the result is zero based.
unsigned int GameState::getAbsPos(unsigned int __x, unsigned int __y) const {
	__y -= 1;
	__x -= 1; 
    unsigned int _offset = (boardSize *(boardSize+1))/2 - (((boardSize-__y) *(boardSize-__y+1))/2); // This formula calculates the offset
    _offset += __x;
    if ((_offset >= (boardSize*(boardSize+1))/2) || (__x >= boardSize - __y) || (__y >= boardSize)) {
		throw YException(BadCoordinates);
	}
	return _offset;
}

//Offset is the value 0...n-1 in the board.
Coords GameState::getCoordPair(unsigned int __offset) const {
    if (__offset >= ((boardSize)*(boardSize+1)/2)) {
        throw YException(BadCoordinates);
    }
    __offset++;
    unsigned _y =1, _x;
    while (__offset > (boardSize-_y+1)) {
        __offset -= boardSize - _y+1;
        _y++;
	} 
    _x = __offset;
	return Coords(_x,_y);
}


//Marks a cell as __type. This method calculates the offset and 
//	rotates bytes accordingly. 

bool GameState::markCell(unsigned int __x, unsigned int __y, CellType __type) {
    //We don't allow neither of these to be zero.
    bool _result = true;
    if (__x == 0 || __y==0) {
        return false;
    }
	unsigned int _offset = getAbsPos(__x, __y); 

	unsigned int _iByte, _iBits;

    Coords _temp(__x, __y);
    //Update our list of empty cells and validate that the cell is free.
    list<Coords>::iterator findIter = std::find(freeMoves.begin(), freeMoves.end(), _temp);
    if (__type!=EMPTY_CELL && findIter!= freeMoves.end()) {
        freeMoves.erase(findIter);
    }
    else if (__type==EMPTY_CELL && findIter==freeMoves.end()) {
        freeMoves.push_front(_temp);
    } else {
        //The cell is taken.
        _result = false;
    }

	byte _clearmask = 0xFC; //A mask to apply our new value
	byte _mask = __type; 

	_iByte = _offset / 4; 	//Calculate the per byte offset and per bit.
	_iBits = _offset % 4;
	
	_clearmask = (_clearmask << _iBits*2) | (_clearmask >> (8 - _iBits*2)) ; //We create a mask of the form 111 ...1 00 1... 111
	_mask = (_mask << _iBits*2) | (_mask >> (8 - _iBits*2));			//We move the type to the position in the byte. 
	
	gameBoard[_iByte] &= _clearmask;	//We reset whatever was there before.
	gameBoard[_iByte] |= _mask;		//We add our new value. 


    return _result;
}

//Set the board as a n x m HEx. It also validates that the board can
// 	support this confiiguration. 

void GameState::setHex(unsigned int __m, unsigned int __n) {
	unsigned int _counter = 0;
	if (boardSize-__m+1<__n) {
		throw YException(ArgsInvalid);
	}
	for(unsigned _iRow = 1; _iRow <= __m; _iRow++) {
		for (unsigned _iCol = __n + 1; _iCol <= boardSize-_iRow+1; _iCol++) {
			this->markCell(_iCol, _iRow, BLACK_CELL); 
			_counter++;
		}
	}
	for (unsigned _iRow = __m+1; _iRow <= boardSize; _iRow++) {
		for (unsigned _iCol = 1; _iCol <= boardSize-_iRow+1; _iCol++) {
			this->markCell(_iCol, _iRow, WHITE_CELL); 
			_counter++;
		}
	}
}

//Return the type inside a cell using masking and shifting.  

CellType GameState::getCell(unsigned int __x, unsigned int __y) { 
	unsigned int _offset = getAbsPos(__x, __y); 
	unsigned int _iByte, _iBits;
	byte _mask = 0x03, _result = 0x00;

	_iByte = _offset / 4; 
	_iBits = _offset % 4;
	
	_mask = (_mask << _iBits*2) | (_mask >> (8 - _iBits*2));
	
	_result = _mask & gameBoard[_iByte];
	_result >>= _iBits*2;
	return static_cast<CellType>((int)_result);  
}



