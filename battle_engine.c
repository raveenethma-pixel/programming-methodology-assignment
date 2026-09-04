#include <stdio.h>
#include <float.h>
#include <math.h>

#include "battle_engine.h"
#include "projectile.h"
#include "escort.h"

static double applyCumulativeEscortDamage( Battleship *B, EscortShip E[], int N, double hitTimes[], int *destroyingEscort)
{
    int processed[MAX_ESCORTS] = {0};
    *destroyingEscort = -1;
    if(B->damage >= 1.0){
        return 0.0;
    }
    for(int count = 0; count < N; count++){
        int next = -1;
        // Find the next escort shell to hit B.
        for(int i = 0; i < N; i++){
            if(processed[i] || hitTimes[i] < 0.0){
                continue;
            }
            if(next == -1 || hitTimes[i] < hitTimes[next]){
                next = i;
            }
        }
        if(next == -1){
            break;
        }
        processed[next] = 1;
        B->damage += E[next].impactPower;
        // This escort's shell pushed cumulative damage to 100%.
        if(B->damage >= 1.0){
            *destroyingEscort = next;
            return hitTimes[next];
        }
    }
    return -1.0;
}

void determineAttackOrder( Battleship *B, EscortShip E[],int N,int attackOrder[],int *attackCount)
{
    double threatTime[N];
    *attackCount = 0;
    // Find all living escorts B can attack.
    for(int i = 0; i < N; i++){
        threatTime[i] = -1.0;
        if(E[i].alive == 0){
            continue;
        }

        if(canBattleshipHitEscort(B, &E[i])){
            attackOrder[*attackCount] = i;
            (*attackCount)++;
            /*
               Escorts capable of hitting B are threats.
               Smaller hit time = higher priority.
            */
            if(canEscortHitBattleship(&E[i], B)){
                double distance = calculate_distance( E[i].x,E[i].y, B->x, B->y);
                threatTime[i] = calculateMinimumHitTime( distance, E[i].vMin, E[i].vMax, E[i].angleMin, E[i].angleMax);
            }
        }
    }
    /*
       Bubble sort the attack order.

       Priority:
       1. Escorts capable of hitting B come first.
       2. Among those, the one that can hit B sooner comes first.
       3. Escorts that cannot hit B come afterwards.
    */

    // Sort threats first, earliest threat first.
    for(int i = 0; i < *attackCount - 1; i++){
        for(int j = 0; j < *attackCount - i - 1; j++){
            int first = attackOrder[j];
            int second = attackOrder[j + 1];
            int swap = 0;

            if(threatTime[first] < 0.0 && threatTime[second] >= 0.0){
                swap = 1;
            }
            else if(threatTime[first] >= 0.0 && threatTime[second] >= 0.0 && threatTime[first] > threatTime[second]){
                swap = 1;
            }
            if(swap){
                int temp = attackOrder[j];
                attackOrder[j] = attackOrder[j + 1];
                attackOrder[j + 1] = temp;
            }
        }
    }
}

BattleResult runReloadBattle( Battleship *B, EscortShip E[], int N, double reloadTime,int cumulativeDamage, int fired[], BattleDetails *details)
{
    double engineFireTimes[N];
    double engineHitTimes[N];
    determineAttackOrder( B, E, N, details->attackOrder, &details->attackCount);

    BattleRules rules;
    rules.cumulativeDamage = cumulativeDamage;
    rules.useBattleshipReload = 1;
    rules.battleshipReloadTime = reloadTime;

    BattleResult result = simulateBattleStep( B, E, N, rules,fired, details->escortHitTimes, engineHitTimes, details->attackOrder, details->attackCount, engineFireTimes);
    //Convert engine arrays from escort-index order into B's attack-order order.
    for(int i = 0; i < N; i++){
        details->fireTimes[i] = -1.0;
        details->hitTimes[i] = -1.0;
    }
    for(int a = 0; a < details->attackCount; a++){
        int index = details->attackOrder[a];
        details->fireTimes[a] = engineFireTimes[index];
        details->hitTimes[a] = engineHitTimes[index];
    }
    return result;
}

