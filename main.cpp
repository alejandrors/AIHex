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
  * Abstract:	Assignment 4.5 - A far more complex Y player
  *             that incorporates reinforcement learning.
  * ========================================================= 
*/ 

// Include section


#include "ycontroller.h"

using namespace ARS; 


/** MAIN
  * ARGS: 	N/A
  * DESC:	Main program entry: creates a Controller class 
	 	and starts executiion.
  * RETR:	Error code. 
*/

int main (int argc, char ** argv) {
        YController y;
        y.startYGame();
        return 0;
}
