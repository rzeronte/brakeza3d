
#ifndef SDL2_3D_ENGINE_CAMERA_H
#define SDL2_3D_ENGINE_CAMERA_H

#include <SDL2/SDL_surface.h>
#include "../Objects/Vertex3D.h"
#include "../Objects/Object3D.h"
#include "../Render/Frustum.h"
#include "../EngineSetup.h"
#include "../Render/M3.h"
#include "Component.h"
#include "ComponentWindow.h"
#include "Camera3D.h"
#include "ComponentCollisions.h"
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionShapes/btCapsuleShape.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

typedef float vec3_t[3];

class ComponentCamera : public Component {
private:
    Camera3D *camera;
    bool freeLook;
    bool autoScroll;
    Vertex3D autoScrollSpeed;
public:
    ComponentCamera();

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    void setAutoScrollSpeed(const Vertex3D &autoScrollSpeed);

    void setAutoScroll(bool autoScroll);

    void setFreeLook(bool isFlyMode);

    [[nodiscard]] Vertex3D &getAutoScrollSpeed();

    [[nodiscard]] bool &isAutoScroll();

    [[nodiscard]] Camera3D *getCamera() const;

    [[nodiscard]] bool isFreeLookEnabled() const;
};


#endif //SDL2_3D_ENGINE_CAMERA_H
