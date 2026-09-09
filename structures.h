#ifndef STRUCTURES_H
#define STRUCTURES_H
//A structure groups related variables of different data types into one custom data type.
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

    int alive;//1=alive, 0=dead
} EscortShip;

//typedef creates the convenient type name: example: Battleship instead of struct Battleship. This allows you to declare variables of this type without using the struct keyword.
typedef struct {  //creating a structure for the battleship
    char type;

    double x;
    double y;

    double vMin;
    double vMax;

    double angleMin;
    double angleMax;

    double impactPower;//Impact power of the battleship

    double damage; //Damage the battleship take
} Battleship;

typedef struct {
    double x;
    double y;
}Position;
#endif