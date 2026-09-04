#ifndef PART2B_H
#define PART2B_H

#include "structures.h"
#include "battle_engine.h"

void getAdvancedReloadSettings( AdvancedBattleRules *rules);
void simulatePart2B( Battleship *B,EscortShip E[],int N,double D);

#endif