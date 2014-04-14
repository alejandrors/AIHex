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
		implementation
  * ========================================================= 
*/ 

#include "yexception.h"

using namespace ARS; 


YException::~YException() throw ()  {
}

YException::YException() {
	this->message[0] = '\0';
}

YException::YException(ExceptionType _t) {
	switch (_t) {
		case CommandInvalid:
			strcpy(message,STR_EXCEPTION_COMMAND);
			break;
		case CommandNumArgs:
			strcpy(message,STR_EXCEPTION_ARGS);
			break;
		case BadCoordinates: 
			strcpy(message,STR_EXCEPTION_COORDS);
			break;
		case NotPlayer:
			strcpy(message,STR_EXCEPTION_NOTPLAY);
			break;
		case ArgsInvalid:
			strcpy(message,STR_EXCEPTION_ARGS);
			break;
        case MCTSTimeOut:
            strcpy(message,STR_EXCEPTION_MCTSTO);
            break;
		case NoMoves: 
			strcpy(message,STR_GAME_NOMOVE);
			break; 
	}
	this->type = _t;
}


const char * YException::what() const throw () {
	return this->message; 
}

const char * YException::getMessage() {
	char * buffer = new char[strlen(message)];
	strcpy(buffer, message);
	return buffer; 
}

void YException::setMessage(const char * _m) {
	strcpy(message,_m); 
}

ExceptionType YException::getType() {
	return this->type; 
}

void YException::setType(ExceptionType _t) {
	this->type = _t; 
}
