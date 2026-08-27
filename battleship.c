#include<stdio.h>
#include"battleship.h" //the declaration of the function is in battleship.h, so we need to include it here
void initializeBattleship(Battleship *B){
    //Initialize the battleship with default values
    printf("Enter the battleship type (U/M/R/S): ");
    scanf(" %c",&B->type);//This is the same as (*B).type, but the arrow operator is much better when dealing with pointers to structures
}