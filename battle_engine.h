#ifndef BATTLE_ENGINE_H
#define BATTLE_ENGINE_H

#include "structures.h"

/*
   Rules that can change between assignment parts.
*/
typedef struct {
    int cumulativeDamage;

    int useBattleshipReload;
    double battleshipReloadTime;

} BattleRules;


/*
   Results produced by one battle simulation.
*/
typedef struct {
    int battleshipDestroyed;

    int sinkingEscort;

    int sunkCount;

    double battleEndTime;

    double lastBattleshipHitTime;
} BattleResult;


/*
   Simulates one battle at the current positions
   of B and the escort ships.
*/
BattleResult simulateBattleStep(Battleship *B,EscortShip E[],int N,BattleRules rules,int fired[]);
/*
this file handles,
can E hit B?
when does E hit B?
can B hit E?
when does B hit E?
which E ships sink?
does B sink?
*/
#endif