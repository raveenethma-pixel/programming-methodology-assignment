/*1. Ignore sunk escorts
2. Ignore escorts B cannot hit
3. Put escorts B can hit into attackOrder[]
4. Calculate how soon each one can hit B
5. Sort them so the most dangerous escort comes first
*/
#include <stdio.h>
#include "files.h"
#include "part2A.h"
#include "projectile.h"
#include "part1B.h"

void determineAttackOrder(Battleship *B,  EscortShip E[], int N,int attackOrder[],int *attackCount){
    double threatTime[N];

    *attackCount = 0;

    // Find escorts that B can attack
    for(int i = 0; i < N; i++){
        threatTime[i] = -1.0;
        if(E[i].alive == 0){
            continue;
        }

        if(canBattleshipHitEscort(B, &E[i])){

            attackOrder[*attackCount] = i;
            (*attackCount)++;

            /*
               If this escort can hit B, calculate how soon
               it can hit B. Smaller time = higher threat.
            */
            if(canEscortHitBattleship(&E[i], B)){
                double distance =calculate_distance(E[i].x,  E[i].y,  B->x,B->y);
                threatTime[i] = calculateMinimumHitTime( distance, E[i].vMin,  E[i].vMax, E[i].angleMin, E[i].angleMax);
            }
        }
    }

    /*
       Sort attackOrder using bubble sort.

       Escorts that can hit B are attacked first,
       with the smallest threat time first.
    */
    for(int i = 0; i < *attackCount - 1; i++){

        for(int j = 0; j < *attackCount - i - 1; j++){

            int first = attackOrder[j];
            int second = attackOrder[j + 1];
            int swap = 0;

            if(threatTime[first] < 0 && threatTime[second] >= 0){
                swap = 1;
            }

            else if(threatTime[first] >= 0 && threatTime[second] >= 0 && threatTime[first] > threatTime[second]){
                swap = 1;
            }

            if(swap){
                int temp = attackOrder[j];attackOrder[j] = attackOrder[j + 1];attackOrder[j + 1] = temp;
            }
        }
    }
}
void simulatePart2A(Battleship *B,EscortShip E[],int N,double D)
{
    double reloadTime;

    do{
        printf("\nEnter battleship reload time T_B in seconds: ");

        if(scanf("%lf", &reloadTime) != 1){
            printf("Invalid input. Enter a number.\n");
            while(getchar() != '\n');
            reloadTime = -1.0;
            continue;
        }

        if(reloadTime <= 0.0){
            printf("Reload time must be greater than 0.\n");
        }

    }while(reloadTime <= 0.0);


    /*
       Save original battlefield conditions.
       Each Part 2-A simulation must start from fresh conditions.
    */
    Battleship originalB = *B;//copies the original structure
    EscortShip originalE[N];

    for(int i = 0; i < N; i++){
        originalE[i] = E[i];
    }


    // ---------------- Part 1-A under Part 2-A rules ----------------

    *B = originalB;

    for(int i = 0; i < N; i++){
        E[i] = originalE[i];
    }

    simulatePart2A_Part1A( B, E, N,reloadTime);
    
    // ---------------- Part 1-B under Part 2-A rules ----------------

    *B = originalB;

    for(int i = 0; i < N; i++){
        E[i] = originalE[i];
    }
    simulatePart2A_Part1B(B, E, N,D, reloadTime);

}

