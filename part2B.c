#include <stdio.h>

#include "part2B.h"
#include "part1B.h"
#include "battle_engine.h"
#include "escort.h"
#include "files.h"
#include "projectile.h"

static int simulatePart2B_Part1BStep(Battleship *B,EscortShip E[],int N,int iteration,double reloadTime,int *totalSunk);

static BattleResult runContinuousEscortBattle( Battleship *B, EscortShip E[], int N, AdvancedBattleRules *rules, BattleDetails *details, int escortHitCount[]);

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
    savePart2BPart1BSetup( path, k,simulationNumber,jamAfter,jammedAngleMin);
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

static BattleResult runContinuousEscortBattle(Battleship *B, EscortShip E[], int N,AdvancedBattleRules *rules,BattleDetails *details,int escortHitCount[])
{
    int fired[N];
    int wasAlive[N];
    double destroyTime[N];
    double travelTime[N];
    double nextFire[N];
    double startDamage = B->damage;

    for(int i = 0; i < N; i++){
        fired[i] = 0;
        wasAlive[i] = E[i].alive;
        destroyTime[i] = -1.0;
        travelTime[i] = -1.0;
        nextFire[i] = 0.0;
        escortHitCount[i] = 0;
    }

    /*
       Reuse the existing Part 2-A engine to calculate
       B's attack order, fire times and E destruction times.
    */
    BattleResult base = runReloadBattle(B,E, N, rules->battleshipReloadTime, 1,fired,details);

    /* Find when B destroys each escort. */
    for(int a = 0; a < details->attackCount; a++){
        int index = details->attackOrder[a];
        if(details->hitTimes[a] >= 0.0){
            destroyTime[index] = details->hitTimes[a];
        }
    }
    /* Restore damage/alive state. We now calculate repeated E attacks. */
    B->damage = startDamage;
    for(int i = 0; i < N; i++){
        E[i].alive = wasAlive[i];
        if(wasAlive[i] && canEscortHitBattleship(&E[i], B)){
            double distance = calculate_distance(E[i].x, E[i].y, B->x, B->y);
            travelTime[i] = calculateMinimumHitTime(distance, E[i].vMin,E[i].vMax, E[i].angleMin,E[i].angleMax);
        }
    }
    BattleResult result = base;
    result.battleshipDestroyed = 0;
    result.sinkingEscort = -1;
    result.sunkCount = 0;
    result.battleEndTime = 0.0;
    result.lastBattleshipHitTime = -1.0;
    double deathTime = -1.0;

    while(B->damage < 1.0){
        int nextEscort = -1;
        double nextHit = -1.0;
        /* Find the next escort shell to hit B. */
        for(int i = 0; i < N; i++){
            if(!wasAlive[i] || travelTime[i] < 0.0){
                continue;
            }
            /* Escort cannot fire at/after its destruction time. */
            if(destroyTime[i] >= 0.0 &&
               nextFire[i] >= destroyTime[i]){
                continue;
            }
            double hitTime = nextFire[i] + travelTime[i];
            if(nextHit < 0.0 || hitTime < nextHit){
                nextHit = hitTime;
                nextEscort = i;
            }
        }
        //No escort can fire again. 
        if(nextEscort == -1){
            break;
        }
        B->damage += E[nextEscort].impactPower;
        escortHitCount[nextEscort]++;
        result.lastBattleshipHitTime = nextHit;
        int type = getEscortTypeIndex(E[nextEscort].type);
        nextFire[nextEscort] += rules->escortReloadTime[type];
        if(B->damage >= 1.0){
            result.battleshipDestroyed = 1;
            result.sinkingEscort = nextEscort;
            result.battleEndTime = nextHit;
            deathTime = nextHit;
            break;
        }
    }
    /*
       Keep only B attacks that were fired before B was destroyed.
       A shell already fired is still allowed to hit afterwards.
    */
    for(int a = 0; a < details->attackCount; a++){
        int index = details->attackOrder[a];
        if(details->fireTimes[a] >= 0.0 &&
           (deathTime < 0.0 || details->fireTimes[a] < deathTime)){
            E[index].alive = 0;
            result.sunkCount++;
        }
        else if(deathTime >= 0.0){
            details->fireTimes[a] = -1.0;
            details->hitTimes[a] = -1.0;
        }
    }
    /* If B survived, find when the last event finished. */
    if(!result.battleshipDestroyed){
        result.battleEndTime = result.lastBattleshipHitTime;
        for(int a = 0; a < details->attackCount; a++){
            if(details->hitTimes[a] > result.battleEndTime){
                result.battleEndTime = details->hitTimes[a];
            }
        }
    }
    return result;
}

