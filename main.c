#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include "structures.h"
#include "battleship.h"
#include "escort.h"
#include "projectile.h"
#include "part1A.h"
#include "files.h"
#include "part1B.h"
#include "part1C.h"
#include "part2A.h"

#define MAX_ESCORTS 100



int main(){
    srand(time(NULL));//make sure that random numbers are given using the current time
    Battleship B;
    EscortShip E[MAX_ESCORTS];
    int N;//Number of escort ships
    double D;//the upper limit of the canvas

    //Get the user inputs
    printf("Enter battlefield size D: ");
    scanf("%lf",&D);
    printf("Enter the number of escort ships N(less than 100) ");
    scanf("%d",&N);
    //Make sure the inputes are valid
    if(D<=0){
        printf("Invalid battlefield size D. It must be greater than 0.\n");
        return 1;
    }
    if(N<=0 || N>MAX_ESCORTS){
        printf("Invalid number of escort ships N. It must be between 1 and %d.\n", MAX_ESCORTS);
        return 1;
    }
    initializeBattleship(&B,D);
    initializeEscortShips(E,N,D,B.vMax);
    Battleship originalB = B;
    EscortShip originalE[N];

    for(int i = 0; i < N; i++){
        originalE[i] = E[i];
    }

    //Print the initialized escort ships
    printf("\n---Escort Ships---\n");
    for(int i=0;i<N;i++){
        printf("ID: %d | Type: %s | Position: (%.2f,%.2f) | Velocity: (%.2f-%.2f) | Angle: (%.2f-%.2f) | Impact Power: %.2f\n",E[i].id,E[i].type,E[i].x,E[i].y,E[i].vMin,E[i].vMax,E[i].angleMin,E[i].angleMax,E[i].impactPower);
    }

    //print the distance of each escort ship from the battleship
    printf("\n---Distances from the battleship---\n");
    for(int i=0;i<N;i++){
        double distance = calculate_distance(B.x,B.y,E[i].x,E[i].y);
        printf("Escort %d (Type: %s): %.2f\n",E[i].id,E[i].type,distance);
    }

    printf("\n---Can Escort Ships Hit the Battleship?---\n");
    for(int i=0;i<N;i++){
        if(canEscortHitBattleship(&E[i],&B)){
            printf("Escort %d (Type: %s) can hit the battleship.\n",E[i].id,E[i].type);
        }
        else{
            printf("Escort %d (Type: %s) cannot hit the battleship.\n",E[i].id,E[i].type);
        }
    }

    printf("\n---Can the Battleship Hit the Escort Ships?---\n");
    for(int i=0;i<N;i++){
        if(canBattleshipHitEscort(&B,&E[i])){
            printf("The battleship can hit Escort %d (Type: %s).\n",E[i].id,E[i].type);
        }
        else{
            printf("The battleship cannot hit Escort %d (Type: %s).\n",E[i].id,E[i].type);
        }
    }
    saveInitialConditions(&B, E, N, D);


    // -------------------- Part 1-A --------------------
    B = originalB;

    for(int i = 0; i < N; i++){
        E[i] = originalE[i];
    }

    simulatePart1A(&B, E, N);


    // -------------------- Part 1-B --------------------
    B = originalB;               //this piece of code take a copy of the original values in the structs. So that any change done by the above codes will have no effect.
                                //Strats from the initial values and conditions
    for(int i = 0; i < N; i++){
        E[i] = originalE[i];
    }

    simulatePart1B(&B, E, N, D);


    // -------------------- Part 1-C --------------------
    B = originalB;

    for(int i = 0; i < N; i++){
        E[i] = originalE[i];
    }

    simulatePart1C(&B, E, N);


    // -------- Part 1-C Movement Simulation --------
    B = originalB;
    for(int i = 0; i < N; i++){
        E[i] = originalE[i];
    }
    simulatePart1CMovement(&B, E, N, D);

    //part 2 A
    B = originalB;
    for(int i = 0; i < N; i++){
        E[i] = originalE[i];
    }
    simulatePart2A(&B, E, N, D);
    return 0;


}