BattleResult simulateBattleStep( Battleship *B, EscortShip E[], int N, BattleRules rules, int fired[],double escortHitTimes[],double battleHitTimes[],int attackOrder[],int attackCount,double battleshipFireTimes[])
{
    BattleResult result;

    result.battleshipDestroyed = 0;
    result.sinkingEscort = -1;
    result.sunkCount = 0;
    result.battleEndTime = 0.0;
    result.lastBattleshipHitTime = 0.0;

    if(escortHitTimes != NULL){
        for(int i = 0; i < N; i++){
            escortHitTimes[i] = -1.0;
        }
    }

    if(battleHitTimes != NULL){
        for(int i = 0; i < N; i++){
                battleHitTimes[i] = -1.0;
            }
        }
    if(battleshipFireTimes != NULL){
        for(int i = 0; i < N; i++){
            battleshipFireTimes[i] = -1.0;
        }
    }

    double earliestEscortHitTime = -1.0;
    double cumulativeDestroyTime = -1.0;
    double pendingEscortHitTimes[N];

    for(int i = 0; i < N; i++){
        pendingEscortHitTimes[i] = -1.0;
    }
    
    //Escort attack B
    // First find the earliest escort shell that can hit B.
    for(int i = 0; i < N; i++){
        //E[i].alive tells us whether a specific escort ship is still alive or already destroyed.
        if(E[i].alive == 0){
            continue;
        }
        //  If fired[] is being used, an escort that already fired cannot fire again.
        if(fired != NULL && fired[i] == 1){
            continue;
        }
        //if an E has not yet fired,and is alive and can hit B
        if(canEscortHitBattleship(&E[i], B)){

            double distance = calculate_distance(E[i].x,E[i].y,B->x,B->y);//distance bet E and B
            double hitTime =calculateMinimumHitTime( distance,E[i].vMin,E[i].vMax,E[i].angleMin,E[i].angleMax);

            if(escortHitTimes != NULL){
                escortHitTimes[i] = hitTime;
            }
            if(hitTime >= 0){
                if(fired != NULL){
                    fired[i] = 1;
                }

                // Save the shell arrival time.
                // Cumulative damage will be applied later in chronological order.
                pendingEscortHitTimes[i] = hitTime;
                if(!rules.cumulativeDamage){
                    if(earliestEscortHitTime < 0 || hitTime < earliestEscortHitTime){
                        earliestEscortHitTime = hitTime;
                        result.sinkingEscort = i;
                        }
                }

                if(hitTime > result.battleEndTime){
                    result.battleEndTime = hitTime;
                }
            }
        }
    }
        if(rules.cumulativeDamage){
        cumulativeDestroyTime = applyCumulativeEscortDamage( B, E, N, pendingEscortHitTimes, &result.sinkingEscort);
        }

/*
   Time at which B becomes incapable of firing again.
   Part 1-A/B: earliest single escort hit
   Part 1-C + Part 2-A: time cumulative damage reaches 100%
*/
        double battleshipDestroyTime =rules.cumulativeDamage ? cumulativeDestroyTime : earliestEscortHitTime;

    
    //Battleship attacks escorts.

        if(rules.useBattleshipReload && attackOrder != NULL){

    /*
       Part 2-A:
       B attacks according to the strategy order and must wait between consecutive firings.
    */
        for(int a = 0; a < attackCount; a++){

        /*
           attackOrder stores escort ARRAY INDEXES.

           Example:
           attackOrder[a] = 2 means B attacks E[2].
        */
            int i = attackOrder[a];
            if(E[i].alive == 0){
                continue;
            }
        /*
           First shot is fired at 0 seconds.

           If reload time = 2:
           a = 0 -> 0 seconds
           a = 1 -> 2 seconds
           a = 2 -> 4 seconds
        */
            double fireTime = a * rules.battleshipReloadTime;
            //B cannot fire a new shell at or after its destruction time.
            if(battleshipDestroyTime >= 0.0 && fireTime >= battleshipDestroyTime){
                break;
            }

            if(canBattleshipHitEscort(B, &E[i])){

                double distance = calculate_distance(B->x,B->y,E[i].x, E[i].y);
                //   This is the amount of time the shell spends travelling.
                double flightTime = calculateMinimumHitTime(distance,B->vMin,B->vMax, B->angleMin,B->angleMax);

                if(flightTime >= 0){
                    // The shot really was fired.
                    if(battleshipFireTimes != NULL){
                        battleshipFireTimes[i] = fireTime;
                    }
                    // Actual time the shell reaches E: firing time + travelling time
                    double hitTime = fireTime + flightTime;
                    if(battleHitTimes != NULL){
                        battleHitTimes[i] = hitTime;
                    }

                    E[i].alive = 0;
                    result.sunkCount++;

                    if(hitTime > result.lastBattleshipHitTime){
                        result.lastBattleshipHitTime =hitTime;
                    }

                    if(hitTime > result.battleEndTime){
                        result.battleEndTime = hitTime;
                    }
                }
            }
        }
    }

    else{

    /*
       Part 1-A / Part 1-B / Part 1-C
       No B reload delay and no custom attack order.
    */
        for(int i = 0; i < N; i++){
            if(E[i].alive == 0){
                continue;
            }


            if(canBattleshipHitEscort(B, &E[i])){
                double distance = calculate_distance( B->x, B->y, E[i].x,E[i].y);
                double hitTime = calculateMinimumHitTime(distance, B->vMin,  B->vMax,  B->angleMin,  B->angleMax);

                if(battleHitTimes != NULL){
                    battleHitTimes[i] = hitTime;
                }
                if(hitTime >= 0){
                /*
                Part 1-A/B:
                B must hit E before the earliest scort shell destroys B.
                */
                    if(!rules.cumulativeDamage && earliestEscortHitTime >= 0 && hitTime >= earliestEscortHitTime){
                        continue;
                    }


                    E[i].alive = 0;
                    result.sunkCount++;
                    if(hitTime > result.lastBattleshipHitTime){
                        result.lastBattleshipHitTime = hitTime;
                    }
                    if(hitTime >result.battleEndTime){
                        result.battleEndTime = hitTime;
                    }
                }
            }
        }
    }
    


    
    // Decide whether B is destroyed.
    if(rules.cumulativeDamage){
        if(B->damage >= 1.0){
            result.battleshipDestroyed = 1;

        /*
           In a cumulative battle, the battle ends when
           cumulative damage reaches 100%.
        */
            if(cumulativeDestroyTime >= 0.0){
                result.battleEndTime = cumulativeDestroyTime;
            }
        }
    }

    else{
        if(result.sinkingEscort != -1){
            result.battleshipDestroyed = 1;
            //In the one-hit case, the relevant destruction time is the earliest escort hit.
            //if no E managed to sink B(result.sinkingEscort != -1) then battleship survived
            result.battleEndTime = earliestEscortHitTime;
        }
    }
    return result;
}

void resetBattlefield( Battleship *B, EscortShip E[], Battleship originalB, EscortShip originalE[],int N)
{
    *B = originalB;
    for(int i = 0; i < N; i++){
        E[i] = originalE[i];
    }
}
