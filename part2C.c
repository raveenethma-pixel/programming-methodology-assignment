#include <stdio.h>
#include <math.h>

#include "part2C.h"
#include "part2A.h"
#include "projectile.h"
#include "escort.h"
#include "part1B.h"
#include "files.h"

#define PART2C_ESCORT_TYPES 5
#define PART2C_MAX_ESCORTS 100
#define PART2C_MAX_PENDING 10000
#define PART2C_EVENT_LIMIT 50000

typedef struct{
    double battleshipReloadTime;
    double escortReloadTime[PART2C_ESCORT_TYPES];
    double battleshipGamma;
    double escortGamma[PART2C_ESCORT_TYPES];
} Part2CRules;

typedef struct{
    int battleshipFireCount;//what n to use in IPn​=IP0​e−γn
    int escortFireCount[PART2C_MAX_ESCORTS];
    double escortDamage[PART2C_MAX_ESCORTS]; //The cumulative damage Escort ship recieved from Battleship
} Part2CState;

typedef struct{
    double hitTime;
    int fromBattleship;
    int shooterIndex;
    int targetIndex;
    double impact;
} Part2CShell;

typedef struct{
    int battleshipDestroyed;
    int sinkingEscort;
    int sunkCount;
    double battleEndTime;

    int attackOrder[PART2C_MAX_ESCORTS];
    int attackCount;
} Part2CResult;

static int findBattleshipTarget(EscortShip E[], int attackOrder[],int attackCount);
static int addShell(Part2CShell shells[], int *count, double hitTime, int fromBattleship,int shooterIndex,int targetIndex,double impact);
//Return a positive value entered by the user.
static double getPositiveDouble(const char prompt[])
{
    double value;
    while(1){
        printf("%s", prompt);
        if(scanf("%lf", &value) != 1){
            printf("Invalid input. Enter a number.\n");
            while(getchar() != '\n');
            continue;
        }

        if(value <= 0.0){
            printf("Value must be greater than 0.\n");
            continue;
        }

        return value;
    }
}


//IPn = IP0 * e^(-gamma * n)
static double calculateDegradedImpact(double initialImpact, double gamma, int firingNumber)
{
    return initialImpact * exp(-gamma * firingNumber);
}


//Get Part 2-C reload times and gamma values.
static void getPart2CSettings( Battleship *B, Part2CRules *rules)
{
    const char *escortTypes[PART2C_ESCORT_TYPES] = { "EA", "EB", "EC", "ED", "EE"};

    char prompt[100];
    printf("\n--- Part 2-C Settings ---\n");
    rules->battleshipReloadTime = getPositiveDouble("Enter Battleship reload time T_B in seconds: ");

    for(int i = 0; i < PART2C_ESCORT_TYPES; i++){

        snprintf( prompt, sizeof(prompt), "Enter %s reload time T_E in seconds: ",escortTypes[i] );
        rules->escortReloadTime[i] = getPositiveDouble(prompt);
    }

    snprintf( prompt, sizeof(prompt), "Enter gamma for Battleship type %c: ", B->type);

    rules->battleshipGamma = getPositiveDouble(prompt);

    for(int i = 0; i < PART2C_ESCORT_TYPES; i++){
        snprintf(prompt,sizeof(prompt),"Enter gamma for Escort type %s: ",escortTypes[i]);
        rules->escortGamma[i] = getPositiveDouble(prompt);
    }
}


//Reset firing counts and Escort damage.
static void initializePart2CState( Part2CState *state, int N)
{
    state->battleshipFireCount = 0;

    for(int i = 0; i < N; i++){
        state->escortFireCount[i] = 0;
        state->escortDamage[i] = 0.0;
    }
}


//Part 2-C uses ONE results file.
static void startPart2CResults( Battleship *B, Part2CRules *rules)
{
    FILE *fp = fopen("part2C_results.txt", "w");

    if(fp == NULL){
        printf("Error creating part2C_results.txt\n");
        return;
    }

    fprintf(fp, "========================================\n");
    fprintf(fp, "            PART 2-C RESULTS\n");
    fprintf(fp, "========================================\n");

    fprintf(fp, "\nBattleship Type: %c\n", B->type);

    fprintf(fp,"Battleship Reload Time: %.2f seconds\n",rules->battleshipReloadTime);

    fprintf( fp,"Battleship Gamma: %.4f\n",rules->battleshipGamma);

    const char *escortTypes[PART2C_ESCORT_TYPES] = {"EA", "EB", "EC", "ED", "EE"};

    fprintf(fp, "\nEscort Settings\n");

    for(int i = 0; i < PART2C_ESCORT_TYPES; i++){
        fprintf( fp,"%s | Reload Time: %.2f seconds | Gamma: %.4f\n",escortTypes[i],rules->escortReloadTime[i], rules->escortGamma[i]);
    }

    fclose(fp);
}

