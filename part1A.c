/* #include <stdio.h>
#include "part1A.h"
#include "projectile.h"
#include "files.h"


void simulatePart1A(Battleship *B, EscortShip E[], int N)
{
    double earliestEscortHitTime = -1.0;
    int sinkingEscort = -1;//no sinking escort has been found yet
    double battleEndTime = 0.0;


    // Find the escort whose shell reaches B first/who sinks B
    for(int i = 0; i < N; i++)
    {
        if(canEscortHitBattleship(&E[i], B))
        {
            double distance = calculate_distance(E[i].x,E[i].y,B->x,B->y);

            double hitTime = calculateMinimumHitTime(distance,E[i].vMin,E[i].vMax,E[i].angleMin,E[i].angleMax);

            if(hitTime >= 0)
            {
                if(earliestEscortHitTime < 0 || hitTime < earliestEscortHitTime) //if  earliestEscortHitTime < 0 is true it means earliestEscortHitTime is -1(initialized)
                 //but in the 2nd iteration we have the hitTime of the first escort ship
                //So earliestEscortHitTime < 0 becomes false and we compare whether hitTime < earliestEscortHitTime 
                {
                    earliestEscortHitTime = hitTime;//if so it is the fastest hit time
                    sinkingEscort = i;//get the index of the escort that sink B
                }
            }
        }
    } 

    if(sinkingEscort != -1)
    {
        printf("\nBattleship %c was sunk by Escort %d (Type: %s).\n",B->type,E[sinkingEscort].id,E[sinkingEscort].type);
        printf("Battleship sinks at %.2f seconds\n", earliestEscortHitTime);
        printf("Battle ended at %.2f seconds.\n",earliestEscortHitTime);
    }
    else
    {
        printf("\nBattleship survived the battle.\n");
    }

    printf("\n--- Escort Ships Sunk by Battleship ---\n");

    int sunkCount = 0;


    //
    FILE *hitFile = fopen("partA1_escort_sunk_by_B.txt","w");
    if(hitFile==NULL){
        printf("Error creating the escort_sunk_by_B.txt\n");
        }
    for(int i = 0; i < N; i++)
    {
        if(canBattleshipHitEscort(B, &E[i]))
        {
            double distance = calculate_distance(B->x,B->y,E[i].x,E[i].y);
            //hitTime = the time it takes to a shell from an battleship to hit the escort ship 
            double hitTime = calculateMinimumHitTime(distance,B->vMin,B->vMax,B->angleMin,B->angleMax);

            if(hitTime >= 0)
            {
                if(earliestEscortHitTime < 0 || hitTime < earliestEscortHitTime) //if the escort didn't hit or battleship hit escort before it sunk the battleship
                {
                    printf("Escort %d (Type: %s) was sunk.\n",E[i].id,E[i].type);
                    E[i].alive = 0;
                    sunkCount++;
                
                    if(hitTime > battleEndTime)
                    {
                        battleEndTime=hitTime;
                    }
                    if(hitFile!=NULL)
                    {
                        fprintf(hitFile,"Escort %d | Type %s | Hit Time %.2f seconds\n",E[i].id,E[i].type,hitTime);
                    }
                }
            }
        }
    }
    if(sunkCount == 0 && hitFile!= NULL)
    {
        fprintf(hitFile,"No escort ship was sunk\n");
    }
    if(hitFile != NULL)
    {
    fclose(hitFile);
    }

    if(sunkCount == 0){  //sunkCount=how many escort ships the battleship has destroyed
        printf("None\n");
    }
    printf("Total escort ships sunk: %d\n", sunkCount);
    if(sinkingEscort == -1){
        
          //if no escort ship was sunken and the battleship i
        printf("Battle Ended at %.2f\n",battleEndTime);
    }
    saveFinalConditions(B,E,N,sinkingEscort,earliestEscortHitTime,sunkCount);

    
} 
*/
#include <stdio.h>

#include "part1A.h"
#include "battle_engine.h"
#include "files.h"


void simulatePart1A(Battleship *B, EscortShip E[], int N)
{
    /*
       Part 1-A rules:

       - No reload time between B's shots.
       - One successful escort hit destroys B.
       - One successful B hit destroys an escort.
       - Each escort only needs to be considered once
         because this is a single-position simulation.
    */
    BattleRules rules;

    rules.cumulativeDamage = 0;

    rules.useBattleshipReload = 0;
    rules.battleshipReloadTime = 0.0;


    /*
    Run the actual battle using the reusable battle engine.
    fired = NULL because Part 1-A only performs one battle at one position. We do not need to remember firing state across multiple iterations.
    */
    BattleResult result = simulateBattleStep(B,E,N,rules,NULL,NULL,NULL);

    //   Display result.
    if(result.battleshipDestroyed){

        printf("\nBattleship %c was sunk by Escort %d (Type: %s).\n", B->type, E[result.sinkingEscort].id, E[result.sinkingEscort].type);
        printf("Battleship sinks at %.2f seconds.\n", result.battleEndTime);
        printf("Battle ended at %.2f seconds.\n",result.battleEndTime);
    }

    else{
        printf("\nBattleship survived the battle.\n");
    }

    //Display escorts destroyed by B.
    printf("\n--- Escort Ships Sunk by Battleship ---\n");
    //sunkCount = how many escort ships the battleship destroyed
    if(result.sunkCount == 0){ 
        printf("None\n");
    }

    else{
        for(int i = 0; i < N; i++){
            if(E[i].alive == 0){ 
                printf("Escort %d (Type: %s) was sunk.\n",E[i].id,E[i].type);
            }
        }
    }

    printf("Total escort ships sunk: %d\n",
           result.sunkCount);

    //If B survived, the battle end time is the time of the last successful B shell.
    
    if(!result.battleshipDestroyed){

        printf("Battle Ended at %.2f\n",result.battleEndTime);
    }


    /*
       Keep using the existing final-condition file function
       for now.

       We will refactor file handling later.
    */
    saveFinalConditions( B, E,N,result.sinkingEscort,result.battleEndTime,result.sunkCount);
}

