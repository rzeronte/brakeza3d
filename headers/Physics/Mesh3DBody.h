//
// Created by darkhead on 2019-05-11.
//
#ifndef BRAKEDA3D_OBJECTPHYSIC_H
#define BRAKEDA3D_OBJECTPHYSIC_H

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include "../Objects/Vertex3D.h"
#include "../Objects/Mesh3D.h"


class Mesh3DBody: public Mesh3D {
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

    btRigidBody* makeRigidBody(float mass, std::vector<Mesh3DBody *> &, Camera3D *cam, btDiscreteDynamicsWorld*, bool useObjectSpace);

    btRigidBody* getRigidBody() { return m_body; }
    btCollisionObject* getCollider() { return m_collider; }
    btRigidBody* getRigidObject() { return m_body; };

    float mass;
    btRigidBody* m_body;

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

private:

    bool m_isActive;

    btCollisionObject* m_collider;
};


#endif //BRAKEDA3D_OBJECTPHYSIC_H
