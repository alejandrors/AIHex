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
  * Abstract:	Assignment 2 - A basic exception handler
		definition
  * ========================================================= 
*/ 

#ifndef Y_EXCEPTION_HANDLER
#define Y_EXCEPTION_HANDLER

#include <exception>
#include <cstdio>
#include <cstring>
#include "yconstants.h"

using namespace std; 

/** CLASS MODEL 
  * 
  * Class model for the custom exception handler used in this Y Game
**/

namespace ARS {
	//> Different exception types -- refer to constants.cpp for a description of each.
    enum ExceptionType  {NoMoves, CommandInvalid, CommandNumArgs, BadCoordinates, NotPlayer, ArgsInvalid, MCTSTimeOut};

	class YException : public exception {
		private:
			char message[256]; 	//> Message to be displayed as the error message.
			ExceptionType type; 	//> The type of exception. 
		public:
			explicit YException();
			YException(ExceptionType _t);
 
			void setType(ExceptionType _t);
			ExceptionType getType();
			
			void setMessage(const char * _m);
			const char * getMessage ();
 
			virtual ~YException() throw (); 
			virtual const char* what() const throw();
	};
}

#endif

