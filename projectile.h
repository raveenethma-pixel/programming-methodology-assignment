#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "structures.h"
double calculate_distance(double x1, double y1, double x2, double y2);
double calculateProjectileRange(double velocity, double angle);
double calculateMaxAttackRange(double vMax,double angleMin,double angleMax);
double calculateMinAttackRange(double vMin,double angleMin,double angleMax);
int canEscortHitBattleship(EscortShip *E, Battleship *B);

#endif // PROJECTILE_H