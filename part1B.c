

#include <stdio.h>
#include <stdlib.h>

#include "part1B.h"
#include "battle_engine.h"
#include "files.h"


void generatePath(Position path[], int k, double D)
{
    for(int i = 0; i < k; i++){
        path[i].x = 1.0 + ((double)rand() / RAND_MAX) * (D - 2.0);
        path[i].y = 1.0 + ((double)rand() / RAND_MAX) * (D - 2.0);
    }
}
//if k=4 and t=3  then the gun jams after iteration 3, so iteration 4 is the first jammed iteration:

int getMovementPoints(const char label[])
{
    int k;
    do{
        printf("\nEnter the number of movement points for %s: ", label);
        if(scanf("%d", &k) != 1){
            printf("Invalid input. Enter an integer.\n");
            while(getchar() != '\n');
            k = -1;
            continue;
        }
        if(k < 2){
            printf("Number of points must be at least 2.\n");
        }
    }while(k < 2);
    return k;
}
void getJamSettings(int k,int *t,double *jammedAngleMin)
{
    do{
        printf("\nEnter the iteration after which the gun jams (t < k): ");

        if(scanf("%d", t) != 1){
            printf("Invalid input. Enter an integer.\n");
            while(getchar() != '\n');
            *t = -1;
            continue;
        }

        if(*t <= 0 || *t >= k){
            printf("t must be greater than 0 and less than %d.\n", k );
        }

    }while(*t <= 0 || *t >= k);


    do{
        printf( "Enter the minimum jammed gun angle (0 < angle < 30): ");

        if(scanf("%lf", jammedAngleMin) != 1){
            printf("Invalid input.\n");
            while(getchar() != '\n');
            *jammedAngleMin = -1.0;
            continue;
        }

        if(*jammedAngleMin <= 0.0 || *jammedAngleMin >= 30.0){
            printf("Angle must be between 0 and 30 degrees.\n");
        }

    }while( *jammedAngleMin <= 0.0 ||  *jammedAngleMin >= 30.0);
}


/*
   Runs one Part 1-B battle iteration.
   Both Simulation 1 and Simulation 2 use this same function.
   Simulation 2 simply changes B's angle before calling it.
*/
int runPart1BIteration(Battleship *B,EscortShip E[],int N,int iteration,int simulationNumber)
{
    /*
       Save which escorts were alive before this iteration.
       After the battle engine runs, we can compare this
       array with E[i].alive to determine which escorts
       were sunk during this particular iteration.
    */
    int wasAlive[N];

    for(int i = 0; i < N; i++){
        wasAlive[i] = E[i].alive;
    }


    /*
       Part 1-B uses the same battle rules as Part 1-A.

       - no reload delay
       - one escort hit destroys B
       - one B hit destroys an escort
    */
    BattleRules rules;

    rules.cumulativeDamage = 0;
    rules.useBattleshipReload = 0;
    rules.battleshipReloadTime = 0.0;

    printf("\n--- Part 1-B Simulation %d: Iteration %d ---\n",simulationNumber,iteration);
    printf("Battleship position: (%.2f, %.2f)\n", B->x,B->y);

    if(simulationNumber == 2){
        printf("Battleship angle range: %.2f - %.2f\n",B->angleMin,B->angleMax);
    }

    //The battle engine now performs all projectile calculations and updates E[i].alive.
    BattleResult result =simulateBattleStep( B, E, N,rules,NULL,NULL,NULL,NULL,0,NULL);

    //  Show which escorts changed from alive to sunk during this iteration.     
    for(int i = 0; i < N; i++){
        if(wasAlive[i] == 1 && E[i].alive == 0){
            printf("Escort %d (Type: %s) was sunk.\n",E[i].id,E[i].type);
        }
    }


    printf("Escort ships sunk in this iteration: %d\n",
           result.sunkCount);


    /*
       Save results.
       We keep your current file-saving functions for now.
       File handling will be refactored later.
    */
    if(simulationNumber == 1){
        savePart1BSim1Iteration(B,E,N,iteration,result.sinkingEscort,result.battleshipDestroyed ? result.battleEndTime: -1.0,result.sunkCount,result.lastBattleshipHitTime);
    }

    else{
        savePart1BSim2Iteration(B,E,N,iteration,result.sinkingEscort,result.battleshipDestroyed? result.battleEndTime: -1.0,result.sunkCount,result.lastBattleshipHitTime,B->angleMin > 0.0);
    }


    if(result.battleshipDestroyed){
        printf("Battleship was sunk by Escort %d (Type: %s).\n",E[result.sinkingEscort].id,E[result.sinkingEscort].type);
        printf("Battleship sank at %.2f seconds.\n",result.battleEndTime);
        return 1;
    }

    printf("Battleship survived iteration %d.\n",iteration);
    return 0;
}