static int findEarliestShell(Part2CShell shells[], int count)
{
    if(count == 0){
        return -1;
    }
    int earliest = 0;
    for(int i = 1; i < count; i++){
        if(shells[i].hitTime < shells[earliest].hitTime){
            earliest = i;
        }
    }

    return earliest;
}

static Part2CResult runPart2CStationaryBattle(Battleship *B, EscortShip E[], int N, Part2CRules *rules,Part2CState *state)
{
    Part2CResult result;

    result.battleshipDestroyed = 0;
    result.sinkingEscort = -1;
    result.sunkCount = 0;
    result.battleEndTime = 0.0;
    result.attackCount = 0;

    //Reuse our Part 2-A attack strategy.
    determineAttackOrder(B, E, N, result.attackOrder, &result.attackCount);

    double escortTravelTime[N];
    double nextEscortFire[N];

    for(int i = 0; i < N; i++){

        escortTravelTime[i] = -1.0;
        nextEscortFire[i] = -1.0;

        if(E[i].alive && canEscortHitBattleship(&E[i], B)){
            double distance = calculate_distance(E[i].x, E[i].y, B->x, B->y);
            escortTravelTime[i] = calculateMinimumHitTime(distance, E[i].vMin, E[i].vMax, E[i].angleMin, E[i].angleMax);

            if(escortTravelTime[i] >= 0.0){
                nextEscortFire[i] = 0.0;
            }
        }
    }

    double nextBattleshipFire = result.attackCount > 0 ? 0.0 : -1.0;

    Part2CShell shells[PART2C_MAX_PENDING];

    int shellCount = 0;
    int eventCount = 0;

    double lastEventTime = 0.0;

    while(eventCount < PART2C_EVENT_LIMIT){
        //Find the earliest shell currently in flight.
        int shellIndex = findEarliestShell(shells, shellCount);
        double nextHitTime = -1.0;

        if(shellIndex >= 0){
            nextHitTime = shells[shellIndex].hitTime;
        }
        
        //Find the next gun firing.
        double nextFireTime = -1.0;

        int firingEscort = -1;
        int battleshipFires = 0;

        if(!result.battleshipDestroyed && nextBattleshipFire >= 0.0){
            nextFireTime = nextBattleshipFire;
            battleshipFires = 1;
        }


        if(!result.battleshipDestroyed){
            for(int i = 0; i < N; i++){
                if(!E[i].alive || nextEscortFire[i] < 0.0){
                    continue;
                }

                if(nextFireTime < 0.0 || nextEscortFire[i] < nextFireTime){

                    nextFireTime = nextEscortFire[i];
                    firingEscort = i;
                    battleshipFires = 0;
                }
            }
        }

        /*
           If a shell hits at the same time a ship wants to fire, process the hit first.
           Therefore a ship destroyed at time t cannot fire at time t.
        */
        if(shellIndex >= 0 && (nextFireTime < 0.0 || nextHitTime <= nextFireTime)){

            Part2CShell shell = shells[shellIndex];

            //Remove this shell from the pending array.
            shells[shellIndex] = shells[shellCount - 1];
            shellCount--;
            lastEventTime = shell.hitTime;


            if(shell.fromBattleship){
                int target = shell.targetIndex;
                //If another B shell already destroyed the target, this shell is simply wasted.
                if(target >= 0 && target < N && E[target].alive){

                    state->escortDamage[target] += shell.impact;

                    if(state->escortDamage[target] >= 1.0){
                        E[target].alive = 0;
                        result.sunkCount++;
                    }
                }
            }
            else{
                //Already-fired Escort shells remain valid even if that Escort was destroyed before the shell arrived.
                if(!result.battleshipDestroyed){
                    B->damage += shell.impact;

                    if(B->damage >= 1.0){
                        result.battleshipDestroyed = 1;
                        result.sinkingEscort = shell.shooterIndex;
                        result.battleEndTime = shell.hitTime;
                       
                        //No new firing after B is sunk.B shells already in flight are still allowed to arrive.                     
                        nextBattleshipFire = -1.0;
                        for(int i = 0; i < N; i++){
                            nextEscortFire[i] = -1.0;
                        }
                    }
                }
            }
            eventCount++;
            continue;
        }
        
        //Nothing can fire and no shells remain. Battle is finished.
        if(nextFireTime < 0.0){
            break;
        }
        
        //B fires.
        if(battleshipFires){
            int target = findBattleshipTarget( E, result.attackOrder, result.attackCount);
            if(target < 0){
                nextBattleshipFire = -1.0;
                continue;
            }

            double distance = calculate_distance(B->x, B->y, E[target].x, E[target].y);
            double travelTime = calculateMinimumHitTime( distance,B->vMin, B->vMax, B->angleMin, B->angleMax);

            if(travelTime < 0.0){
                nextBattleshipFire = -1.0;
                continue;
            }
            //First firing = n = 1.
            state->battleshipFireCount++;
            double impact = calculateDegradedImpact(B->impactPower, rules->battleshipGamma, state->battleshipFireCount);

            if(!addShell(
                    shells,
                    &shellCount,
                    nextFireTime + travelTime,
                    1,
                    -1,
                    target,
                    impact
               )){
                printf("Part 2-C pending shell limit reached.\n");
                break;
            }
            nextBattleshipFire += rules->battleshipReloadTime;
        }

        //Escort fires.
        else if(firingEscort >= 0){
            int type = getEscortTypeIndex(E[firingEscort].type);
            if(type < 0){
                nextEscortFire[firingEscort] = -1.0;
                continue;
            }
            state->escortFireCount[firingEscort]++;
            double impact = calculateDegradedImpact( E[firingEscort].impactPower, rules->escortGamma[type], state->escortFireCount[firingEscort]);

            if(!addShell(
                    shells,
                    &shellCount,
                    nextFireTime + escortTravelTime[firingEscort],
                    0,
                    firingEscort,
                    -1,
                    impact
               )){
                printf("Part 2-C pending shell limit reached.\n");
                break;
            }
            nextEscortFire[firingEscort] += rules->escortReloadTime[type];
        }
        eventCount++;
    }
    if(eventCount >= PART2C_EVENT_LIMIT){
        printf("Part 2-C event safety limit reached.\n");
    }
    if(!result.battleshipDestroyed){
        result.battleEndTime = lastEventTime;
    }
    return result;
}

