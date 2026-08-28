#include <stdio.h>
#include "files.h"

void saveInitialConditions(Battleship *B, EscortShip E[], int N, double D)
{
    FILE *fp = fopen("initial_conditions.txt", "w");//file pointer with writing

    if(fp == NULL)
    {
        printf("Error creating initial_conditions.txt\n");
        return;
    }

    fprintf(fp, "--- Initial Battlefield Conditions ---\n");//start of the file

    fprintf(fp, "\nBattlefield size: %.2f x %.2f\n", D, D);//print the details about the battleship

    fprintf(fp, "\nBattleship\n");
    fprintf(fp, "Type: %c\n", B->type);
    fprintf(fp, "Position: (%.2f, %.2f)\n", B->x, B->y);
    fprintf(fp, "Minimum Velocity: %.2f\n", B->vMin);
    fprintf(fp, "Maximum Velocity: %.2f\n", B->vMax);
    fprintf(fp, "Minimum Angle: %.2f\n", B->angleMin);
    fprintf(fp, "Maximum Angle: %.2f\n", B->angleMax);

    fprintf(fp, "\nEscort Ships\n");//print the detais about the escort ships

    for(int i = 0; i < N; i++)
    {
        fprintf(fp,"ID: %d | Type: %s | Position: (%.2f, %.2f) | Velocity: (%.2f - %.2f) | Angle: (%.2f - %.2f) | Impact Power: %.2f\n",
                E[i].id,
                E[i].type,
                E[i].x,
                E[i].y,
                E[i].vMin,
                E[i].vMax,
                E[i].angleMin,
                E[i].angleMax,
                E[i].impactPower);
    }

    fclose(fp);

    printf("Initial conditions saved to initial_conditions.txt\n");
}