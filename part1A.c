
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
    BattleResult result = simulateBattleStep(B,E,N,rules,NULL,NULL,NULL,NULL,0,NULL);

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

    */
    saveFinalConditions( B, E,N,result.sinkingEscort,result.battleEndTime,result.sunkCount);
}

