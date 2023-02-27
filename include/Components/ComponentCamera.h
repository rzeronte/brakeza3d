
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
    bool freeLook = false;
    Vertex3D autoScrollSpeed;
    bool autoScroll = false;
public:
    ComponentCamera();

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    [[nodiscard]] Vertex3D &getAutoScrollSpeed();

    void setAutoScrollSpeed(const Vertex3D &autoScrollSpeed);

    [[nodiscard]] bool &isAutoScroll();

    void setAutoScroll(bool autoScroll);

    [[nodiscard]] Camera3D *getCamera() const;

    [[nodiscard]] bool isFreeLookEnabled() const;

    void setFreeLook(bool isFlyMode);

    void updatePositionForTrackingObject();


};


#endif //SDL2_3D_ENGINE_CAMERA_H
