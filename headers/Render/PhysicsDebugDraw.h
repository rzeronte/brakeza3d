//
// Created by darkhead on 2019-05-12.
//

#ifndef BRAKEDA3D_PHYSICSDEBUGDRAW_H
#define BRAKEDA3D_PHYSICSDEBUGDRAW_H


#include <LinearMath/btIDebugDraw.h>
#include "../Components/Camera3D.h"

class PhysicsDebugDraw : public btIDebugDraw {
public:
    Camera3D *cam;

    PhysicsDebugDraw(Camera3D *cam);

    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;

    void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime,
                          const btVector3 &color) override;

    void reportErrorWarning(const char *warningString) override;

    void draw3dText(const btVector3 &location, const char *textString) override;

    void setDebugMode(int debugMode) override;

    int getDebugMode() const override;
};

#endif //BRAKEDA3D_PHYSICSDEBUGDRAW_H