static int findBattleshipTarget( EscortShip E[], int attackOrder[],int attackCount)
{
    for(int i = 0; i < attackCount; i++){
        int index = attackOrder[i];
        if(E[index].alive){
            return index;
        }
    }

    return -1;
}


static int addShell( Part2CShell shells[], int *count, double hitTime, int fromBattleship, int shooterIndex, int targetIndex, double impact)
{
    if(*count >= PART2C_MAX_PENDING){
        return 0;
    }

    shells[*count].hitTime = hitTime;
    shells[*count].fromBattleship = fromBattleship;
    shells[*count].shooterIndex = shooterIndex;
    shells[*count].targetIndex = targetIndex;
    shells[*count].impact = impact;

    (*count)++;

    return 1;
}

static void savePart2CResults(Battleship *B,EscortShip E[],int N,Part2CRules *rules,Part2CState *state,Part2CResult result,const char title[]){
    FILE *fp = fopen("part2C_results.txt", "a");
    if(fp == NULL){
        printf("Error opening part2C_results.txt\n");
        return;
    }
    fprintf(fp,"\n\n========================================\n");
    fprintf(fp,"%s\n",title); 
    fprintf(fp, "========================================\n");
    fprintf(fp, "\nBattleship Attack Order\n");
    if(result.attackCount == 0){
        fprintf(fp, "No escorts within B attack range.\n");
    }
    else{
        for(int i = 0;i < result.attackCount;i++){
            int index = result.attackOrder[i];
            fprintf(fp, "%d. Escort %d (%s)\n", i + 1, E[index].id, E[index].type);
        }
    }


    double currentBImpact = calculateDegradedImpact(B->impactPower, rules->battleshipGamma, state->battleshipFireCount);
    fprintf(fp, "\nBattleship\n");
    fprintf(fp,"Position: (%.2f, %.2f)\n",B->x,B->y);
    fprintf(fp,"Angle Range: %.2f - %.2f\n",B->angleMin,B->angleMax);
    fprintf(fp,"Fire Count: %d\n", state->battleshipFireCount);
    fprintf(fp,"Current Impact Factor: %.6f\n",currentBImpact);
    fprintf(fp, "Cumulative Damage: %.2f%%\n",B->damage * 100.0);
    fprintf(fp,"Status: %s\n", result.battleshipDestroyed ? "SUNK" : "SURVIVED");

    fprintf(fp, "\nEscort Ships\n");
    for(int i = 0; i < N; i++){
        int type = getEscortTypeIndex(E[i].type);
        double currentImpact = E[i].impactPower;
        if(type >= 0){
            currentImpact = calculateDegradedImpact(E[i].impactPower,rules->escortGamma[type], state->escortFireCount[i]);
        }
        fprintf(fp,"\nEscort %d | Type: %s\n",E[i].id,E[i].type);
        fprintf(fp,"Status: %s\n",E[i].alive ? "ALIVE" : "SUNK");
        fprintf(fp,"Fire Count: %d\n",state->escortFireCount[i]);
        fprintf(fp,"Current Impact Factor: %.6f\n",currentImpact);
        fprintf(fp,"Damage Received From B: %.2f%%\n",state->escortDamage[i] * 100.0);
    }

    fprintf(fp, "\n--- Final Result ---\n");
    fprintf(fp,"Total Escorts Sunk: %d\n", result.sunkCount);
    fprintf(fp,"Battleship Damage: %.2f%%\n", B->damage * 100.0);
    fprintf(fp, "Battle End Time: %.2f seconds\n", result.battleEndTime);
    if(result.battleshipDestroyed && result.sinkingEscort >= 0){
        fprintf(fp,"Battleship sunk by Escort %d (%s)\n", E[result.sinkingEscort].id, E[result.sinkingEscort].type );
    }

    fclose(fp);
}

