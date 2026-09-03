/*
#ifndef PART1B_H
#define PART1B_H

#include "structures.h"
void generatePath(Position path[], int k, double D);

void simulatePart1B(Battleship *B,EscortShip E[],int N,double D);

int simulatePart1BStep(Battleship *B,EscortShip E[],int N,int iteration);//one battle step at one battleship position.return 1 if B is destroyed or 0 if B survived

int simulatePart1BSim2Step(Battleship *B,EscortShip E[],int N,int iteration);
#endif
*/
#ifndef PART1B_H
#define PART1B_H

#include "structures.h"

void generatePath(Position path[],int k,double D);
void simulatePart1B(Battleship *B, EscortShip E[],int N,double D);
int runPart1BIteration(Battleship *B,EscortShip E[],int N,int iteration,int simulationNumber);
void getJamSettings(int k,int *t,double *jammedAngleMin);
int getMovementPoints(const char label[]);

#endif