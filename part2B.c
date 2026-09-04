#include <stdio.h>

#include "part2B.h"
#include "part1B.h"
#include "battle_engine.h"
#include "escort.h"
#include "files.h"


static int simulatePart2B_Part1BStep(Battleship *B,EscortShip E[],int N,int iteration,double reloadTime,int *totalSunk);


static double getPositiveTime(const char message[])
{
    double value;
    do{
        printf("%s", message);
        if(scanf("%lf", &value) != 1){
            printf("Invalid input. Enter a number.\n");
            while(getchar() != '\n');
            value = -1.0;
            continue;
        }
        if(value <= 0.0){
            printf("Time must be greater than 0.\n");
        }
    }while(value <= 0.0);
    return value;
}

void getAdvancedReloadSettings(AdvancedBattleRules *rules)
{
    rules->battleshipReloadTime =getPositiveTime( "\nEnter battleship reload time T_B in seconds: ");
    const char *types[ESCORT_TYPE_COUNT] = {"EA", "EB", "EC", "ED", "EE"};
    for(int i = 0; i < ESCORT_TYPE_COUNT; i++){
        printf("Enter reload time T_E for Escort type %s: ",types[i]);
        rules->escortReloadTime[i] = getPositiveTime("");
    }
}

static void simulatePart2B_Part1A(Battleship *B,EscortShip E[],int N,AdvancedBattleRules *rules)
{
    BattleDetails details;
    BattleResult result =runReloadBattle(B,E,N, rules->battleshipReloadTime, 0, NULL, &details);

    printf("\n--- Part 2-B / Part 1-A Summary ---\n");
    printf("Attackable escorts: %d\n", details.attackCount);
    printf("Escorts sunk: %d\n", result.sunkCount);

    if(result.battleshipDestroyed){
        printf("Battleship was sunk by Escort %d (Type: %s) at %.2f seconds.\n", E[result.sinkingEscort].id, E[result.sinkingEscort].type,result.battleEndTime );
    }
    else{
        printf("Battleship survived.\n");
    }
    savePart2BPart1AResults(B,E, &details,result);
}

static void runPart2BPart1BSimulation( Battleship *B, EscortShip E[], int N, Position path[],int k,double reloadTime,int simulationNumber,int jamAfter,double jammedAngleMin)
{
    int totalSunk = 0;
    printf("\n--- Starting Part 2-B / Part 1-B Simulation %d ---\n",simulationNumber);
    for(int iteration = 0; iteration < k; iteration++){

        B->x = path[iteration].x;
        B->y = path[iteration].y;

        if(jamAfter > 0 && iteration >= jamAfter){
            B->angleMin = jammedAngleMin;
            B->angleMax = 90.0;
        }

        if(simulatePart2B_Part1BStep(B,E,N,iteration + 1,reloadTime,&totalSunk))
        {
            printf( "Simulation %d stopped at iteration %d because B was destroyed.\n", simulationNumber, iteration + 1);
            break;
        }
    }
    printf("\n--- Part 2-B / Part 1-B Simulation %d Summary ---\n",simulationNumber);
    printf("Total escorts sunk: %d\n", totalSunk);
}

static void simulatePart2B_Part1B(Battleship *B,EscortShip E[],int N,double D,double reloadTime)
{
    int k = getMovementPoints("Part 2-B / Part 1-B");

    Position path[k];
    generatePath(path, k, D);

    Battleship originalB = *B;
    EscortShip originalE[N];

    for(int i = 0; i < N; i++){
        originalE[i] = E[i];
    }

    runPart2BPart1BSimulation( B, E, N, path, k, reloadTime, 1, -1, 0.0);

    int jamAfter;
    double jammedAngleMin;

    getJamSettings(  k, &jamAfter, &jammedAngleMin);

    resetBattlefield( B, E, originalB, originalE,N);
    runPart2BPart1BSimulation( B, E, N, path, k,reloadTime,2,jamAfter,jammedAngleMin);
}

static int simulatePart2B_Part1BStep(Battleship *B,EscortShip E[],int N,int iteration,double reloadTime,int *totalSunk)
{
    BattleDetails details;
    BattleResult result = runReloadBattle(B,E,N,reloadTime, 0, NULL, &details);
    *totalSunk += result.sunkCount;  //totalSunk = the Es that were sunk in the entire simulation, not just this iteration.
    //result.SunkCount = the Es that were sunk in this iteration.
    savePart2BPart1BIteration(B,E,iteration, &details,result);
    if(result.battleshipDestroyed){
        printf("Iteration %d: Battleship was sunk by Escort %d (Type: %s) at %.2f seconds.\n",iteration,E[result.sinkingEscort].id,E[result.sinkingEscort].type,result.battleEndTime);
        printf("Escorts sunk in this iteration: %d\n",result.sunkCount );
        return 1;
    }
    printf( "Iteration %d: Battleship survived | Escorts sunk: %d\n",iteration,result.sunkCount);
    return 0;
}

//Even though Part 2-B introduces: E firing multiple times, for  part 1A,B it is not needed. Because B can only take one hit.
void simulatePart2B(Battleship *B,EscortShip E[],int N,double D)
{
    AdvancedBattleRules rules;
    getAdvancedReloadSettings(&rules);
    startPart2BResults(&rules);
    Battleship originalB = *B;
    EscortShip originalE[N];
    for(int i = 0; i < N; i++){
        originalE[i] = E[i];
    }
    // Part 1-A style simulation.
    resetBattlefield(B, E, originalB,originalE, N);
    simulatePart2B_Part1A( B, E, N, &rules);
    //part 1-B style simulation.
    resetBattlefield(B, E, originalB,originalE, N);
    simulatePart2B_Part1B( B, E, N, D, rules.battleshipReloadTime);
}