static void runPart2CMovement(Battleship *B,EscortShip E[],int N,Position path[],int k,Part2CRules *rules,int sim,int jamAfter,double jamAngle)
{
    Part2CState state;
    initializePart2CState(&state,N);
    B->damage = 0.0;
    int totalSunk = 0;

    printf("\n--- Starting Part 2-C Movement Simulation %d ---\n",sim);

    for(int i=0;i<k;i++){
        B->x = path[i].x;
        B->y = path[i].y;

        if(jamAfter > 0 && i >= jamAfter){
            B->angleMin = jamAngle;
            B->angleMax = 90.0;
        }

        Part2CResult result = runPart2CStationaryBattle(B,E,N,rules,&state);
        totalSunk += result.sunkCount;

        char title[80];
        snprintf(title,sizeof(title),
                 "PART 2-C : MOVEMENT SIMULATION %d - ITERATION %d",sim,i+1);

        savePart2CResults(B,E,N,rules,&state,result,title);

        printf("Iteration %d: Escorts sunk: %d | B damage: %.2f%%\n",
               i+1,result.sunkCount,B->damage*100.0);

        if(result.battleshipDestroyed){
            printf("Battleship destroyed during iteration %d.\n",i+1);
            break;
        }
    }

    FILE *fp = fopen("part2C_results.txt","a");
    if(fp){
        fprintf(fp,"\nSimulation %d Summary | Total Escorts Sunk: %d | Final B Damage: %.2f%%\n",
                sim,totalSunk,B->damage*100.0);
        fclose(fp);
    }

    printf("Simulation %d summary: %d escorts sunk | B damage %.2f%%\n",
           sim,totalSunk,B->damage*100.0);
}

void simulatePart2C(Battleship *B,EscortShip E[],int N,double D)
{
    Part2CRules rules;
    getPart2CSettings(B,&rules);

    Battleship originalB = *B;
    EscortShip originalE[N];
    for(int i=0;i<N;i++) originalE[i]=E[i];

    startPart2CResults(B,&rules);

    //Stationary 
    Part2CState state;
    initializePart2CState(&state,N);
    B->damage=0.0;

    printf("\n--- Starting Part 2-C Stationary Simulation ---\n");
    Part2CResult result=runPart2CStationaryBattle(B,E,N,&rules,&state);
    savePart2CResults(B,E,N,&rules,&state,result, "PART 2-C : STATIONARY PART 1-C");
    printf("Escorts sunk: %d | B damage: %.2f%% | %s\n",result.sunkCount,B->damage*100.0,result.battleshipDestroyed ? "SUNK" : "SURVIVED");

    //Movement 
    resetBattlefield(B,E,originalB,originalE,N);

    int k;
    do{
        printf("\nEnter movement points for Part 2-C (minimum 2): ");
        if(scanf("%d",&k)!=1){
            while(getchar()!='\n');
            k=0;
        }
    }while(k<2);

    Position path[k];
    generatePath(path,k,D);

    FILE *fp=fopen("part2C_results.txt","a");
    if(fp){
        fprintf(fp,"\n--- Part 2-C Movement Path ---\n");
        for(int i=0;i<k;i++)
            fprintf(fp,"Point %d: (%.2f, %.2f)\n",i+1,path[i].x,path[i].y);
        fclose(fp);
    }

    // Simulation 1 
    runPart2CMovement(B,E,N,path,k,&rules,1,-1,0.0);

    // Simulation 2 
    int t;
    double jamAngle;
    getJamSettings(k,&t,&jamAngle);

    resetBattlefield(B,E,originalB,originalE,N);

    runPart2CMovement(B,E,N,path,k,&rules,2,t,jamAngle);

    printf("\nDetailed Part 2-C results saved to part2C_results.txt\n");
}
