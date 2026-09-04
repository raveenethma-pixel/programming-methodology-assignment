#ifndef ESCORT_H
#define ESCORT_H
#include "structures.h"//The compiler needs to know what EscortShip is before it can use it as a function
void initializeEscortShips(EscortShip E[],int N,double D, double battleVMax);
int getEscortTypeIndex(const char type[]);
#endif 
//EscortShip - array of escort ships
//battleVMax - maximum shell velocity of the battleship