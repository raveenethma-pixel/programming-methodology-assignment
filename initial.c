#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#define MAX_ESCORTS 100

typedef struct {
    int id;
    char type[3];

    double x;
    double y;

    double vMin;
    double vMax;

    double angleMin;
    double angleMax;

    double impactPower;
} EscortShip;

typedef struct {
    char type;

    double x;
    double y;

    double vMin;
    double vMax;

    double angleMin;
    double angleMax;

    double impactPower;
} Battleship;
int main(){
    Battleship B;
    EscortShip E[MAX_ESCORTS];
    int N;
    double D;
    
    printf("Enter battlefield size D: ");
    scanf("%lf",&D);
    printf("Enter the number of escort ships N(less than 100) ");
    scanf("%d",&N);
    if(D<=0){
        printf("Invalid battlefield size D. It must be greater than 0.\n");
        return 1;
    }
    if(N<=0 || N>MAX_ESCORTS){
        printf("Invalid number of escort ships N. It must be between 1 and %d.\n", MAX_ESCORTS);
        return 1;
    }
    return 0;


}