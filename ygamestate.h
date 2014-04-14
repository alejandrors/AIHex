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
  * Abstract:	Assignment 2 - This class defines a container
  *		and a set of operations for using a bit vector
  *		to represent and manipulate the game state.
  * ========================================================= 
*/ 


#ifndef Y_GAMESTATE_H
#define Y_GAMESTATE_H

#include "yconstants.h"
#include "yexception.h"
#include <cstdio>
#include <iostream>
#include <list>
#include <algorithm>

using namespace std;
typedef unsigned char byte; 

/** CLASS MODEL
  *
  * This class provides a data persistence model for the Y board game. It is 
  * implemented using a bit vector with two bits for each position. The
  * bits are stored in <gameBoard> while <sizeBoard> is the number of 
  * actual positions in the Y board. Three cases:
  * 
  * 1) 00 = EMPTY
  * 2) 01 = BLACK
  * 3) 10 = WHITE
  * 
  * 
*/

namespace ARS {

	//> The different types of cells.
	enum CellType {EMPTY_CELL, BLACK_CELL, WHITE_CELL}; 
	//> A data structure for coordinates
	struct Coords {
		Coords() : x(0), y(0) {}
		Coords(const Coords &__other) {*this=__other;}  
		Coords(int _x, int _y) : x(_x), y(_y) {}
		int x; 
		int y;
		bool operator==(const Coords &__other) const {return (this->x == __other.x && this->y == __other.y);}
		bool operator!=(const Coords &__other) const {return !(*this == __other);}
		Coords& operator=( const Coords& __other ) {this->x = __other.x; this->y = __other.y; return *this;}
        bool isValid() {return (x>0 && y>0);}
	};

    //A structure to represent player moves. x, y \in [1,boardSize]
    struct PlayerMove {
        PlayerMove(): type(EMPTY_CELL), x(0), y(0){}
        PlayerMove(Coords _m, CellType _c) { this->set(_m,_c);}
        CellType type;
        unsigned int x;
        unsigned int y;
        void set(Coords _m, CellType _c) { x=_m.x; y =_m.y; type=_c; }
        bool isValid() {return (x>0 && y>0);}
    };

	class GameState {
	
	private:
		unsigned int boardSize;		//> The size of the board
        list<Coords> freeMoves;	//> A list with the set of free cells. It is keep along the board for faster seek times of empty cells.
		byte * gameBoard; 		//> The byte array that contains the board. 
		
        void initialize(unsigned int, bool);

	public:
        //Constructors
		GameState(const GameState& ); 
		GameState(unsigned int _size = INT_DEFAULT_BOARDSIZE); 
		~GameState(); 

        //Crops a triangle in the board
        GameState * crop(Coords _startCell, unsigned __boardSize);

        // Y-reductions
        //GameState * reduceTo5();
        GameState * reduceAtLevel(unsigned) ;
		CellType fullReduce(); 

        //Cell operations
		bool markCell(unsigned int, unsigned int, CellType); 			
		CellType getCell(unsigned int, unsigned int); 	

        //List of empty cells for easier access, especially in large boards.
        list<Coords> getEmptyCells() const;
        list<Coords> * getEmptyCellsPtr() ;
        //Checks if the board is full.
        bool isFull() { return (getEmptyCells().size() == 0); }
        //Resets the board and sets it to a new size and/or gets the size.
		unsigned int getBoardSize() {return boardSize;}
        void setBoard(unsigned int _n) {initialize(_n, false);}

        //Gets a coordinates par (x,y) in [1,+inf[ for a absolute value in [0,boardCells-1]
        Coords getCoordPair(unsigned int) const;
        //Returns the absolute value of a coordinates pair, the value in [0, boardCells-1]
        unsigned int getAbsPos(unsigned int, unsigned int) const;
        unsigned int getAbsPos(Coords _c) const { return getAbsPos(_c.x,_c.y); }


		void setHex(unsigned int, unsigned int); 
	};

}

#endif
