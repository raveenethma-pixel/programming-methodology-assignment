#include<stdio.h>
#include<stdlib.h>

#include "part1B.h"
#include "projectile.h"
#include "files.h"

void generatePath(Position path[], int k, double D){
    for(int i=0;i<k;i++){ //get the k (number of positions) from the user
        path[i].x= 1.0 + ((double)rand()/ RAND_MAX)*(D - 2.0); //for each position generate random x y values
        path[i].y= 1.0 + ((double)rand() / RAND_MAX) * (D - 2.0);
    }
}

void simulatePart1B(Battleship *B,EscortShip E[],int N,double D){
    int k;
    //make sure that the useer input a positive int
    do{
        printf("\nEnter the number of points you want the battleship to go: ");
        if(scanf("%d",&k)!=1){
            printf("Invalid input.Enter a number(int)\n");
            while(getchar()!='\n');
            k=-1;
            continue;
        }
        if(k<=0){
            printf("k(num of paths)must be >0\n");
        }
    }while(k<=0);

    Position path[k];
    generatePath(path,k,D);//generate the paths
    //Save original conditions for simulation 2
    Battleship BSim2 = *B;
    EscortShip ESim2[N];
    for(int i = 0; i < N; i++){
        ESim2[i] = E[i];
    }


    printf("\n---Starting part 1 B simulation 1---\n");
    printf("\n--- Part 1-B Simulation 1 Path ---\n");
    for(int i=0;i<k;i++){//for each k print the generated positions
        printf("Point %d: (%.2f, %.2f)\n",i+1,path[i].x,path[i].y);
    }
    //creat part 1 B result file
    FILE *fp = fopen("part1B_sim1_results.txt", "w");//write a new for every time the program started
    if(fp == NULL){
        printf("Error creating part1B_sim1_results.txt\n");
        }
    else{
        fprintf(fp, "--- Part 1-B Simulation 1 Results ---\n");
        fprintf(fp, "\n--- Battleship Path ---\n");

        for(int i = 0; i < k; i++){
            fprintf(fp,"Point %d: (%.2f, %.2f)\n",i + 1,path[i].x,path[i].y);
            }
        fclose(fp);
        }
    //Run the battle at each point generated
    for(int i = 0; i < k; i++){
        B->x = path[i].x;
        B->y = path[i].y;

        int battleshipDestroyed = simulatePart1BStep(B, E, N, i + 1);//ask if the battleship is destroyed

        if(battleshipDestroyed == 1){ //if the battleship got killed stop
            printf("\nSimulation stopped at Point %d because B was destroyed.\n",i + 1);
            FILE *fp = fopen("part1B_sim1_results.txt", "a");
            if(fp!= NULL){
              fprintf(fp,"\nSimulation stopped at Point %d because B was destroyed.\n",i + 1); 
              fclose(fp);
            }
            break;  
            
        }
    }

    int t;//get the iteretion in which the B's gun jams

    do{
        printf("\nEnter the iteration after which the gun jams (t < k): ");
        //check whether the user enters a valid iteration(t>0 and t<=k)
        if(scanf("%d", &t) != 1){
            printf("Invalid input. Enter an integer.\n");
            while(getchar() != '\n');
            t = -1;
            continue;
        }

        if(t <= 0 || t >= k){
            printf("t must be greater than 0 and less than %d\n", k);
        }

    }while(t <= 0 || t >= k);

    double jammedAngleMin; //get the jammed angle and check if it's in the range(0<30)

    do{
        printf("Enter the minimum jammed gun angle (0 < angle < 30): ");
        if(scanf("%lf", &jammedAngleMin) != 1){
            printf("Invalid input. Enter a number.\n");
            while(getchar() != '\n');

            jammedAngleMin = -1.0;
            continue;
        }

        if(jammedAngleMin <= 0 || jammedAngleMin >= 30){
            printf("Angle must be greater than 0 and less than 30 degrees.\n");
        }

    }while(jammedAngleMin <= 0 || jammedAngleMin >= 30);


    printf("\n--- Part 1-B Simulation 2 Settings ---\n");
    printf("Gun jams after iteration %d\n", t);
    printf("Jammed angle range: %.2f - 90.00 degrees\n",jammedAngleMin);
    
    //save the part 1 B sim 2
    FILE *fp2 = fopen("part1B_sim2_results.txt", "w");
    if(fp2 != NULL){
        fprintf(fp2, "--- Part 1-B Simulation 2 Results ---\n");

        fprintf(fp2, "\nGun jams after iteration %d\n", t);
        fprintf(fp2,"Jammed angle range: %.2f - 90.00 degrees\n",jammedAngleMin);
        fprintf(fp2, "\n--- Battleship Path ---\n");
        for(int i = 0; i < k; i++){
            fprintf(fp2,"Point %d: (%.2f, %.2f)\n",i + 1,path[i].x,path[i].y);
        }

        fclose(fp2);
    }
    printf("\n--- Starting Part 1-B Simulation 2 ---\n");

    for(int i = 0; i < k; i++){
        int gunJammed = 0;
        // Move B to the SAME path point used in Simulation 1
        BSim2.x = path[i].x;
        BSim2.y = path[i].y;

        // Gun jams only AFTER t iterations
        if(i >= t){
            gunJammed = 1;
            BSim2.angleMin = jammedAngleMin;
            BSim2.angleMax = 90.0;

            printf("\nGun is JAMMED.\n");
            printf("New angle range: %.2f - %.2f degrees\n",BSim2.angleMin,BSim2.angleMax);
        }

        int battleshipDestroyed = simulatePart1BSim2Step(&BSim2, ESim2, N, i + 1);

        if(battleshipDestroyed == 1){
            printf("\nSimulation 2 stopped at Point %d because B was destroyed.\n",i + 1);
            FILE *fp2 = fopen("part1B_sim2_results.txt", "a");
            if(fp2 != NULL){
                fprintf(fp2,"\nSimulation 2 stopped at Point %d because B was destroyed.\n",i + 1);
                fclose(fp2);
            }
            break;
        }
    }  
}   
int simulatePart1BStep(Battleship *B,EscortShip E[],int N,int iteration){
    double earliestEscortHitTime = -1.0;//no valid escort hit time found yet(variable represents the earliest time an escort shell reaches B.)
    int sinkingEscort = -1;
    double iterationEndTime = 0.0;

    printf("\n--- Part 1-B Simulation 1: Iteration %d ---\n",iteration);
    printf("Battleship position: (%.2f, %.2f)\n",B->x,B->y);
    // Find the fastest living escort that can hit B
    for(int i = 0; i < N; i++){
        if(E[i].alive == 0){ //if an escort is sunken skip it
            continue;
        }

        if(canEscortHitBattleship(&E[i], B)){
            double distance = calculate_distance(E[i].x,E[i].y,B->x,B->y);
            double hitTime = calculateMinimumHitTime(distance,E[i].vMin,E[i].vMax,E[i].angleMin,E[i].angleMax);

            if(hitTime >= 0){
                if(earliestEscortHitTime < 0 || hitTime < earliestEscortHitTime){
                    earliestEscortHitTime = hitTime;
                    sinkingEscort = i;
                }
            }
        }
    }
    int sunkThisIteration = 0;//How many escorts sank in this iteration

    for(int i = 0; i < N; i++){
        if(E[i].alive == 0){
            continue;
        }

        if(canBattleshipHitEscort(B, &E[i])){
            double distance = calculate_distance(B->x,B->y,E[i].x,E[i].y);
            double hitTime =calculateMinimumHitTime(distance,B->vMin,B->vMax,B->angleMin,B->angleMax);

            if(hitTime >= 0){
                if(earliestEscortHitTime < 0 || hitTime < earliestEscortHitTime){
                    printf("Escort %d (Type: %s) was sunk.\n",E[i].id,E[i].type);
                    E[i].alive = 0;
                    sunkThisIteration++;

                    if(hitTime > iterationEndTime){
                        iterationEndTime = hitTime;
                    }
                }
            }
        }
    }
    printf("Escort ships sunk in this iteration: %d\n",sunkThisIteration);//print the number of escort that got sunk
    savePart1BSim1Iteration(B,E,N,iteration,sinkingEscort,earliestEscortHitTime,sunkThisIteration,iterationEndTime);
    savePart1BSim2Iteration(B,E, N,iteration,sinkingEscort,earliestEscortHitTime,sunkThisIteration,iterationEndTime,B->angleMin > 0.0);
    if(sinkingEscort != -1){
        printf("Battleship was sunk by Escort %d (Type: %s).\n",E[sinkingEscort].id,E[sinkingEscort].type);
        printf("Battleship sank at %.2f seconds.\n",earliestEscortHitTime);
        return 1;//means B died
    }

    printf("Battleship survived iteration %d.\n",iteration);

    return 0;//means B survived
}

