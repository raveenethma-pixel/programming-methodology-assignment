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
#include "part2B.h"

#define MAX_ESCORTS 100

static int getMenuChoice(int min, int max);

static void setupBattlefield(Battleship *B,EscortShip E[],int *N,double *D,Battleship *originalB,EscortShip originalE[]);

static void runSimulationMenu(Battleship *B, EscortShip E[], int N, double D, Battleship originalB,EscortShip originalE[]);

static void showInstructions(void);

int main()
{
    srand(time(NULL));

    Battleship B;
    Battleship originalB;

    EscortShip E[MAX_ESCORTS];
    EscortShip originalE[MAX_ESCORTS];

    int N = 0;
    double D = 0.0;

    int setupDone = 0;
    int mainChoice;

    do{
        printf("\n========================================\n");
        printf("     ADVANCED NAVAL BATTLE SIMULATOR\n");
        printf("========================================\n");
        printf("1. Start Simulation\n");
        printf("2. View Instructions\n");
        printf("3. Simulation Statistics\n");
        printf("4. Exit\n");

        mainChoice = getMenuChoice(1, 4);

        switch(mainChoice){

            case 1:{
                int startChoice;
                do{
                    printf("\n--- Start Simulation ---\n");
                    printf("1. Setup Battlefield\n");
                    printf("2. Run Simulation\n");
                    printf("3. Return to Main Menu\n");

                    startChoice = getMenuChoice(1, 3);

                    if(startChoice == 1){
                        setupBattlefield(&B,E,&N,&D,&originalB,originalE);
                        setupDone = 1;
                    }

                    else if(startChoice == 2){
                        if(!setupDone){
                            printf("\nPlease setup the battlefield first.\n");
                        }
                        else{
                            runSimulationMenu( &B, E, N, D, originalB, originalE );
                        }
                    }

                }while(startChoice != 3);
                break;
            }

            case 2:
                showInstructions();
                break;

            case 3:
                printf("\nSimulation statistics will be added next.\n");
                break;

            case 4:
                printf("\nExiting simulator.\n");
                break;
        }

    }while(mainChoice != 4);
    return 0;
}
static int getMenuChoice(int min, int max)
{
    int choice;
    while(1){
        printf("Enter choice: ");
        if(scanf("%d", &choice) != 1){
            printf("Invalid input. Enter a number.\n");
            while(getchar() != '\n');
            continue;
        }

        if(choice < min || choice > max){
            printf( "Choice must be between %d and %d.\n", min,max);
            continue;
        }
        return choice;
    }
}
static void setupBattlefield( Battleship *B,EscortShip E[],int *N,double *D,Battleship *originalB,EscortShip originalE[])
{
    do{
        printf("\nEnter battlefield size D: ");

        if(scanf("%lf", D) != 1){
            printf("Invalid input. Enter a number.\n");
            while(getchar() != '\n');
            *D = -1.0;
            continue;
        }

        if(*D <= 0){
            printf("Battlefield size must be greater than 0.\n");
        }
    }while(*D <= 0);

    do{
        printf("Enter the number of escort ships N (1-%d): ",MAX_ESCORTS);

        if(scanf("%d", N) != 1){
            printf("Invalid input. Enter an integer.\n");
            while(getchar() != '\n');
            *N = -1;
            continue;
        }

        if(*N <= 0 || *N > MAX_ESCORTS){
            printf("Number of escort ships must be between 1 and %d.\n",MAX_ESCORTS);
        }
    }while(*N <= 0 || *N > MAX_ESCORTS);

    initializeBattleship(B, *D);
    initializeEscortShips(E,*N,*D,B->vMax);

    *originalB = *B;

    for(int i = 0; i < *N; i++){
        originalE[i] = E[i];
    }


    printf("\n--- Escort Ships ---\n");

    for(int i = 0; i < *N; i++){

        printf("ID: %d | Type: %s | Position: (%.2f,%.2f) | Velocity: (%.2f-%.2f) | Angle: (%.2f-%.2f) | Impact Power: %.2f\n",
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
    printf("\n--- Distances from the Battleship ---\n");
    for(int i = 0; i < *N; i++){
        double distance = calculate_distance( B->x, B->y, E[i].x, E[i].y);
        printf("Escort %d (Type: %s): %.2f\n",E[i].id,E[i].type,distance);
    }


    printf("\n--- Can Escort Ships Hit the Battleship? ---\n");

    for(int i = 0; i < *N; i++){

        printf(
            "Escort %d (Type: %s) %s hit the battleship.\n",
            E[i].id,
            E[i].type,
            canEscortHitBattleship(&E[i], B) ?
            "can" : "cannot");
    }

    printf("\n--- Can the Battleship Hit the Escort Ships? ---\n");

    for(int i = 0; i < *N; i++){

        printf(
            "The battleship %s hit Escort %d (Type: %s).\n",
            canBattleshipHitEscort(B, &E[i]) ?
            "can" : "cannot",E[i].id,E[i].type);
    }
    saveInitialConditions( B, E, *N,*D);
    printf("\nBattlefield setup completed.\n");
}
static void runSimulationMenu( Battleship *B, EscortShip E[], int N, double D, Battleship originalB, EscortShip originalE[])
{
    int choice;

    do{
        printf("\n--- Select Simulation ---\n");
        printf("1. Part 1-A\n");
        printf("2. Part 1-B\n");
        printf("3. Part 1-C\n");
        printf("4. Part 2-A\n");
        printf("5. Part 2-B\n");
        printf("6. Return\n");

        choice = getMenuChoice(1, 6);

        switch(choice){

            case 1:
                resetBattlefield( B, E,originalB,originalE, N);
                simulatePart1A( B, E, N);
                break;

            case 2:
                resetBattlefield( B, E, originalB, originalE, N);
                simulatePart1B(B, E, N,D);
                break;

            case 3:
                // Part 1-C stationary.
                resetBattlefield( B, E, originalB, originalE, N);
                simulatePart1C( B, E, N);

                // Part 1-C movement simulations.
                resetBattlefield( B, E, originalB, originalE, N);
                simulatePart1CMovement( B, E, N, D);
                break;

            case 4:
                resetBattlefield(B,E,originalB,originalE, N);
                simulatePart2A(B, E, N,D);
                break;

            case 5:
                resetBattlefield(B,E,originalB,originalE,N);
                simulatePart2B( B,E, N,D);
                break;

            case 6:
                break;
        }
    }while(choice != 6);
}

static void showInstructions(void)
{
    printf("\n========================================\n");
    printf("              INSTRUCTIONS\n");
    printf("========================================\n");

    printf("\nObjective:\n");
    printf("The Battleship attempts to destroy as many Escort ships as possible\n");
    printf("while minimizing damage received from Escort attacks.\n");

    printf("\nHow to use the simulator:\n");
    printf("1. Select Start Simulation from the Main Menu.\n");
    printf("2. Select Setup Battlefield before running a simulation.\n");
    printf("3. Enter the battlefield size, number of escorts and Battleship settings.\n");
    printf("4. Select Run Simulation.\n");
    printf("5. Choose Part 1-A, 1-B, 1-C, 2-A or 2-B.\n");
    printf("6. Follow any additional prompts for movement, reload times or gun jamming.\n");

    printf("\nSimulation Parts:\n");
    printf("Part 1-A : Basic stationary battle.\n");
    printf("Part 1-B : Battleship movement and gun-jamming simulations.\n");
    printf("Part 1-C : Cumulative damage and movement simulations.\n");
    printf("Part 2-A : Adds Battleship reload time and attack-order strategy.\n");
    printf("Part 2-B : Adds continuous Escort firing and Escort reload times.\n");

    printf("\nResults:\n");
    printf("Detailed simulation results are saved in text files.\n");
    printf("Use Simulation Statistics from the Main Menu to view saved results.\n");

    printf("\nBattleship Types\n");
    printf("U = USS Iowa (BB-61) Gun = 50-caliber Mark 7 gun\n");
    printf("M = MS King George V Gun = (356 mm) Mark VII gun\n");
    printf("R = Richelieu Gun = (15 inch) Mle 1935 gun\n");
    printf("S = Sovetsky Soyuz-class Gun = (16 inch) B-37 gun\n");

    printf("\nEscort Ship Types\n");
    printf("EA = 1936A-class Destroyer| Gun = SK C/34 naval gun| Impact power = 0.08 \n");
    printf("EB = Gabbiano-classCorvette| Gun = L-47 dual- purpose gun| Impact power = 0.06 \n");
    printf("EC = Matsu-class Destroyer | Gun = Type 89 dual-purpose gun| Impact power = 0.07 \n");
    printf("ED = F-classEscort Ships| Gun = SK C/32 naval gun| Impact power = 0.05 \n");
    printf("EE = Japanese Kaibōkan| Gun = (4.7 inch) naval guns| Impact power = 0.04 \n");
}