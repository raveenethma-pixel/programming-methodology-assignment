#ifndef PART1C_H
#define PART1C_H

#include "structures.h"

void simulatePart1C(Battleship *B,EscortShip E[],int N);
void simulatePart1CMovement(Battleship *B,EscortShip E[],int N,double D);
/*
int simulatePart1CMovementStep(Battleship *B,EscortShip E[],int N,int iteration,int *totalSunk,int fired[]);//return 1 if damage to B is 100 and 0 if not 100%
int simulatePart1CMovementSim2Step(Battleship *B, EscortShip E[], int N, int iteration, int *totalSunk,int fired[]);
*/
int runPart1CMovementIteration( Battleship *B,  EscortShip E[], int N, int iteration, int *totalSunk, int fired[],int simulationNumber);
#endif