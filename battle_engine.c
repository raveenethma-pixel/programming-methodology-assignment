#include <stdio.h>
#include "battle_engine.h"
#include "projectile.h"

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

        if(battleshipFireTimes != NULL){
            battleshipFireTimes[i] = fireTime;
        }

        //If B is destroyed before it reaches this iring time, it cannot fire this shell.
        if(earliestEscortHitTime >= 0 && fireTime >= earliestEscortHitTime){
            break;
        }


        if(canBattleshipHitEscort(B, &E[i])){

            double distance = calculate_distance(B->x,B->y,E[i].x, E[i].y);
            //   This is the amount of time the shell spends travelling.
            double flightTime = calculateMinimumHitTime(distance,B->vMin,B->vMax, B->angleMin,B->angleMax);

            if(flightTime >= 0){

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