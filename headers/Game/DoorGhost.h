//
// Created by darkhead on 8/2/21.
//

#ifndef BRAKEDA3D_DOORGHOST_H
#define BRAKEDA3D_DOORGHOST_H


#include "../Physics/Mesh3DGhost.h"

#define VERTICAL_MOVEMENT_ANGLE -2

class DoorGhost : public Mesh3DGhost {

public:

    // Quake BSP Moving attributes
    float speedMoving = 1000;
    float angleMoving;
    bool moving;
    bool waiting;
    bool reverseMoving;
    float offsetMoving;
    Timer *timer;
    float waitTime = 3;
    float last_ticks = 0;
    float timerCurrent = 0;

    bool active = true;

    DoorGhost();

    void integrate();
    void integrateMoving();
    Vertex3D integrateHorizontalMovement(float sizeX, float sizeZ);
    Vertex3D integrateVerticalMovement(float sizeY);

    bool isMoving() const;
    void setMoving(bool moving);
    bool isReverseMoving() const;
    void setReverseMoving(bool moving);
    void setWaiting(bool waiting);
    bool isWaiting() const;
    void setSpeedMoving(float speed);
    float getSpeedMoving();

    float getAngleMoving() const;

    void setAngleMoving(float angleMoving);

};


#endif //BRAKEDA3D_DOORGHOST_H
