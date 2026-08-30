#ifndef FILES_H
#define FILES_H

#include "structures.h"

void saveInitialConditions(Battleship *B, EscortShip E[], int N, double D);
void saveFinalConditions(Battleship *B,EscortShip E[],int N,int sinkingEscort,double earliestEscortHitTime,int sunkCount);
void savePart1BSim1Iteration(Battleship *B,EscortShip E[],int N,int iteration,int sinkingEscort,double earliestEscortHitTime,int sunkThisIteration,double iterationEndTime);
void savePart1BSim2Iteration(Battleship *B,EscortShip E[],int N,int iteration,int sinkingEscort,double earliestEscortHitTime,int sunkThisIteration,double iterationEndTime,int gunJammed);
void savePart1CResults(Battleship *B,EscortShip E[],int N,double escortHitTimes[], double battleHitTimes[],int sunkCount,double battleEndTime);
void startPart1CMovementResults(Position path[], int k);
void savePart1CMovementIteration(Battleship *B, EscortShip E[], int N, int iteration, double escortHitTimes[],  double battleHitTimes[],int sunkThisIteration);
void startPart1CMovementSim2Results(Position path[],int k,int t,double jammedAngleMin);
void savePart1CMovementSim2Iteration(Battleship *B,EscortShip E[],int N, int iteration, double escortHitTimes[],double battleHitTimes[],int sunkThisIteration);
#endif