#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "structures.h"
double calculate_distance(double x1, double y1, double x2, double y2);
double calculateProjectileRange(double velocity, double angle);
double calculateMaxAttackRange(double vMax,double angleMin,double angleMax);
double calculateMinAttackRange(double vMin,double angleMin,double angleMax);
int canEscortHitBattleship(EscortShip *E, Battleship *B);
int canBattleshipHitEscort(Battleship *B, EscortShip *E);
double calculateFlightTime(double distance,double velocity,double angle);
double calculateRequiredVelocity(double distance, double angle);
double calculateMinimumHitTime(double distance, double vMin, double vMax, double angleMin, double angleMax);
#endif // PROJECTILE_H