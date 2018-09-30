#ifndef SDL2_3D_ENGINE_LIGHTPOINT_H
#define SDL2_3D_ENGINE_LIGHTPOINT_H

#include "Object3D.h"
#include "Frustum.h"
#include "Camera.h"
#include "../imgui/imgui.h"

class Tools;

class LightPoint : public Object3D {
public:

    LightPoint();
    void syncFrustum();

    void clearShadowMappingBuffer();
    float getShadowMappingBuffer(int x, int y);
    float setShadowMappingBuffer(int x, int y, float value);

    Uint32 color;

    float kc = 1;   // constant attenuation
    float kl = 0;   // linear attenuation
    float kq = 0;   // quadratic attenuation

    Billboard *billboard;

    Camera *cam;
    float *shadowMappingBuffer;
    int sizeBuffer;

    ImVec4 imgui_color;

    void setColor(int, int, int);
};


#endif //SDL2_3D_ENGINE_LIGHTPOINT_H
