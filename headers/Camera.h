
#ifndef SDL2_3D_ENGINE_CAMERA_H
#define SDL2_3D_ENGINE_CAMERA_H

#include <SDL_surface.h>
#include "Vertex.h"
#include "Frustum.h"
#include "Object3D.h"
#include "EngineSetup.h"

class Camera :public Object3D {
public:

    float pixelPerUnit;
    float horizontal_fov;

    float farDistance ;
    float aspectRatio;

    Frustum *frustum;

    Camera();

    float calcCanvasNearWidth();

    float calcCanvasNearHeight();

    float getScreenAspectRatio();

    float calcCanvasFarWidth();

    float calcCanvasFarHeight();

    float getNearDistance();

    float getVerticalFOV();

    void syncFrustum();

    Vertex eyeVector();

    Vertex upVector();

    Vertex rightVector();

    void consoleInfo();
};


#endif //SDL2_3D_ENGINE_CAMERA_H
