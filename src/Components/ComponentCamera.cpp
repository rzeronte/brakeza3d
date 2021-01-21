
#include "../../headers/Components/ComponentCamera.h"
#include "../../headers/ComponentsManager.h"
#include "../../headers/Brakeza3D.h"

ComponentCamera::ComponentCamera()
{
    this->camera = new Camera3D();
}

void ComponentCamera::onStart() {
    std::cout << "ComponentCamera onStart" << std::endl;
}

void ComponentCamera::preUpdate()
{
    getCamera()->velocity.vertex1 = getCamera()->getPosition();
    getCamera()->velocity.vertex2 = getCamera()->getPosition();
}

void ComponentCamera::onUpdate() {

    float reduction = 0;
    bool  allowVertical = true;

    if (ComponentsManager::get()->getComponentBSP()->getBSP()->isLoaded()) {
        if (ComponentsManager::get()->getComponentBSP()->getBSP()->isCurrentLeafLiquid()) {
            reduction = SETUP->WALKING_SPEED_LIQUID_DIVISOR;
            allowVertical = true;
        }
    }
    getCamera()->UpdateVelocity( reduction, allowVertical );

    updateCameraBSPCollider();

    if(EngineSetup::getInstance()->DRAW_BSP_CAMERA_HULL_CONTENTS) {
        vec3_t p;
        BSPCollider::Vertex3DToVec3(getCamera()->getPosition(),p);
        int contents = pointHullContent( p );
        Logging::getInstance()->Log("Camera Hull CONTENT: " + std::to_string(contents));
    }
}

void ComponentCamera::postUpdate()
{
    if (getCamera()->getFollowTo() != nullptr) {
        getCamera()->setPosition( getCamera()->getFollowTo()->getPosition() );
        getCamera()->setRotation( getCamera()->getFollowTo()->getRotation() );
    } else {
        getCamera()->UpdatePositionForVelocity();
        getCamera()->UpdateRotation();
    }

    this->getCamera()->UpdateFrustum();
}

void ComponentCamera::onEnd() {

}

void ComponentCamera::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

Camera3D *ComponentCamera::getCamera() const {
    return camera;
}

void ComponentCamera::updateCameraBSPCollider()
{
    BSPCollider* bspCollider = ComponentsManager::get()->getComponentBSP()->getBSPCollider();

    Vertex3D velocity = getCamera()->velocity.getComponent();
    BSPCollider::Vertex3DToVec3(velocity, bspCollider->getPlayerModel()->velocity);


    Vertex3D cp = getCamera()->getPosition();
    vec3_t origin;
    BSPCollider::Vertex3DToVec3(cp, origin);

    bspCollider->getPlayerModel()->origin[0] = origin[0];
    bspCollider->getPlayerModel()->origin[1] = origin[1];
    bspCollider->getPlayerModel()->origin[2] = origin[2];

    bspCollider->SV_AddGravity (
            ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel(),
            Brakeza3D::get()->getDeltaTime()
    );

    bspCollider->SV_CheckStuck (
            ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel()
    );

    bspCollider->SV_WalkMove(
            ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel(),
            Brakeza3D::get()->getDeltaTime()
    );

    Vertex3D newVelocity = BSPCollider::QuakeToVertex3D(bspCollider->getPlayerModel()->velocity);
    getCamera()->velocity.vertex2 = cp + newVelocity;
}

void ComponentCamera::checkTrace(Vertex3D start, Vertex3D vel, vec3_t mins, vec3_t maxs)
{
    BSPCollider* bspCollider = ComponentsManager::get()->getComponentBSP()->getBSPCollider();

    // start trace
    vec3_t origin;
    BSPCollider::Vertex3DToVec3(start, origin);

    // end trace
    vec3_t end;
    Vertex3D endPosition = start + vel;
    BSPCollider::Vertex3DToVec3(endPosition, end);

    trace_t t = bspCollider->SV_ClipMoveToEntity ( bspCollider->getWorldModel(), origin, mins, maxs, end );

    bspCollider->consoleTrace(&t);
}

int ComponentCamera::pointHullContent(vec3_t p)
{
    BSPCollider* bspCollider = ComponentsManager::get()->getComponentBSP()->getBSPCollider();

    int test = bspCollider->SV_HullPointContents(&bspCollider->getWorldModel()->hulls[0],0, p );

    Logging::getInstance()->Log("SV_HullPointContents: " + std::to_string(test));
}