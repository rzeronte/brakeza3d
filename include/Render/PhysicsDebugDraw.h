/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by darkhead on 2019-05-12.
//

#ifndef BRAKEDA3D_PHYSICSDEBUGDRAW_H
#define BRAKEDA3D_PHYSICSDEBUGDRAW_H

#include <mutex>
#include <LinearMath/btIDebugDraw.h>
#include "../Components/Camera3D.h"

class PhysicsDebugDraw : public btIDebugDraw
{
    int m_debugMode = DBG_NoDebug;
    std::mutex mtx;
public:

    PhysicsDebugDraw();

    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;

    void drawContactPoint(
        const btVector3 &PointOnB,
        const btVector3 &normalOnB,
        btScalar distance,
        int lifeTime,
        const btVector3 &color
    ) override;

    void reportErrorWarning(const char *warningString) override;

    void draw3dText(const btVector3 &location, const char *textString) override;

    void setDebugMode(int debugMode) override;

    [[nodiscard]] int getDebugMode() const override;
};

#endif //BRAKEDA3D_PHYSICSDEBUGDRAW_H
