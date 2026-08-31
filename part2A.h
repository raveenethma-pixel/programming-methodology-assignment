//B first attacks the escort ships that can attack B first
#ifndef PART2A_H
#define PART2A_H

#include "structures.h"

void simulatePart2A(Battleship *B, EscortShip E[], int N);

void determineAttackOrder(Battleship *B,  EscortShip E[], int N,  int attackOrder[],int *attackCount);

#endif