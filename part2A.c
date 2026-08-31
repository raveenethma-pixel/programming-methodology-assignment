/*1. Ignore sunk escorts
2. Ignore escorts B cannot hit
3. Put escorts B can hit into attackOrder[]
4. Calculate how soon each one can hit B
5. Sort them so the most dangerous escort comes first
*/
#include <stdio.h>

#include "part2A.h"
#include "projectile.h"

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

void simulatePart2A(Battleship *B, EscortShip E[], int N)
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


    int attackOrder[N];
    int attackCount = 0;

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
}