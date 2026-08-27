#include<stdio.h>
#include"battleship.h" //the declaration of the function is in battleship.h, so we need to include it here
void initializeBattleship(Battleship *B){
    do{
        //Initialize the battleship with default values
        printf("Enter the battleship type (U/M/R/S): ");
        scanf(" %c",&B->type);//This is the same as (*B).type, but the arrow operator is much better when dealing with pointers to structures
    //make sure the input is valid
        if(B->type!='U' && B->type!='M' && B->type!='R' && B->type!='S'){ 
            printf("Invalid battleship type. It must be U, M, R, or S.\n");
            }
    }while(B->type!='U' && B->type!='M' && B->type!='R' && B->type!='S');//keep asking for input until a valid type is entered

    B->vMin=0.0;
    B->angleMin=0.0;
    B->angleMax=90.0;
    B->impactPower=1.0;
    do{
        printf("Enter the maximum shell velocity: ");  //Ask the user to input the maximum shell velocity for the battleship
        scanf("%lf",&B->vMax);
        if(B->vMax<=0){
            printf("Invalid maximum shell velocity. It must be greater than 0.\n");
        }
    }while(B->vMax<=0);//keep asking for input until a valid maximum shell velocity is entered
    printf("Battleship  %c  initialized successfully.\n",B->type);//B->type is the same as (*B).type, it points to the chosen type of the battleship
}