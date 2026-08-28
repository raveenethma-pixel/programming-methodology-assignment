#include<math.h>
#include "projectile.h"
#define PI 3.1416
#define GRAVITY 10.00

//the distance between 2 points(x1,y1)and(x2,y2) is given by = [(x1-x2)^2 + (y1-y2)^2]^(1/2)
double calculate_distance(double x1, double y1, double x2, double y2){
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

//Range
double calculateProjectileRange(double velocity, double angle){
    // Angle in radians
    double radians = angle * (PI / 180.0);
    // range = (v^2 * sin(2 * theta)) / g
    double range = (velocity * velocity * sin(2 * radians)) / GRAVITY;
    return range;
}

//Max attack range(45 or the one closest to 45)
double calculateMaxAttackRange(double vMax,double angleMin,double angleMax){
    double bestAngle;
    if(angleMin<=45.0 && angleMax>=45.0){
        bestAngle=45.0;
    }
    else if(angleMax<45.0){
        bestAngle=angleMax;
    }
    else{
        bestAngle=angleMin;
    }
    return calculateProjectileRange(vMax,bestAngle);
}
 
double calculateMinAttackRange(double vMin,double angleMin,double angleMax){
    double rangeAtMinAngle=calculateProjectileRange(vMin,angleMin);
    double rangeAtMaxAngle=calculateProjectileRange(vMin,angleMax);
    if(rangeAtMinAngle<rangeAtMaxAngle){
        return rangeAtMinAngle;
    }
    else{
        return rangeAtMaxAngle;
    }
}

int canEscortHitBattleship(EscortShip *E, Battleship *B){
    double distance=calculate_distance(E->x,E->y,B->x,B->y);
    double maxRange=calculateMaxAttackRange(E->vMax,E->angleMin,E->angleMax);
    double minRange=calculateMinAttackRange(E->vMin,E->angleMin,E->angleMax);
    if(distance>=minRange && distance<=maxRange){
        return 1;
    }
    else{
        return 0;
    }
}