void simulatePart2A_Part1A(Battleship *B, EscortShip E[], int N,double reloadTime)
{
    
    int attackOrder[N];
    int attackCount = 0;
    double fireTimes[N];
    double hitTimes[N];

    for(int i = 0; i < N; i++){
        fireTimes[i] = -1.0;
        hitTimes[i] = -1.0;
    }

    determineAttackOrder(B,E, N, attackOrder, &attackCount);

    printf("\n--- Starting Part 2-A ---\n");


    double currentFireTime = 0.0;
    int sunkCount = 0;
    double earliestEscortHitTime = -1.0;
    int sinkingEscort = -1;

    for(int i = 0; i < N; i++){
        if(E[i].alive == 0){
            continue;
        }

        if(canEscortHitBattleship(&E[i], B)){

            double distance =calculate_distance( E[i].x, E[i].y, B->x, B->y);
            double hitTime =calculateMinimumHitTime( distance,E[i].vMin,E[i].vMax,E[i].angleMin,E[i].angleMax);

            if(hitTime >= 0 && (earliestEscortHitTime < 0 || hitTime < earliestEscortHitTime)){
                earliestEscortHitTime = hitTime;
                sinkingEscort = i;
            }
        }
    }
    //battleship's attack order
    for(int i = 0; i < attackCount; i++){
        int index = attackOrder[i];
         //  Calculate how long the shell itself takesto reach this escort.
        double distance =calculate_distance(  B->x,  B->y, E[index].x, E[index].y);
        double shellFlightTime =calculateMinimumHitTime(  distance,   B->vMin,   B->vMax,  B->angleMin,  B->angleMax);

        if(shellFlightTime < 0){
            continue;
        }


        /*
           Actual time the escort is hit:

           firing time + projectile flight time
        */
        double actualHitTime =currentFireTime + shellFlightTime;
        if(earliestEscortHitTime >= 0 && currentFireTime >= earliestEscortHitTime){
            break;
        }
        fireTimes[i] = currentFireTime;
        hitTimes[i] = actualHitTime;
        printf("Attack %d -> Escort %d (Type: %s) | ""Fire Time: %.2f s | Hit Time: %.2f s\n", i + 1,  E[index].id,  E[index].type, currentFireTime, actualHitTime);
        //   B still destroys E with one successful hitat this stage.
        
        E[index].alive = 0;
        sunkCount++;

        //   The gun must wait before it can fire again. 
        currentFireTime += reloadTime;
    }
    printf("\n--- Part 2-A Summary ---\n");
    printf("Attackable escorts: %d\n", attackCount);
    printf("Escorts sunk: %d\n", sunkCount);
    if(sinkingEscort != -1){
        printf("Battleship was sunk by Escort %d (Type: %s).\n",E[sinkingEscort].id,E[sinkingEscort].type);
        printf("Battleship sank at %.2f seconds.\n",earliestEscortHitTime);
    }
    else{
        printf("Battleship survived.\n");
    }
    savePart2APart1AResults(B,E,N,reloadTime,attackOrder,attackCount, fireTimes, hitTimes, sunkCount, sinkingEscort, earliestEscortHitTime);
}
void simulatePart2A_Part1B(Battleship *B,EscortShip E[],int N,double D,double reloadTime)
{
    int k;

    do{
        printf("\nEnter the number of movement points for Part 2-A / Part 1-B: ");

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
    startPart2APart1BResults( path,k,reloadTime);

    printf("\n--- Part 2-A / Part 1-B Path ---\n");

    for(int i = 0; i < k; i++){
        printf("Point %d: (%.2f, %.2f)\n",i + 1,path[i].x,path[i].y);
    }  
    int totalSunk = 0;

    printf("\n--- Starting Part 2-A / Part 1-B Simulation ---\n");

    for(int iteration = 0; iteration < k; iteration++){

        B->x = path[iteration].x;
        B->y = path[iteration].y;

        int battleshipDestroyed =simulatePart2A_Part1BStep( B, E, N,iteration + 1,reloadTime,&totalSunk);

        if(battleshipDestroyed == 1){
            printf("Simulation stopped at iteration %d because B was destroyed.\n",iteration + 1);
            break;
        }
    }

    printf("\n--- Part 2-A / Part 1-B Summary ---\n");
    printf("Total escorts sunk: %d\n", totalSunk);
}
int simulatePart2A_Part1BStep(Battleship *B,EscortShip E[],int N,int iteration,double reloadTime,int *totalSunk)
{
    int attackOrder[N];
    int attackCount = 0;
    double fireTimes[N];
    double hitTimes[N];

    for(int i = 0; i < N; i++){
        fireTimes[i] = -1.0;
        hitTimes[i] = -1.0;
    }

    determineAttackOrder( B,E, N, attackOrder,&attackCount);

    double earliestEscortHitTime = -1.0;
    int sinkingEscort = -1;

    for(int i = 0; i < N; i++){

        if(E[i].alive == 0){
            continue;
    }

        if(canEscortHitBattleship(&E[i], B)){

            double distance =calculate_distance(E[i].x,E[i].y,B->x,B->y);
            double hitTime =calculateMinimumHitTime(distance,E[i].vMin,E[i].vMax,E[i].angleMin,E[i].angleMax);

            if(hitTime >= 0 && (earliestEscortHitTime < 0 || hitTime < earliestEscortHitTime)){
                earliestEscortHitTime = hitTime;
                sinkingEscort = i;
            }
        }
    }
    double currentFireTime = 0.0;
    int sunkThisIteration = 0;

    for(int i = 0; i < attackCount; i++){

        int index = attackOrder[i];
        double distance =calculate_distance(B->x,B->y,E[index].x,E[index].y);
        double shellFlightTime =calculateMinimumHitTime(distance, B->vMin,B->vMax,B->angleMin,B->angleMax);

        if(shellFlightTime < 0){
            continue;
        }

    
    //  B cannot fire once the escort shell has already hit it.
    
        if(earliestEscortHitTime >= 0 &&
            currentFireTime >= earliestEscortHitTime){
            break;
        }

        double actualHitTime = currentFireTime + shellFlightTime;
        fireTimes[i] = currentFireTime;
        hitTimes[i] = actualHitTime;
        E[index].alive = 0;

        (*totalSunk)++;
        sunkThisIteration++;

        currentFireTime += reloadTime;
    }
    savePart2APart1BIteration( B, E, N,iteration,attackOrder,attackCount,fireTimes,hitTimes,sunkThisIteration,sinkingEscort,earliestEscortHitTime);
 
    if(sinkingEscort != -1){

    printf("Iteration %d: Battleship was sunk by Escort %d (Type: %s) at %.2f seconds.\n", iteration, E[sinkingEscort].id, E[sinkingEscort].type,earliestEscortHitTime);
    printf("Escorts sunk in this iteration: %d\n", sunkThisIteration);

    return 1;
    }

    printf("Iteration %d: Battleship survived.\n",iteration);
    printf("Escorts sunk in this iteration: %d\n",sunkThisIteration);

    return 0;
}