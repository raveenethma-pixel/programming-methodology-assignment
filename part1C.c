#include <stdio.h>

#include "part1C.h"
#include "projectile.h"
#include "files.h"

void simulatePart1C(Battleship *B,EscortShip E[],int N){
    B->damage = 0.0;
    int sunkCount = 0;
    double battleEndTime = 0.0;
    double escortHitTimes[N];
    double battleHitTimes[N];
    for(int i = 0; i < N; i++){
        escortHitTimes[i] = -1.0;
        battleHitTimes[i] = -1.0;
    }
    printf("\n--- Starting Part 1-C ---\n");

    for(int i = 0; i < N; i++){
        if(E[i].alive == 0){
            continue;
        }

        double escortHitTime = -1.0;
        double battleHitTime = -1.0;
        
        // Check whether Escort i can hit B.
        if(canEscortHitBattleship(&E[i], B)){
            double distance =calculate_distance(E[i].x,E[i].y,B->x,B->y);
            escortHitTime =calculateMinimumHitTime( distance,E[i].vMin,E[i].vMax,E[i].angleMin, E[i].angleMax);
        }
        //  Check whether B can hit Escort i.
        if(canBattleshipHitEscort(B, &E[i])){
            double distance =calculate_distance(B->x,B->y,E[i].x,E[i].y);
            battleHitTime =calculateMinimumHitTime(distance,B->vMin,B->vMax,B->angleMin,B->angleMax);
        }
        //Save the calculated hit times for the result file 
        escortHitTimes[i] = escortHitTime;
        battleHitTimes[i] = battleHitTime;
        /*
           If E fired a valid shell, its damage still reaches B
           even if E is destroyed before the shell arrives.
        */
        if(escortHitTime >= 0){
            B->damage += E[i].impactPower;
            if(escortHitTime > battleEndTime){
                battleEndTime = escortHitTime;
            }
        }
        
        //B can still destroy E with one successful shell.
        if(battleHitTime >= 0){
            E[i].alive = 0;
            sunkCount++;

            if(battleHitTime > battleEndTime){
                battleEndTime = battleHitTime;
            }
        }
    }
    
    //Keep terminal output short.
    if(B->damage >= 1.0){
        printf("Battleship was destroyed.\n");
    }
    else{
        printf("Battleship survived.\n");
    }

    printf("Total escorts sunk: %d\n",sunkCount);
    printf("Cumulative damage on battleship: %.2f%%\n",B->damage * 100.0);
    printf("Battle ended at %.2f seconds.\n", battleEndTime);
    savePart1CResults(B,E,N,escortHitTimes,battleHitTimes,sunkCount,battleEndTime);
}