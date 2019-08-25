#ifndef BRAKEDA3D_OBJECTPHYSIC_H
#define BRAKEDA3D_OBJECTPHYSIC_H

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "../Objects/Vertex3D.h"
#include "../Objects/Mesh3D.h"
#include "Body.h"


class Mesh3DBody: public Mesh3D, public Body {
public:
    Mesh3DBody();

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

    void integrate();
    void integrateMoving();

    btRigidBody* makeRigidBody(float mass, std::vector<Object3D *> &gameObjects, Camera3D *cam, btDiscreteDynamicsWorld*, bool useObjectSpace);

    // Quake BSP Moving attributes
    float speedMoving = 100;
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

};


#endif //BRAKEDA3D_OBJECTPHYSIC_H