static void simulatePart2B_Part1CStationary( Battleship *B, EscortShip E[], int N,AdvancedBattleRules *rules)
{
    BattleDetails details;
    int escortHitCount[N];
    B->damage = 0.0;
    BattleResult result = runContinuousEscortBattle(B, E, N, rules,&details,escortHitCount);

    printf( "\n--- Part 2-B / Part 1-C Stationary Simulation ---\n");
    printf( "Escorts sunk: %d\n",result.sunkCount);
    printf("Cumulative damage on B: %.2f%%\n",B->damage * 100.0);
    if(result.battleshipDestroyed){
        printf("Battleship was destroyed by Escort %d (Type: %s) at %.2f seconds.\n",E[result.sinkingEscort].id,E[result.sinkingEscort].type,result.battleEndTime);
    }
    else{
        printf("Battleship survived.\n");
    }
    savePart2BPart1CResult("PART 2-B / PART 1-C - STATIONARY", B, E, N,&details,result,escortHitCount);
}

static void runPart2BPart1CSimulation(Battleship *B,EscortShip E[],int N,Position path[],int k,AdvancedBattleRules *rules,int simulationNumber,int jamAfter,double jammedAngleMin)
{
    int totalSunk = 0;
    B->damage = 0.0;

    printf("\n--- Starting Part 2-B / Part 1-C Simulation %d ---\n",simulationNumber);

    for(int iteration = 0; iteration < k; iteration++){

        B->x = path[iteration].x;
        B->y = path[iteration].y;

        if(jamAfter > 0 && iteration >= jamAfter){
            B->angleMin = jammedAngleMin;
            B->angleMax = 90.0;
        }

        BattleDetails details;
        int escortHitCount[N];

        BattleResult result =runContinuousEscortBattle( B,E, N, rules, &details, escortHitCount);

        totalSunk += result.sunkCount;

        char title[100];

        snprintf(
            title,
            sizeof(title),
            "PART 2-B / PART 1-C - SIMULATION %d - ITERATION %d", simulationNumber,iteration + 1);
        savePart2BPart1CResult( title, B, E,  N, &details, result, escortHitCount);
        printf("Iteration %d: Escorts sunk: %d | Damage on B: %.2f%%\n", iteration + 1, result.sunkCount, B->damage * 100.0);
        if(result.battleshipDestroyed){
            printf("Battleship destroyed during iteration %d at %.2f seconds.\n",iteration + 1,result.battleEndTime);
            break;
        }
    }
    printf("\n--- Part 2-B / Part 1-C Simulation %d Summary ---\n",simulationNumber);
    printf("Total escorts sunk: %d\n", totalSunk);
    printf("Final cumulative damage: %.2f%%\n", B->damage * 100.0);
}

static void simulatePart2B_Part1CMovement( Battleship *B,EscortShip E[], int N, double D, AdvancedBattleRules *rules)
{
    int k = getMovementPoints("Part 2-B / Part 1-C");

    Position path[k];
    generatePath(path, k, D);
    Battleship originalB = *B;
    EscortShip originalE[N];
    for(int i = 0; i < N; i++){
        originalE[i] = E[i];
    }
    // Simulation 1 - normal gun.
    runPart2BPart1CSimulation( B, E, N, path, k, rules, 1, -1, 0.0);

    int jamAfter;
    double jammedAngleMin;

    getJamSettings( k, &jamAfter,&jammedAngleMin);

    // Simulation 2 starts from the same original battlefield.
    resetBattlefield( B, E, originalB,originalE,N);
    runPart2BPart1CSimulation( B, E,N,path,k,rules, 2,jamAfter,jammedAngleMin);
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
    simulatePart2B_Part1B( B, E, N, D,rules.battleshipReloadTime);
    // Part 1-C style
    resetBattlefield(B,E,originalB,originalE, N);
    simulatePart2B_Part1CStationary(B,E,N,&rules);
    // Reset before Part 1-C movement simulations.
    resetBattlefield(B,E,originalB, originalE, N);
    simulatePart2B_Part1CMovement( B, E, N, D, &rules);//AdvancedBattleRules *rules
}
