#ifndef FILES_H
#define FILES_H

#include "structures.h"

void saveInitialConditions(Battleship *B, EscortShip E[], int N, double D);
void saveFinalConditions(Battleship *B,EscortShip E[],int N,int sinkingEscort,double earliestEscortHitTime,int sunkCount);
#endif