int simulatePart1BSim2Step(Battleship *B,EscortShip E[],int N,int iteration){
    double earliestEscortHitTime = -1.0;
    int sinkingEscort = -1;
    double iterationEndTime = 0.0;

    printf("\n--- Part 1-B Simulation 2: Iteration %d ---\n", iteration);
    printf("Battleship position: (%.2f, %.2f)\n", B->x,B->y);
    printf("Battleship angle range: %.2f - %.2f\n",B->angleMin,B->angleMax);

    // Find fastest living escort that can hit B
    for(int i = 0; i < N; i++){
        if(E[i].alive == 0){
            continue;
        }

        if(canEscortHitBattleship(&E[i], B)){
            double distance = calculate_distance(E[i].x,E[i].y,B->x,B->y);
            double hitTime =calculateMinimumHitTime(distance,E[i].vMin,E[i].vMax,E[i].angleMin,E[i].angleMax);

            if(hitTime >= 0){
                if(earliestEscortHitTime < 0 || hitTime < earliestEscortHitTime){
                    earliestEscortHitTime = hitTime;
                    sinkingEscort = i;
                }
            }
        }
    }

    int sunkThisIteration = 0;

    // Battleship attacks living escorts
    for(int i = 0; i < N; i++){
        if(E[i].alive == 0){
            continue;
        }

        if(canBattleshipHitEscort(B, &E[i])){
            double distance = calculate_distance(B->x,B->y,E[i].x,E[i].y);
            double hitTime = calculateMinimumHitTime(distance,B->vMin,B->vMax,B->angleMin,B->angleMax);

            if(hitTime >= 0){
                if(earliestEscortHitTime < 0 || hitTime < earliestEscortHitTime){
                    printf("Escort %d (Type: %s) was sunk.\n", E[i].id,E[i].type);
                    E[i].alive = 0;
                    sunkThisIteration++;

                    if(hitTime > iterationEndTime){
                        iterationEndTime = hitTime;
                    }
                }
            }
        }
    }

    printf("Escort ships sunk in this iteration: %d\n", sunkThisIteration);
    savePart1BSim2Iteration(B,E, N,iteration,sinkingEscort,earliestEscortHitTime,sunkThisIteration,iterationEndTime,B->angleMin > 0.0);
    if(sinkingEscort != -1){
        printf("Battleship was sunk by Escort %d (Type: %s).\n", E[sinkingEscort].id,E[sinkingEscort].type);
        printf("Battleship sank at %.2f seconds.\n",earliestEscortHitTime);
        return 1;
    }

    printf("Battleship survived iteration %d.\n",iteration);
    return 0;
}