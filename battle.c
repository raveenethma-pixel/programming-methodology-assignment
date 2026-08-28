#include <stdio.h>
#include "battle.h"
#include "projectile.h"

void simulateBattle(Battleship *B, EscortShip E[], int N)
{
    double earliestEscortHitTime = -1.0;
    int sinkingEscort = -1;

    // Find the escort whose shell reaches B first
    for(int i = 0; i < N; i++)
    {
        if(canEscortHitBattleship(&E[i], B))
        {
            double distance = calculate_distance(E[i].x,E[i].y,B->x,B->y);

            double hitTime = calculateMinimumHitTime(distance,E[i].vMin,E[i].vMax,E[i].angleMin,E[i].angleMax);

            if(hitTime >= 0)
            {
                if(earliestEscortHitTime < 0 ||
                   hitTime < earliestEscortHitTime)
                {
                    earliestEscortHitTime = hitTime;
                    sinkingEscort = i;
                }
            }
        }
    }

    if(sinkingEscort != -1)
    {
        printf("\nBattleship was sunk by Escort %d (Type: %s).\n",E[sinkingEscort].id,E[sinkingEscort].type);
    }
    else
    {
        printf("\nBattleship survived the battle.\n");
    }

    printf("\n--- Escort Ships Sunk by Battleship ---\n");

    int sunkCount = 0;

    for(int i = 0; i < N; i++)
    {
        if(canBattleshipHitEscort(B, &E[i]))
        {
            double distance = calculate_distance(B->x,B->y,E[i].x,E[i].y);

            double hitTime = calculateMinimumHitTime(distance,B->vMin,B->vMax,B->angleMin,B->angleMax);

            if(hitTime >= 0)
            {
                if(earliestEscortHitTime < 0 || hitTime < earliestEscortHitTime)
                {
                    printf("Escort %d (Type: %s) was sunk.\n",E[i].id,E[i].type);

                    sunkCount++;
                }
            }
        }
    }

    printf("Total escort ships sunk: %d\n", sunkCount);
    printf("Battleship sinks at %.2f seconds\n",earliestEscortHitTime);
}