void simulatePart1B(Battleship *B,EscortShip E[],int N,double D)
{
    //   Get number of path points.
    int k = getMovementPoints("Part 1-B");
    Position path[k];
    generatePath(path, k, D);

    /*
       Save original state for Simulation 2.

       Simulation 2 must use the same initial battlefield
       and same path as Simulation 1.
    */
    Battleship BSim2 = *B;
    EscortShip ESim2[N];

    for(int i = 0; i < N; i++){
        ESim2[i] = E[i];
    }

 //simulation 1
    printf("\n--- Starting Part 1-B Simulation 1 ---\n");
    printf("\n--- Part 1-B Simulation 1 Path ---\n");

    for(int i = 0; i < k; i++){
        printf("Point %d: (%.2f, %.2f)\n",i + 1,path[i].x,path[i].y);
    }


    /*
       Create Simulation 1 result file.

       We leave this file logic here temporarily.
       It will be refactored later.
    */
    FILE *fp =fopen("part1B_sim1_results.txt", "w");

    if(fp == NULL){
        printf("Error creating part1B_sim1_results.txt\n");
    }

    else{
        fprintf(fp,"--- Part 1-B Simulation 1 Results ---\n");
        fprintf(fp,"\n--- Battleship Path ---\n");

        for(int i = 0; i < k; i++){
            fprintf(fp,"Point %d: (%.2f, %.2f)\n",i + 1,path[i].x,path[i].y);
        }

        fclose(fp);
    }


    
    //   Move B through the path. At each point, use the SAME reusable battle engine.
    
    for(int i = 0; i < k; i++){

        B->x = path[i].x;
        B->y = path[i].y;

        int battleshipDestroyed =runPart1BIteration(B,E,N,i + 1,1);

        if(battleshipDestroyed){
            printf("\nSimulation stopped at Point %d because B was destroyed.\n",i + 1);

            FILE *fp =fopen("part1B_sim1_results.txt", "a");

            if(fp != NULL){
                fprintf(fp,"\nSimulation stopped at Point %d because B was destroyed.\n",i + 1);
                fclose(fp);
            }
            break;
        }
    }


    //simulation 2
    int t;
    double jammedAngleMin;
    getJamSettings(k, &t, &jammedAngleMin);

    printf("\n--- Part 1-B Simulation 2 Settings ---\n");
    printf("Gun jams after iteration %d\n", t);
    printf("Jammed angle range: %.2f - 90.00 degrees\n",jammedAngleMin);

    FILE *fp2 =fopen("part1B_sim2_results.txt", "w");

    if(fp2 == NULL){
        printf("Error creating part1B_sim2_results.txt\n");
    }

    else{
        fprintf(fp2,"--- Part 1-B Simulation 2 Results ---\n");
        fprintf(fp2, "\nGun jams after iteration %d\n",t);
        fprintf(fp2,"Jammed angle range: %.2f - 90.00 degrees\n",jammedAngleMin);
        fprintf(fp2,"\n--- Battleship Path ---\n");

        for(int i = 0; i < k; i++){
            fprintf(fp2,"Point %d: (%.2f, %.2f)\n",i + 1,path[i].x,path[i].y);
        }

        fclose(fp2);
    }

    printf("\n--- Starting Part 1-B Simulation 2 ---\n");

    for(int i = 0; i < k; i++){
        //Use the SAME path as Simulation 1.
        BSim2.x = path[i].x;
        BSim2.y = path[i].y;
        /*
           First t iterations: normal 0 - 90 degree angle.
           Starting at iteration t + 1: jammedAngleMin - 90.
        */
        if(i >= t){

            BSim2.angleMin = jammedAngleMin;
            BSim2.angleMax = 90.0;

            if(i == t){
                printf("\nGun is JAMMED.\n");
                printf("New angle range: %.2f - %.2f degrees\n",BSim2.angleMin,BSim2.angleMax);
            }
        }

        int battleshipDestroyed =runPart1BIteration(&BSim2,ESim2,N,i + 1,2);

        if(battleshipDestroyed){
            printf("\nSimulation 2 stopped at Point %d " "because B was destroyed.\n",i + 1);
            FILE *fp2 =fopen("part1B_sim2_results.txt", "a");
            if(fp2 != NULL){
                fprintf(fp2,"\nSimulation 2 stopped at Point %d ""because B was destroyed.\n",i + 1);
                fclose(fp2);
            }
            break;
        }
    }
}