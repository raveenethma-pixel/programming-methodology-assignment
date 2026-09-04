#include <stdio.h>

#include "part2A.h"
#include "part1B.h"
#include "projectile.h"
#include "battle_engine.h"
#include "files.h"

void simulatePart2A( Battleship *B,EscortShip E[], int N,double D)
{
    double reloadTime;
    do{
        printf("\nEnter battleship reload time T_B in seconds: " );
        if(scanf("%lf", &reloadTime) != 1){
            printf( "Invalid input. Enter a number.\n");
            while(getchar() != '\n');
            reloadTime = -1.0;
            continue;
        }

        if(reloadTime <= 0.0){
            printf( "Reload time must be greater than 0.\n");
        }

    }while(reloadTime <= 0.0);

    /*
       Save the starting battlefield.
       Every Part 2-A simulation must begin from the same original state.
    */
    Battleship originalB = *B;
    EscortShip originalE[N];
    for(int i = 0; i < N; i++){
        originalE[i] = E[i];
    }

    //PART 1-A STYLE
    resetBattlefield(B, E, originalB, originalE, N);
    simulatePart2A_Part1A( B, E, N,reloadTime);

    //PART 1-B STYLE
    resetBattlefield(B, E, originalB, originalE, N);
    simulatePart2A_Part1B( B,E, N,D,reloadTime);
    
    // PART 1-C STYLE
    resetBattlefield(B, E, originalB, originalE, N);
    simulatePart2A_Part1C( B, E, N, D,reloadTime);
}


void simulatePart2A_Part1A(Battleship *B,EscortShip E[],int N,double reloadTime)
{
    BattleDetails details; // no cumulative damage,escorts do not need fired[] here
    BattleResult result = runReloadBattle(B, E, N, reloadTime, 0, NULL, &details);
    //print attacks that were actually fired
    for(int a = 0; a < details.attackCount; a++){
        int index = details.attackOrder[a];

        //Only display attacks that were actually fired.
        if(details.fireTimes[a] >= 0.0 && details.hitTimes[a] >= 0.0){
            printf("Attack %d -> Escort %d (Type: %s) | Fire Time: %.2f s | Hit Time: %.2f s\n", a + 1, E[index].id, E[index].type, details.fireTimes[a],details.hitTimes[a]);
        }
    }

    printf("\n--- Part 2-A Summary ---\n");
    printf("Attackable escorts: %d\n", details.attackCount);
    printf("Escorts sunk: %d\n",result.sunkCount);

    if(result.battleshipDestroyed){
        printf("Battleship was sunk by Escort %d (Type: %s).\n", E[result.sinkingEscort].id, E[result.sinkingEscort].type);
        printf("Battleship sank at %.2f seconds.\n",result.battleEndTime);
    }
    else{
        printf("Battleship survived.\n");
    }
  savePart2APart1AResults( B,E, N,reloadTime, &details,result); 
}

static void runPart2APart1BSimulation( Battleship *B, EscortShip E[], int N, Position path[],  int k, double reloadTime, int simulationNumber,int jamAfter,double jammedAngleMin)
{
    int totalSunk = 0;
    const char *filename = simulationNumber == 1 ? "part2A_part1B_sim1_results.txt" : "part2A_part1B_sim2_results.txt";


    startPart2APart1BResults(filename,path, k, reloadTime, jamAfter, jammedAngleMin);
    printf( "\n--- Starting Part 2-A / Part 1-B Simulation %d ---\n",simulationNumber);
    for(int iteration = 0; iteration < k; iteration++){

        B->x = path[iteration].x;
        B->y = path[iteration].y;

        // Simulation 2: gun jams after iteration jamAfter.
        if(jamAfter > 0 && iteration >= jamAfter){
            B->angleMin = jammedAngleMin;
            B->angleMax = 90.0;
        }

        int battleshipDestroyed =simulatePart2A_Part1BStep( B, E, N,iteration + 1, reloadTime, &totalSunk,filename);
        if(battleshipDestroyed){
            printf( "Simulation %d stopped at iteration %d because B was destroyed.\n",simulationNumber,iteration + 1);
            break;
        }
    }

    printf("\n--- Part 2-A / Part 1-B Simulation %d Summary ---\n",simulationNumber);
    printf("Total escorts sunk: %d\n", totalSunk);
}
void simulatePart2A_Part1B( Battleship *B,EscortShip E[], int N, double D, double reloadTime)
{
    int k = getMovementPoints("Part 2-A / Part 1-B");
    Position path[k];
    generatePath(path, k, D);
    printf("\n--- Part 2-A / Part 1-B Path ---\n");

    for(int i = 0; i < k; i++){
        printf("Point %d: (%.2f, %.2f)\n", i + 1, path[i].x, path[i].y);
    }

    // Save original state for Simulation 2.
    Battleship originalB = *B;
    EscortShip originalE[N];

    for(int i = 0; i < N; i++){
        originalE[i] = E[i];
    }

    // Simulation 1 - normal gun.
    runPart2APart1BSimulation( B, E,N, path, k, reloadTime, 1, -1, 0.0);

    // Get jam settings for Simulation 2.
    int t;
    double jammedAngleMin;
    getJamSettings( k, &t, &jammedAngleMin);
    // Simulation 2 starts from original battlefield.
    resetBattlefield( B, E, originalB, originalE, N);
    runPart2APart1BSimulation( B, E, N, path, k, reloadTime, 2,t,jammedAngleMin);
}

