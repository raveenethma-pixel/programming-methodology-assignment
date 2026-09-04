#ifndef BATTLE_ENGINE_H
#define BATTLE_ENGINE_H

#include "structures.h"
#define MAX_ESCORTS 100
#define ESCORT_TYPE_COUNT 5
//Rules that can change between assignment parts.

void determineAttackOrder(Battleship *B,EscortShip E[],int N, int attackOrder[],int *attackCount);
typedef struct {
    int cumulativeDamage;
    int useBattleshipReload;
    double battleshipReloadTime;

} BattleRules;


//Shared rules for Part 2-B and Part 2-C.

typedef struct {
    // Part 2-A rule still applies.
    double battleshipReloadTime;
    // Part 2-B: one reload time for each E type.
    double escortReloadTime[ESCORT_TYPE_COUNT];
    // 0 for A/B style, 1 for C style.
    
} AdvancedBattleRules;


//Results produced by one battle simulation.
typedef struct {
    int battleshipDestroyed;

    int sinkingEscort;

    int sunkCount;

    double battleEndTime;

    double lastBattleshipHitTime;
} BattleResult;


//Simulates one battle at the current positions of B and the escort ships.
BattleResult simulateBattleStep(Battleship *B,EscortShip E[],int N,BattleRules rules,int fired[], double escortHitTimes[],double battleHitTimes[],int attackOrder[],int attackCount,double battleshipFireTimes[]);
void resetBattlefield(Battleship *B,EscortShip E[],Battleship originalB,EscortShip originalE[],int N);

typedef struct {
    int attackOrder[MAX_ESCORTS];
    int attackCount;
    double escortHitTimes[MAX_ESCORTS];
    double fireTimes[MAX_ESCORTS];
    double hitTimes[MAX_ESCORTS];
} BattleDetails;

BattleResult runReloadBattle( Battleship *B, EscortShip E[],int N, double reloadTime,int cumulativeDamage,int fired[],BattleDetails *details);
#endif
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