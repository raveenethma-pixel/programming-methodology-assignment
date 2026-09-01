#ifndef BATTLE_ENGINE_H
#define BATTLE_ENGINE_H

#include "structures.h"
#define MAX_ESCORTS 100

//Rules that can change between assignment parts.

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
BattleResult simulateBattleStep(Battleship *B,EscortShip E[],int N,BattleRules rules,int fired[], double escortHitTimes[],double battleHitTimes[],int attackOrder[],int attackCount,double battleshipFireTimes[]);
void resetBattlefield(Battleship *B,EscortShip E[],Battleship originalB,EscortShip originalE[],int N);
typedef struct {
    int attackOrder[MAX_ESCORTS];
    int attackCount;

    double escortHitTimes[MAX_ESCORTS];
    double fireTimes[MAX_ESCORTS];
    double hitTimes[MAX_ESCORTS];
} BattleDetails;
/*
this file handles,
can E hit B?
when does E hit B?
can B hit E?
when does B hit E?
which E ships sink?
does B sink?
*/
/*
attackOrder[] = which escorts B should attack and in what order

attackCount = how many escorts are in that order

battleshipFireTimes[] = records when B fired at each escort*/
#endif


/*
initialize result
        ↓
initialize optional arrays
        ↓
Escort → B calculations
        ↓
Is B reload enabled?
   ↙                 ↘
YES                  NO
Part 2-A             Part 1
attackOrder[]        normal loop
reload time
   ↘                 ↙
 determine result
        ↓
 return result
 */