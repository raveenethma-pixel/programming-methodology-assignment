/*
#include <stdio.h>

#include "part1C.h"
#include "projectile.h"
#include "files.h"
#include "part1B.h"

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
        
        //If E fired a valid shell, its damage still reaches Beven if E is destroyed before the shell arrives.
        
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
void simulatePart1CMovement(Battleship *B, EscortShip E[],int N,double D){
    int k;
    do{
        printf("\nEnter the number of movement points for Part 1-C: ");

        if(scanf("%d", &k) != 1){
            printf("Invalid input. Enter an integer.\n");
            while(getchar() != '\n');
            k = -1;
            continue;
        }
        if(k <= 0){
            printf("Number of points must be greater than 0.\n");
        }
    }while(k <= 0);

    Position path[k];
    generatePath(path, k, D);
    Battleship BSim2 = *B;
    EscortShip ESim2[N];

    for(int i = 0; i < N; i++){
        ESim2[i] = E[i];
    }
    startPart1CMovementResults(path, k);
    
    //Part 1-C cumulative damage starts at zero.
    //Damage will carry from one iteration to the next.
    
    B->damage = 0.0;

    int totalSunk = 0;
    int battleEndedIteration = k;
    int fired[N];

    for(int i = 0; i < N; i++){
        fired[i] = 0;
    }
    printf("\n--- Starting Part 1-C Movement Simulation ---\n");


    for(int iteration = 0; iteration < k; iteration++)
    {
        B->x = path[iteration].x;
        B->y = path[iteration].y;
        int battleshipDestroyed =
        simulatePart1CMovementStep( B,  E, N, iteration + 1,&totalSunk,fired);
        if(battleshipDestroyed == 1){
            battleEndedIteration = iteration + 1;
            break;
        }
    }

    // Keep terminal output short.
    printf("\n--- Part 1-C Movement Summary ---\n");

    if(B->damage >= 1.0) {
        printf("Battleship was destroyed at iteration %d.\n",battleEndedIteration);
    }
    else{
        printf("Battleship survived all %d iterations.\n",k);
    }

    printf("Total escorts sunk: %d\n", totalSunk);
    printf("Final cumulative damage: %.2f%%\n",B->damage * 100.0);
    // ---------------- Part 1-C Movement Simulation 2 ----------------

    int t;

    do{
        printf("\nEnter the iteration after which the gun jams (t < k): ");

        if(scanf("%d", &t) != 1){
            printf("Invalid input. Enter an integer.\n");
            while(getchar() != '\n');
            t = -1;
            continue;
        }

        if(t <= 0 || t >= k){
            printf("t must be greater than 0 and less than %d.\n", k);
        }
    }while(t <= 0 || t >= k);

    double jammedAngleMin;

    do{
        printf("Enter the minimum jammed gun angle (0 < angle < 30): ");

        if(scanf("%lf", &jammedAngleMin) != 1){
            printf("Invalid input.\n");
            while(getchar() != '\n');
            jammedAngleMin = -1.0;
            continue;
        }

        if(jammedAngleMin <= 0.0 || jammedAngleMin >= 30.0){
            printf("Angle must be between 0 and 30 degrees.\n");
        }

    }while(jammedAngleMin <= 0.0 || jammedAngleMin >= 30.0);
    startPart1CMovementSim2Results(path,k,t,jammedAngleMin);

    BSim2.damage = 0.0;

    int totalSunkSim2 = 0;
    int battleEndedIterationSim2 = k;

    int firedSim2[N];

    for(int i = 0; i < N; i++){
        firedSim2[i] = 0;
    }

    printf("\n--- Starting Part 1-C Movement Simulation 2 ---\n");

    for(int iteration = 0; iteration < k; iteration++){

        BSim2.x = path[iteration].x;
        BSim2.y = path[iteration].y;

        
        //Gun works normally for the first t iterations.
        //It jams starting from iteration t + 1.
        
        if(iteration >= t){
            BSim2.angleMin = jammedAngleMin;
            BSim2.angleMax = 90.0;
        }

        int battleshipDestroyed =simulatePart1CMovementSim2Step( &BSim2,  ESim2,  N,  iteration + 1,&totalSunkSim2,    firedSim2);

        if(battleshipDestroyed == 1){
            battleEndedIterationSim2 = iteration + 1;
            break;
        }
    }

    printf("\n--- Part 1-C Movement Simulation 2 Summary ---\n");

    if(BSim2.damage >= 1.0){
        printf("Battleship was destroyed at iteration %d.\n",battleEndedIterationSim2);
    }
    else{
        printf("Battleship survived all %d iterations.\n", k);
    }
    printf("Total escorts sunk: %d\n", totalSunkSim2);
    printf("Final cumulative damage: %.2f%%\n",BSim2.damage * 100.0);
}
int simulatePart1CMovementStep(Battleship *B,EscortShip E[],int N,int iteration,int *totalSunk,int fired[]){
    double escortHitTimes[N];
    double battleHitTimes[N];

    int sunkThisIteration = 0;

    for(int i = 0; i < N; i++){
        escortHitTimes[i] = -1.0;
        battleHitTimes[i] = -1.0;
    }
    for(int i = 0; i < N; i++){
        if(E[i].alive == 0){
            continue;
        }

        double escortHitTime = -1.0;
        double battleHitTime = -1.0;

        // Escort -> Battleship
        if(fired[i] == 0 && canEscortHitBattleship(&E[i], B)){
            double distance =calculate_distance(E[i].x,E[i].y, B->x,B->y);
            escortHitTime =calculateMinimumHitTime(distance,E[i].vMin,E[i].vMax,E[i].angleMin,E[i].angleMax);
        }

        // Battleship -> Escort
        if(canBattleshipHitEscort(B, &E[i])){
            double distance =calculate_distance(B->x,B->y,E[i].x,E[i].y);
            battleHitTime =calculateMinimumHitTime(distance,B->vMin,B->vMax,B->angleMin,B->angleMax);
        }
        escortHitTimes[i] = escortHitTime;
        battleHitTimes[i] = battleHitTime;
        //   If E has a valid shell, its damage still reaches B.
        if(escortHitTime >= 0){
            B->damage += E[i].impactPower;
            fired[i] = 1;
        }
        //   B can still destroy E with one hit.   
        if(battleHitTime >= 0){
            E[i].alive = 0;
            (*totalSunk)++;
            sunkThisIteration++;
        }
    }
    // Save all results from this iteration
    savePart1CMovementIteration( B, E, N, iteration, escortHitTimes,battleHitTimes,sunkThisIteration);
    //Stop the movement simulation once cumulative damage reaches 100%.
    if(B->damage >= 1.0){
        return 1;
     }
    return 0;
}
int simulatePart1CMovementSim2Step(Battleship *B, EscortShip E[],  int N,  int iteration,  int *totalSunk,int fired[]){
    double escortHitTimes[N];
    double battleHitTimes[N];

    int sunkThisIteration = 0;

    for(int i = 0; i < N; i++){
        escortHitTimes[i] = -1.0;
        battleHitTimes[i] = -1.0;
    }

    for(int i = 0; i < N; i++){

        if(E[i].alive == 0){
            continue;
        }

        double escortHitTime = -1.0;
        double battleHitTime = -1.0;

        // Escort -> Battleship
        if(fired[i] == 0 && canEscortHitBattleship(&E[i], B)){

            double distance =calculate_distance(E[i].x,E[i].y ,B->x,B->y);
            escortHitTime =calculateMinimumHitTime(distance, E[i].vMin, E[i].vMax,  E[i].angleMin,E[i].angleMax);
        }

        // Battleship -> Escort
        // This automatically uses the current B angle range.
        // Before the jam: 0 - 90
        // After the jam: jammedAngleMin - 90
        if(canBattleshipHitEscort(B, &E[i])){

            double distance =calculate_distance(B->x, B->y,E[i].x,E[i].y);
            battleHitTime =calculateMinimumHitTime(distance,B->vMin,B->vMax,B->angleMin,B->angleMax);
        }

        escortHitTimes[i] = escortHitTime;
        battleHitTimes[i] = battleHitTime;

        // Each escort can attack only once.
        if(escortHitTime >= 0){

            B->damage += E[i].impactPower;
            fired[i] = 1;
        }

        // Battleship still destroys an escort with one successful attack.
        if(battleHitTime >= 0){

            E[i].alive = 0;
            (*totalSunk)++;
            sunkThisIteration++;
        }
    }
    savePart1CMovementSim2Iteration(B,E,N,iteration,escortHitTimes,battleHitTimes,sunkThisIteration);
    if(B->damage >= 1.0){
        return 1;
    }

    return 0;
} */
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
    int k;

    do{
        printf("\nEnter the number of movement points for Part 1-C: ");

        if(scanf("%d", &k) != 1){
            printf("Invalid input. Enter an integer.\n");
            while(getchar() != '\n');
            k = -1;
            continue;
        }

        if(k <= 0){
            printf("Number of points must be greater than 0.\n");
        }

    }while(k <= 0);

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

    do{
        printf("\nEnter the iteration after which the gun jams (t < k): ");
        if(scanf("%d", &t) != 1){
            printf("Invalid input. Enter an integer.\n");
            while(getchar() != '\n');
            t = -1;
            continue;
        }

        if(t <= 0 || t >= k){

            printf("t must be greater than 0 and less than %d.\n",k);
        }

    }while(t <= 0 || t >= k);

    double jammedAngleMin;
    do{
        printf("Enter the minimum jammed gun angle (0 < angle < 30): ");

        if(scanf("%lf", &jammedAngleMin) != 1){
            printf("Invalid input.\n");
            while(getchar() != '\n');
            jammedAngleMin = -1.0;
            continue;
        }

        if(jammedAngleMin <= 0.0 || jammedAngleMin >= 30.0){
            printf("Angle must be between 0 and 30 degrees.\n");
        }

    }while(jammedAngleMin <= 0.0 || jammedAngleMin >= 30.0);

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


        /*
           The first t iterations use the normal angle.

           The jam starts at iteration t + 1.
        */
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