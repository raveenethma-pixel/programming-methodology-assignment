#include<stdio.h>
#include"battleship.h" //the declaration of the function is in battleship.h, so we need to include it here
void initializeBattleship(Battleship *B){
    //Initialize the battleship with default values
    printf("Enter the battleship type (U/M/R/S): ");
    scanf(" %c",&B->type);//This is the same as (*B).type, but the arrow operator is much better when dealing with pointers to structures
    //make sure the input is valid
    if(B->type!='U' && B->type!='M' && B->type!='R' && B->type!='S'){ 
        printf("Invalid battleship type. It must be U, M, R, or S.\n");
        return;
    }
    printf("Battleship type %c selected.\n",B->type);//B->type is the same as (*B).type, it points to the chosen type of the battleship
}