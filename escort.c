#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "escort.h"

void initializeEscortShips(EscortShip E[], int N, double D, double battleVMax){
    for(int i = 0; i < N; i++){
        E[i].id = i + 1; // Assign a unique id to each escort ship
        int randomtype= rand()%5;//generate 1 random number from 0 t0 4

        switch(randomtype){
            case 0:
                strcpy(E[i].type,"EA");
                E[i].impactPower=0.08;
                E[i].angleMin=20.0;
                break;
            case 1:
                strcpy(E[i].type,"EB");
                E[i].impactPower = 0.06;
                E[i].angleMin = 30.0;
                break;
            case 2:
                strcpy(E[i].type,"EC");
                E[i].impactPower = 0.07;
                E[i].angleMin = 25.0;
                break;
            case 3:
                strcpy(E[i].type,"ED");
                E[i].impactPower= 0.05;
                E[i].angleMin = 50.0;
                break;
            case 4:
                strcpy(E[i].type,"EE");
                E[i].impactPower = 0.04;
                E[i].angleMin = 70.0;
                break;

        }

        //random angles=minimum angle + random numfrom 0.00-1.00*(maximum angle - minimum angle)
        //Maximum angle is 80 degrees for all escort ships(I made this decision because it is a lot harder to sink battleships eith escort ships.)
        E[i].angleMax=E[i].angleMin + ((double)rand()/RAND_MAX)*(80.0-E[i].angleMin);

        //I don't want any ship to be sitting on the boundary.Therefore the x,y coordinates will be 1<x,y<D-1.0
        //Take 1 + random number(0.00-1.00)*the range of the canvas from 1.0 to D-1=(D-1)-1=D-2.0
        E[i].x=1.0 + ((double)rand()/RAND_MAX)*(D-2.0);
        E[i].y=1.0 + ((double)rand()/RAND_MAX)*(D-2.0);

        E[i].vMin=((double)rand()/RAND_MAX)*battleVMax;
        
        //If the escort ship is of type EA, its maximum shell velocity will be 1.2 times the maximum shell velocity of the battleship. 
        if(strcmp(E[i].type,"EA")==0){
            E[i].vMax=1.2 * battleVMax;
        }
        //if the E ships arn't EA then their maximum shell velocity will be a random number between their minimum shell velocity and the maximum shell velocity of the battleship.
        else{
            E[i].vMax=E[i].vMin + ((double)rand()/RAND_MAX)*(battleVMax-E[i].vMin);
        }
    }
}