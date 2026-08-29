#ifndef PART1B_H
#define PART1B_H

#include "structures.h"
void generatePath(Position path[], int k, double D);

void simulatePart1B(Battleship *B,EscortShip E[],int N,double D);

int simulatePart1BStep(Battleship *B,EscortShip E[],int N,int iteration);//one battle step at one battleship position.return 1 if B is destroyed or 0 if B survived
#endif