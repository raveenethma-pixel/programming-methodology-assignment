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
void savePart1CResults(Battleship *B,EscortShip E[],int N,double escortHitTimes[],double battleHitTimes[],int sunkCount,double battleEndTime){

    FILE *fp = fopen("part1C_results.txt", "w");

    if(fp == NULL){
        printf("Error creating part1C_results.txt\n");
        return;
    }

    fprintf(fp, "--- Part 1-C Results ---\n");
    fprintf(fp, "\nBattleship\n");
    fprintf(fp, "Type: %c\n", B->type);
    fprintf(fp, "Position: (%.2f, %.2f)\n", B->x, B->y);

    fprintf(fp, "\n--- Escort Attack Details ---\n");

    double cumulativeDamage = 0.0;

    for(int i = 0; i < N; i++) {
        fprintf(fp, "\nEscort %d\n", E[i].id);
        fprintf(fp, "Type: %s\n", E[i].type);

        if(escortHitTimes[i] >= 0){
            cumulativeDamage += E[i].impactPower;

            fprintf(fp, "Hit Battleship: YES\n");
            fprintf(fp,"Hit Time: %.2f seconds\n",escortHitTimes[i]);
            fprintf(fp,"Impact Power: %.2f\n",E[i].impactPower);
            fprintf(fp, "Cumulative Damage: %.2f%%\n",cumulativeDamage * 100.0);
        }
        else{
            fprintf(fp, "Hit Battleship: NO\n");
        }

        if(battleHitTimes[i] >= 0){
            fprintf(fp, "Destroyed by Battleship: YES\n");
            fprintf(fp,"Battleship Hit Time: %.2f seconds\n",battleHitTimes[i]);
        }
        else{
            fprintf(fp, "Destroyed by Battleship: NO\n");
        }
    }

    fprintf(fp, "\n--- Final Result ---\n");
    fprintf(fp,"Total Escorts Sunk: %d\n",sunkCount);
    fprintf(fp,"Final Cumulative Damage: %.2f%%\n",B->damage * 100.0);

    if(B->damage >= 1.0){
        fprintf(fp, "Battleship Status: SUNK\n");
    }
    else{
        fprintf(fp, "Battleship Status: SURVIVED\n");
    }
    fprintf(fp,"Battle End Time: %.2f seconds\n",battleEndTime);
    fclose(fp);
    printf("Detailed results saved to part1C_results.txt\n");
}
void startPart1CMovementResults(Position path[], int k){

    FILE *fp = fopen("part1C_movement_results.txt", "w");

    if(fp == NULL){
        printf("Error creating part1C_movement_results.txt\n");
        return;
    }
    fprintf(fp, "--- Part 1-C Movement Simulation Results ---\n");
    fprintf(fp, "\n--- Battleship Path ---\n");

    for(int i = 0; i < k; i++){
        fprintf(fp,"Point %d: (%.2f, %.2f)\n", i + 1,path[i].x,path[i].y);
    }

    fclose(fp);
}
void savePart1CMovementIteration(Battleship *B, EscortShip E[], int N, int iteration, double escortHitTimes[], double battleHitTimes[], int sunkThisIteration)
{
    FILE *fp = fopen("part1C_movement_results.txt", "a");

    if(fp == NULL){
        printf("Error opening part1C_movement_results.txt\n");
        return;
    }

    fprintf(fp, "\n--------------------------------\n");
    fprintf(fp, "Iteration %d\n", iteration);
    fprintf(fp, "--------------------------------\n");

    fprintf(fp,"Battleship Position: (%.2f, %.2f)\n",B->x,B->y);

    for(int i = 0; i < N; i++){
        fprintf(fp,"\nEscort %d | Type: %s | Status: %s\n", E[i].id,E[i].type,E[i].alive ? "ALIVE" : "SUNK");

        if(escortHitTimes[i] >= 0){
            fprintf(fp,"Hit Battleship: YES | Hit Time: %.2f seconds | Impact Power: %.2f\n",escortHitTimes[i],E[i].impactPower);
        }
        else{
            fprintf(fp, "Hit Battleship: NO\n");
        }

        if(battleHitTimes[i] >= 0){
            fprintf(fp, "Hit by Battleship: YES | Hit Time: %.2f seconds\n",battleHitTimes[i]);
        }
        else{
            fprintf(fp, "Hit by Battleship: NO\n");
        }
    }

    fprintf(fp, "\nEscorts sunk during this iteration: %d\n",sunkThisIteration);
    fprintf(fp,"Cumulative damage on Battleship: %.2f%%\n",B->damage * 100.0);

    if(B->damage >= 1.0){
        fprintf(fp, "Battleship Status: SUNK\n");
    }
    else{
        fprintf(fp, "Battleship Status: SURVIVED\n");
    }

    fclose(fp);
}
void startPart1CMovementSim2Results(Position path[],int k,int t,double jammedAngleMin){
    FILE *fp = fopen("part1C_movement_sim2_results.txt", "w");

    if(fp == NULL){
        printf("Error creating part1C_movement_sim2_results.txt\n");
        return;
    }

    fprintf(fp, "--- Part 1-C Movement Simulation 2 Results ---\n");

    fprintf(fp, "\nGun jams after iteration %d\n", t);
    fprintf(fp,"Jammed angle range: %.2f - 90.00 degrees\n",jammedAngleMin);

    fprintf(fp, "\n--- Battleship Path ---\n");

    for(int i = 0; i < k; i++){
        fprintf(fp,"Point %d: (%.2f, %.2f)\n",i + 1,path[i].x,path[i].y);
    }

    fclose(fp);
}
void savePart1CMovementSim2Iteration(Battleship *B, EscortShip E[], int N,int iteration, double escortHitTimes[], double battleHitTimes[], int sunkThisIteration)
{
    FILE *fp = fopen("part1C_movement_sim2_results.txt", "a");

    if(fp == NULL){
        printf("Error opening part1C_movement_sim2_results.txt\n");
        return;
    }

    fprintf(fp, "\n--------------------------------\n");
    fprintf(fp, "Iteration %d\n", iteration);
    fprintf(fp, "--------------------------------\n");

    fprintf(fp,"Battleship Position: (%.2f, %.2f)\n", B->x,B->y);

    fprintf(fp,"Battleship Angle Range: %.2f - %.2f degrees\n",B->angleMin,B->angleMax);

    for(int i = 0; i < N; i++){

        fprintf(fp, "\nEscort %d | Type: %s | Status: %s\n",E[i].id,E[i].type,E[i].alive ? "ALIVE" : "SUNK");

        if(escortHitTimes[i] >= 0){
            fprintf(fp,"Hit Battleship: YES | Hit Time: %.2f seconds | Impact Power: %.2f\n",escortHitTimes[i],E[i].impactPower);
        }
        else{
            fprintf(fp, "Hit Battleship: NO\n");
        }

        if(battleHitTimes[i] >= 0){
            fprintf(fp,"Hit by Battleship: YES | Hit Time: %.2f seconds\n",battleHitTimes[i]);
        }
        else{
            fprintf(fp, "Hit by Battleship: NO\n");
        }
    }

    fprintf(fp, "\nEscorts sunk during this iteration: %d\n", sunkThisIteration);
    fprintf(fp, "Cumulative damage on Battleship: %.2f%%\n",B->damage * 100.0);

    if(B->damage >= 1.0){
        fprintf(fp, "Battleship Status: SUNK\n");
    }
    else{
        fprintf(fp, "Battleship Status: SURVIVED\n");
    }

    fclose(fp);
}
void savePart2APart1AResults(Battleship *B, EscortShip E[], int N, double reloadTime, int attackOrder[], int attackCount,double fireTimes[], double hitTimes[], int sunkCount,int sinkingEscort,double earliestEscortHitTime)
{
    FILE *fp = fopen("part2A_part1A_results.txt", "w");

    if(fp == NULL){
        printf("Error creating part2A_part1A_results.txt\n");
        return;
    }

    fprintf(fp, "--- Part 2-A / Part 1-A Results ---\n");

    fprintf(fp,"\nBattleship Type: %c\n", B->type);
    fprintf(fp,"Battleship Position: (%.2f, %.2f)\n", B->x,B->y);

    fprintf(fp,"Battleship Reload Time T_B: %.2f seconds\n",reloadTime);
    fprintf(fp, "\n--- Battleship Attack Order ---\n");

    if(attackCount == 0){
        fprintf(fp, "No escorts were within Battleship attack range.\n");
    }
    else{
        for(int i = 0; i < attackCount; i++){
            int index = attackOrder[i];
            fprintf(fp, "Attack %d -> Escort %d | Type: %s\n", i + 1, E[index].id,E[index].type);
        }
    }


    fprintf(fp, "\n--- Attack Timing ---\n");

    for(int i = 0; i < attackCount; i++){

        int index = attackOrder[i];

        if(fireTimes[i] >= 0.0){
            fprintf(fp,"Escort %d | Fire Time: %.2f s | Hit Time: %.2f s\n",E[index].id,fireTimes[i],hitTimes[i]);
        }
        else{
            fprintf(fp, "Escort %d | NOT FIRED AT\n",E[index].id);
        }
    }


    fprintf(fp,"\nTotal Escorts Sunk: %d\n",sunkCount);


    if(sinkingEscort != -1){

        fprintf(fp,"Battleship Status: SUNK\n");
        fprintf(fp,"Sunk by Escort %d | Type: %s\n",E[sinkingEscort].id,E[sinkingEscort].type);
        fprintf(fp,"Battleship Sink Time: %.2f seconds\n",earliestEscortHitTime);
    }
    else{
        fprintf(fp, "Battleship Status: SURVIVED\n");
    }

    fclose(fp);

    printf("Detailed results saved to part2A_part1A_results.txt\n");
}
    

