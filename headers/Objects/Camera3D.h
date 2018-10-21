
#ifndef SDL2_3D_ENGINE_CAMERA_H
#define SDL2_3D_ENGINE_CAMERA_H

#include <SDL_surface.h>
#include "Vertex3D.h"
#include "../Render/Frustum.h"
#include "Object3D.h"
#include "../Render/EngineSetup.h"

class Camera3D :public Object3D {
public:

    float pixelPerUnit;
    float horizontal_fov;

    float farDistance ;
    float aspectRatio;

    Frustum *frustum;

    Camera3D();

    float calcCanvasNearWidth();

    float calcCanvasNearHeight();

    float getScreenAspectRatio();

    float calcCanvasFarWidth();

    float calcCanvasFarHeight();

    float getNearDistance();

    float getVerticalFOV();

    void syncFrustum();

    Vertex3D eyeVector();

    Vertex3D upVector();

    Vertex3D rightVector();

    void consoleInfo();
};


#endif //SDL2_3D_ENGINE_CAMERA_H
