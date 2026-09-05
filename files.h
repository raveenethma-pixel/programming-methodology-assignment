#ifndef FILES_H
#define FILES_H
#include "battle_engine.h"


void saveInitialConditions(Battleship *B, EscortShip E[], int N, double D);
void saveFinalConditions(Battleship *B,EscortShip E[],int N,int sinkingEscort,double earliestEscortHitTime,int sunkCount);
void savePart1BSim1Iteration(Battleship *B,EscortShip E[],int N,int iteration,int sinkingEscort,double earliestEscortHitTime,int sunkThisIteration,double iterationEndTime);
void savePart1BSim2Iteration(Battleship *B,EscortShip E[],int N,int iteration,int sinkingEscort,double earliestEscortHitTime,int sunkThisIteration,double iterationEndTime,int gunJammed);
void savePart1CResults(Battleship *B,EscortShip E[],int N,double escortHitTimes[], double battleHitTimes[],int sunkCount,double battleEndTime);
void startPart1CMovementResults(Position path[], int k);
void savePart1CMovementIteration(Battleship *B, EscortShip E[], int N, int iteration, double escortHitTimes[],  double battleHitTimes[],int sunkThisIteration);
void startPart1CMovementSim2Results(Position path[],int k,int t,double jammedAngleMin);
void savePart1CMovementSim2Iteration(Battleship *B,EscortShip E[],int N, int iteration, double escortHitTimes[],double battleHitTimes[],int sunkThisIteration);
void savePart2APart1AResults( Battleship *B,EscortShip E[],int N,double reloadTime,BattleDetails *details, BattleResult result);
void startPart2APart1BResults(const char filename[], Position path[], int k, double reloadTime, int t, double jammedAngleMin);
void savePart2APart1BIteration(const char filename[],Battleship *B,EscortShip E[],int N,int iteration, BattleDetails *details,BattleResult result);
void startPart2APart1CResults( Position path[], int k, double reloadTime);
void savePart2APart1CJamSettings(int t,double jammedAngleMin);
void savePart2APart1CIteration(Battleship *B, EscortShip E[], int N,int iteration,int simulationNumber, BattleDetails *details,BattleResult result);
void startPart2BResults(AdvancedBattleRules *rules);
void savePart2BPart1AResults(Battleship *B,EscortShip E[],BattleDetails *details,BattleResult result);
void savePart2BPart1BSetup(Position path[],int k,int simulationNumber,int jamAfter,double jammedAngleMin);
void savePart2BPart1BIteration(Battleship *B,EscortShip E[],int iteration, BattleDetails *details,BattleResult result);
void savePart2BPart1CResult(const char title[],Battleship *B,EscortShip E[],int N,BattleDetails *details,BattleResult result,int escortHitCount[]);
void displayTextFile(const char filename[]);
#endif