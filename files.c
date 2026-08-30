#include <stdio.h>
#include "files.h"

void saveInitialConditions(Battleship *B, EscortShip E[], int N, double D)
{
    FILE *fp = fopen("partA1_initial_conditions.txt", "w");//file pointer with writing

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

    printf("Initial conditions saved to partA1_initial_conditions.txt\n");
}
//Final conditions file part 1 A
void saveFinalConditions(Battleship *B,EscortShip E[],int N,int sinkingEscort,double earliestEscortHitTime,int sunkCount){
    FILE *fp = fopen("partA1_final_conditions.txt", "w");
    if(fp == NULL)
    {
        printf("Error creating final_conditions.txt\n");
        return;
    }
    fprintf(fp, "--- Final Battlefield Conditions ---\n");
    fprintf(fp, "\nBattleship\n");
    fprintf(fp, "Type: %c\n", B->type);
    fprintf(fp, "Position: (%.2f, %.2f)\n", B->x, B->y);
    if(sinkingEscort != -1)
    {
        fprintf(fp, "Status: SUNK\n");
        fprintf(fp,"Sunk by Escort %d (Type: %s)\n",E[sinkingEscort].id,E[sinkingEscort].type);
        fprintf(fp,"Battle ended at %.2f seconds\n",earliestEscortHitTime);
    }
    else{
      fprintf(fp, "Status: SURVIVED\n");  
    }
    fprintf(fp,"\nTotal escort ships sunk by battleship: %d\n",sunkCount);
     fclose(fp);
    printf("Final conditions saved to partA1_final_conditions.txt\n");
}

//Part 1 B simulation 1 file
void savePart1BSim1Iteration(Battleship *B,EscortShip E[],int N,int iteration,int sinkingEscort,double earliestEscortHitTime,int sunkThisIteration,double iterationEndTime){

    FILE *fp = fopen("part1B_sim1_results.txt", "a");//for every iteration we need to append the data

    if(fp == NULL){
        printf("Error creating part1B_sim1_results.txt\n");
        return;
    }
    fprintf(fp, "\n--------------------------------\n");
    fprintf(fp, "Iteration %d\n", iteration);
    fprintf(fp, "--------------------------------\n");

    fprintf(fp, "Battleship Position: (%.2f, %.2f)\n",B->x, B->y);
    fprintf(fp, "Battleship Type: %c\n", B->type);
    fprintf(fp, "\nEscort Ship Conditions:\n");

    for(int i = 0; i < N; i++)
    {
        fprintf(fp,"ID: %d | Type: %s | Position: (%.2f, %.2f) | Status: %s\n",E[i].id,E[i].type,E[i].x,E[i].y,E[i].alive ? "ALIVE" : "SUNK");
    }

    fprintf(fp, "\nEscort ships sunk in this iteration: %d\n",sunkThisIteration);

    if(sinkingEscort != -1){
        fprintf(fp, "Battleship Status: SUNK\n");
        fprintf(fp, "Sunk by Escort %d (Type: %s)\n",E[sinkingEscort].id,E[sinkingEscort].type);
        fprintf(fp, "Battleship sank at %.2f seconds\n",earliestEscortHitTime);
    }
    else{
        fprintf(fp, "Battleship Status: SURVIVED\n");
        fprintf(fp, "Iteration ended at %.2f seconds\n",iterationEndTime);
    }

    fclose(fp);
}
//save part 1 B simulation 2 to a file
void savePart1BSim2Iteration(Battleship *B,EscortShip E[],int N,int iteration,int sinkingEscort,double earliestEscortHitTime,int sunkThisIteration,double iterationEndTime,int gunJammed){
    FILE *fp = fopen("part1B_sim2_results.txt", "a");
    if(fp == NULL){
        printf("Error opening part1B_sim2_results.txt\n");
        return;
    }

    fprintf(fp, "\n--------------------------------\n");
    fprintf(fp, "Iteration %d\n", iteration);
    fprintf(fp, "--------------------------------\n");
    fprintf(fp, "Battleship Position: (%.2f, %.2f)\n",B->x, B->y);
    fprintf(fp, "Battleship Type: %c\n", B->type);

    if(gunJammed == 1){
        fprintf(fp, "Gun Status: JAMMED\n");
    }
    else{
        fprintf(fp, "Gun Status: NORMAL\n");
    }
    fprintf(fp, "Angle Range: %.2f - %.2f\n", B->angleMin,B->angleMax);
    fprintf(fp, "\nEscort Ship Conditions:\n");

    for(int i = 0; i < N; i++){
        fprintf(fp,"ID: %d | Type: %s | Position: (%.2f, %.2f) | Status: %s\n",E[i].id,E[i].type,E[i].x,E[i].y,E[i].alive ? "ALIVE" : "SUNK");
    }
    fprintf(fp,"\nEscort ships sunk in this iteration: %d\n",sunkThisIteration);

    if(sinkingEscort != -1){
        fprintf(fp, "Battleship Status: SUNK\n");
        fprintf(fp,"Sunk by Escort %d (Type: %s)\n",E[sinkingEscort].id,E[sinkingEscort].type);
        fprintf(fp,"Battleship sank at %.2f seconds\n",earliestEscortHitTime);
    }
    else{
        fprintf(fp, "Battleship Status: SURVIVED\n");
        fprintf(fp,"Iteration ended at %.2f seconds\n",iterationEndTime);
    }

    fclose(fp);
}
    

