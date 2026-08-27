#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include "structures.h"
#include "battleship.h"


#define MAX_ESCORTS 100



int main(){
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
    initializeBattleship(&B);
    return 0;


}
