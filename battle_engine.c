#include <stdio.h>
#include "battle_engine.h"
#include "projectile.h"

BattleResult simulateBattleStep( Battleship *B, EscortShip E[], int N, BattleRules rules, int fired[])
{
    BattleResult result;

    result.battleshipDestroyed = 0;
    result.sinkingEscort = -1;
    result.sunkCount = 0;
    result.battleEndTime = 0.0;
    result.lastBattleshipHitTime = 0.0;

    double earliestEscortHitTime = -1.0;
    
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

        if(canEscortHitBattleship(&E[i], B)){

            double distance = calculate_distance(E[i].x,E[i].y,B->x,B->y);//distance bet E and B

            double hitTime =calculateMinimumHitTime( distance,E[i].vMin,E[i].vMax,E[i].angleMin,E[i].angleMax);

            if(hitTime >= 0){
                // Mark the escort as having fired if Part 1-C is using the fired[] array.
                
                if(fired != NULL){
                    fired[i] = 1;
                }
            
                //Part 1-C: accumulate impact power instead of instant destruction.
                if(rules.cumulativeDamage){
                    B->damage += E[i].impactPower;
                }
                else{
                    //  Part 1-A / Part 1-B:  one escort shell can destroy B.
                    if(earliestEscortHitTime < 0 || hitTime < earliestEscortHitTime){
                        earliestEscortHitTime = hitTime;
                        result.sinkingEscort = i; //sinkingEscort = which escort ship caused the battleship to sink.
                    }
                }

                if(hitTime > result.battleEndTime){
                    result.battleEndTime = hitTime;
                }
            }
        }
    }


    /*
       Battleship attacks escorts.
       For Part 1 there is no reload delay yet.
    */
    for(int i = 0; i < N; i++){

        if(E[i].alive == 0){
            continue;
        }

        if(canBattleshipHitEscort(B, &E[i])){

            double distance = calculate_distance( B->x,  B->y, E[i].x, E[i].y);
            double hitTime =calculateMinimumHitTime(distance,B->vMin,B->vMax,B->angleMin,B->angleMax);

            if(hitTime >= 0){
                if(!rules.cumulativeDamage && earliestEscortHitTime >= 0 && hitTime >= earliestEscortHitTime){
                    continue;
                }
                E[i].alive = 0;
                result.sunkCount++;

                if(hitTime > result.lastBattleshipHitTime){
                    result.lastBattleshipHitTime = hitTime;
                }

                if(hitTime > result.battleEndTime){
                    result.battleEndTime = hitTime;
                }
            }
        }
    }


    
    // Decide whether B is destroyed.
    if(rules.cumulativeDamage){
        if(B->damage >= 1.0){
            result.battleshipDestroyed = 1;
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