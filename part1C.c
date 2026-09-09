
#include <stdio.h>

#include "part1C.h"
#include "part1B.h"
#include "battle_engine.h"
#include "files.h"


void simulatePart1C(Battleship *B, EscortShip E[], int N)
{
    B->damage = 0.0;

    double escortHitTimes[N];
    double battleHitTimes[N];

    /*
       Part 1-C rules:

       - Escort attacks cause cumulative damage.
       - B still destroys an escort with one hit.
       - No battleship reload delay yet.
    */
    BattleRules rules;

    rules.cumulativeDamage = 1;
    rules.useBattleshipReload = 0;
    rules.battleshipReloadTime = 0.0;

    printf("\n--- Starting Part 1-C ---\n");


    BattleResult result =simulateBattleStep(B, E, N, rules,  NULL, escortHitTimes, battleHitTimes,NULL,0,NULL);

    if(result.battleshipDestroyed){
        printf("Battleship was destroyed.\n");
    }
    else{
        printf("Battleship survived.\n");
    }


    printf("Total escorts sunk: %d\n", result.sunkCount);
    printf("Cumulative damage on battleship: %.2f%%\n", B->damage * 100.0);
    printf("Battle ended at %.2f seconds.\n", result.battleEndTime);

    savePart1CResults( B, E, N,escortHitTimes,battleHitTimes,result.sunkCount,result.battleEndTime);
}

/*
   One Part 1-C movement battle.

   This same function is used for both:
   Simulation 1
   Simulation 2

   Simulation 2 simply changes B's angle before calling it.
*/
int runPart1CMovementIteration(Battleship *B,EscortShip E[],int N,int iteration,int *totalSunk,int fired[],int simulationNumber)
{
    double escortHitTimes[N];
    double battleHitTimes[N];

//simulationNumber tells whether the function is currently being used for simulation 1 or 2
    BattleRules rules;

    rules.cumulativeDamage = 1;
    rules.useBattleshipReload = 0;
    rules.battleshipReloadTime = 0.0;

    BattleResult result =simulateBattleStep( B, E, N,rules,fired,escortHitTimes,battleHitTimes,NULL,0,NULL);
    *totalSunk += result.sunkCount;

    /*
       Keep the existing file functions for now.
       We'll refactor files.c later.
    */
    if(simulationNumber == 1){
        savePart1CMovementIteration( B,  E,  N, iteration, escortHitTimes, battleHitTimes, result.sunkCount);
    }

    else{
        savePart1CMovementSim2Iteration(B,E,N,iteration,escortHitTimes,battleHitTimes,result.sunkCount);
    }

    return result.battleshipDestroyed;
}


void simulatePart1CMovement(Battleship *B,EscortShip E[],int N,double D)
{
    int k = getMovementPoints("Part 1-C");

    Position path[k];
    generatePath(path, k, D);
    /*
       Save initial conditions for Simulation 2.

       Simulation 2 must start with exactly the same
       battlefield state and use exactly the same path.
    */
    Battleship BSim2 = *B;
    EscortShip ESim2[N];

    for(int i = 0; i < N; i++){
        ESim2[i] = E[i];
    }

    //   MOVEMENT SIMULATION 1
    startPart1CMovementResults(path, k);
    B->damage = 0.0;
    int totalSunk = 0;
    int battleEndedIteration = k;

    //   fired[i] remembers whether Escort i has already fired during a previous movement iteration.
    int fired[N];
    for(int i = 0; i < N; i++){
        fired[i] = 0;
    }
    printf("\n--- Starting Part 1-C Movement Simulation ---\n");

    for(int iteration = 0;iteration < k;iteration++){
        B->x = path[iteration].x;
        B->y = path[iteration].y;

        int battleshipDestroyed = runPart1CMovementIteration( B, E, N, iteration + 1,&totalSunk,fired,1);

        if(battleshipDestroyed){
            battleEndedIteration = iteration + 1;
            break;
        }
    }

    printf("\n--- Part 1-C Movement Summary ---\n");

    if(B->damage >= 1.0){
        printf("Battleship was destroyed at iteration %d.\n", battleEndedIteration);
    }

    else{
        printf("Battleship survived all %d iterations.\n",k);
    }


    printf("Total escorts sunk: %d\n", totalSunk);
    printf("Final cumulative damage: %.2f%%\n", B->damage * 100.0);

    //   MOVEMENT SIMULATION 2
    int t;
    double jammedAngleMin;
    getJamSettings(k, &t, &jammedAngleMin);
    startPart1CMovementSim2Results( path,  k, t, jammedAngleMin);


    BSim2.damage = 0.0;
    int totalSunkSim2 = 0;
    int battleEndedIterationSim2 = k;
    int firedSim2[N];

    for(int i = 0; i < N; i++){
        firedSim2[i] = 0;
    }

    printf("\n--- Starting Part 1-C Movement Simulation 2 ---\n");

    for(int iteration = 0;iteration < k; iteration++){
        BSim2.x = path[iteration].x;
        BSim2.y = path[iteration].y;


        
        //The first t iterations use the normal angle. The jam starts at iteration t + 1.
        if(iteration >= t){
            BSim2.angleMin = jammedAngleMin;
            BSim2.angleMax = 90.0;
        }

        int battleshipDestroyed = runPart1CMovementIteration( &BSim2, ESim2, N,   iteration + 1,   &totalSunkSim2,   firedSim2,  2);

        if(battleshipDestroyed){
            battleEndedIterationSim2 = iteration + 1;
            break;
        }
    }


    printf("\n--- Part 1-C Movement Simulation 2 Summary ---\n");
    if(BSim2.damage >= 1.0){
        printf("Battleship was destroyed at iteration %d.\n", battleEndedIterationSim2);
    }
    else{
        printf("Battleship survived all %d iterations.\n", k);
    }
    printf("Total escorts sunk: %d\n",totalSunkSim2);
    printf("Final cumulative damage: %.2f%%\n",BSim2.damage * 100.0);
}