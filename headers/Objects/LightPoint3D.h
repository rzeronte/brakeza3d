#ifndef SDL2_3D_ENGINE_LIGHTPOINT_H
#define SDL2_3D_ENGINE_LIGHTPOINT_H

#include "Object3D.h"
#include "../Render/Frustum.h"
#include "../Components/Camera3D.h"
#include "../../imgui/imgui.h"

class LightPoint3D : public Object3D {
public:

    LightPoint3D();
    void syncFrustum();

    void clearShadowMappingBuffer();
    float getShadowMappingBuffer(int x, int y);
    float setShadowMappingBuffer(int x, int y, float value);

    Uint32 color;

    float kc = 1;   // constant attenuation
    float kl = 0;   // linear attenuation
    float kq = 0;   // quadratic attenuation

    Camera3D *cam;
    float *shadowMappingBuffer;
    int sizeBuffer;

    ImVec4 imgui_color;

    void setColor(int, int, int);
    Uint32 mixColor(Uint32 color, Vertex3D Q);

};


#endif //SDL2_3D_ENGINE_LIGHTPOINT_H
