//B first attacks the escort ships that can attack B first
#ifndef PART2A_H
#define PART2A_H

#include "structures.h"

void simulatePart2A(Battleship *B, EscortShip E[], int N,double D);
void determineAttackOrder(Battleship *B,  EscortShip E[], int N,  int attackOrder[],int *attackCount);
void simulatePart2A_Part1A(Battleship *B, EscortShip E[],int N,double reloadTime);
void simulatePart2A_Part1B(Battleship *B, EscortShip E[], int N, double D, double reloadTime);
void simulatePart2A_Part1C(Battleship *B, EscortShip E[], int N, double D, double reloadTime);
int simulatePart2A_Part1BStep(Battleship *B,EscortShip E[],int N, int iteration, double reloadTime, int *totalSunk,const char filename[]);
void simulatePart2A_Part1C(Battleship *B, EscortShip E[], int N, double D, double reloadTime);
int simulatePart2A_Part1CStep(Battleship *B, EscortShip E[], int N, int iteration, double reloadTime, int *totalSunk, int fired[],int simulationNumber);

#endif