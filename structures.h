#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct { //Creating a structure for the escort ships
    int id;
    char type[3];

    double x;
    double y;

    double vMin;
    double vMax;

    double angleMin;
    double angleMax;

    double impactPower;

    int alive;
} EscortShip;

typedef struct {  //creating a structure for the battleship
    char type;

    double x;
    double y;

    double vMin;
    double vMax;

    double angleMin;
    double angleMax;

    double impactPower;
} Battleship;

typedef struct {
    double x;
    double y;
}Position;
#endif