int simulatePart2A_Part1BStep( Battleship *B, EscortShip E[], int N, int iteration, double reloadTime, int *totalSunk, const char filename[])
{
    BattleDetails details;
    // Part 1-B: no cumulative damage no fired[] restriction
    BattleResult result = runReloadBattle( B, E, N,reloadTime, 0, NULL, &details);
    *totalSunk += result.sunkCount;
    savePart2APart1BIteration(filename, B, E, N,iteration,&details,result);
    if(result.battleshipDestroyed){
        printf("Iteration %d: Battleship was sunk by Escort %d (Type: %s) at %.2f seconds.\n", iteration, E[result.sinkingEscort].id, E[result.sinkingEscort].type, result.battleEndTime);
        printf("Escorts sunk in this iteration: %d\n",result.sunkCount);
        return 1;
    }
    printf("Iteration %d: Battleship survived.\n", iteration);
    printf( "Escorts sunk in this iteration: %d\n", result.sunkCount);

    return 0;
}
static void runPart2APart1CStationary(Battleship *B,EscortShip E[],int N,double reloadTime)
{
    int fired[N];
    for(int i = 0; i < N; i++){
        fired[i] = 0;
    }
    B->damage = 0.0;
    BattleDetails details;
    BattleResult result =
        runReloadBattle(
            B,
            E,
            N,
            reloadTime,
            1,          // cumulative damage
            fired,      // each escort attacks only once
            &details
        );
// 0 = stationary simulation
    savePart2APart1CIteration(B,E,N,1,0,&details,result);
    printf("\n--- Part 2-A / Part 1-C Stationary Simulation ---\n");
    printf("Escorts sunk: %d\n",result.sunkCount);
    printf("Cumulative damage on Battleship: %.2f%%\n",B->damage * 100.0);
    if(result.battleshipDestroyed){
        printf("Battleship was destroyed at %.2f seconds.\n",result.battleEndTime);
    }
    else{
        printf("Battleship survived.\n");
    }
}
static void runPart2APart1CSimulation(Battleship *B,EscortShip E[],int N,Position path[],int k,double reloadTime,int simulationNumber,int jamAfter,double jammedAngleMin)
{
    int totalSunk = 0;
    int fired[N];
    B->damage = 0.0;
    for(int i = 0; i < N; i++){
        fired[i] = 0;
    }
    printf("\n--- Starting Part 2-A / Part 1-C Simulation %d ---\n", simulationNumber);

    for(int iteration = 0; iteration < k; iteration++){
        B->x = path[iteration].x;
        B->y = path[iteration].y;
        // Simulation 2 only: gun jams after iteration jamAfter.
        if(jamAfter > 0 && iteration >= jamAfter){
            B->angleMin = jammedAngleMin;
            B->angleMax = 90.0;
        }
        int battleshipDestroyed =simulatePart2A_Part1CStep( B, E, N,iteration + 1, reloadTime, &totalSunk,fired, simulationNumber);
        if(battleshipDestroyed){
            printf( "Simulation %d stopped at iteration %d because B was destroyed.\n", simulationNumber, iteration + 1);
            break;
        }
    }
    printf( "\n--- Part 2-A / Part 1-C Simulation %d Summary ---\n",simulationNumber);
    printf("Total escorts sunk: %d\n", totalSunk);
    printf("Final cumulative damage: %.2f%%\n", B->damage * 100.0 );
}
void simulatePart2A_Part1C(Battleship *B,EscortShip E[],int N,double D,double reloadTime)
{
    int k = getMovementPoints("Part 2-A / Part 1-C");
    Position path[k];
    generatePath(path, k, D);
    startPart2APart1CResults(path,k,reloadTime);
    // Save the original battlefield.
    // Stationary, Movement 1 and Movement 2 must all
    // begin from these same initial conditions.
    Battleship originalB = *B;
    EscortShip originalE[N];

    for(int i = 0; i < N; i++){
        originalE[i] = E[i];
    }
    // Part 1-C stationary Part 1-A-style simulation.
    runPart2APart1CStationary(B,E,N,reloadTime);
    // Reset before Movement Simulation 1.
    resetBattlefield(B,E,originalB,originalE,N);
    // Movement Simulation 1 - normal gun.
    runPart2APart1CSimulation(B,E,N,path,k,reloadTime,1,-1,0.0);
    // Jam settings for Simulation 2.
    int t;
    double jammedAngleMin;
    getJamSettings( k, &t, &jammedAngleMin);
    savePart2APart1CJamSettings(t,jammedAngleMin);
    // Simulation 2 must start from the original battlefield.
    resetBattlefield( B, E, originalB, originalE, N);
    // Simulation 2 - jammed gun.
    runPart2APart1CSimulation( B, E, N,path, k,reloadTime, 2,t, jammedAngleMin );
}
int simulatePart2A_Part1CStep(Battleship *B,EscortShip E[],int N,int iteration,double reloadTime,int *totalSunk,int fired[],int simulationNumber)
{
    BattleDetails details;
// Part 1-C uses cumulative damage each escort can fire only once
    BattleResult result = runReloadBattle( B, E, N, reloadTime,1,fired, &details);
    *totalSunk += result.sunkCount;
    savePart2APart1CIteration(B, E, N, iteration, simulationNumber,&details,result);
    printf("Iteration %d: Escorts sunk: %d | Damage on B: %.2f%%\n",iteration, result.sunkCount, B->damage * 100.0 );
    if(result.battleshipDestroyed){
        printf("Battleship destroyed during iteration %d.\n",iteration);
        return 1;
    }
    return 0;
}
