#include<stdio.h>
#include"battleship.h" //the declaration of the function is in battleship.h, so we need to include it here
void initializeBattleship(Battleship *B,double D){
    do{
        
        printf("Enter the battleship type (U/M/R/S): ");
        scanf(" %c",&B->type);//This is the same as (*B).type, but the arrow operator is much better when dealing with pointers to structures
    //make sure the input is valid
        if(B->type!='U' && B->type!='M' && B->type!='R' && B->type!='S'){ 
            printf("Invalid battleship type. It must be U, M, R, or S.\n");
            }
    }while(B->type!='U' && B->type!='M' && B->type!='R' && B->type!='S');//keep asking for input until a valid type is entered

    //Initialize the battleship with default values
    B->vMin=0.0;
    B->angleMin=0.0;
    B->angleMax=90.0;
    B->impactPower=1.0;
    B->damage = 0.0;
    int ch;
    do{
        printf("Enter the maximum shell velocity: ");  //Ask the user to input the maximum shell velocity for the battleship
        if(scanf("%lf",&B->vMax)!= 1){
            printf("Invalid input.Enter a number\n");
            while((ch=getchar()!='\n' && ch!=EOF));//clear the input buffer(if the user inputs some other characters not numbers)
            B->vMax=-1;
            continue;
        }
        if(B->vMax<=0){
            printf("Invalid maximum shell velocity. It must be greater than 0.\n");
        }
    }while(B->vMax<=0);//keep asking for input until a valid maximum shell velocity is entered
    
    printf("\nEnter the battleship position\n");
    do{
        printf("Enter the x coordinate of the %c battleship(Must be >0 and <%.2f): ",B->type,D); //Get the x values "inside" the canvas
        if(scanf("%lf",&B->x)!= 1){
            printf("Invalid input.Enter a number\n");
            while((ch=getchar()!='\n' && ch!=EOF));//clear the input buffer(if the user inputs special characters)
            B->x=-1;
            continue;
        }
        if(B->x<=0 || B->x>=D){
            printf("Invalid x coordinate. It must be greater than 0 and less than %.2f.\n",D);
        }
    }while(B->x<=0 || B->x>=D);
    do{
        printf("Enter the y coordinate of the %c battleship(Must be>0 and <%.2f):  ",B->type,D);
        if(scanf("%lf",&B->y)!= 1){
            printf("Invalid input.Enter a number\n");
            while((ch=getchar()!='\n' && ch!=EOF));//clear the input buffer(if the user inputs special characters)
            B->y=-1;
            continue;
        }
        if(B->y<=0 || B->y>=D){
            printf("Invalid y coordinate. It must be greater than 0 and less than %.2f.\n",D);//get the y values "inside" the canvas
        }
    }while(B->y<=0 || B->y>=D);
    printf("\nBattleship  %c  initialized successfully.\n",B->type);//B->type is the same as (*B).type, it points to the chosen type of the battleship
    printf("Battleship position: (%.2f, %.2f)\n",B->x,B->y);
}