
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

    ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel()->velocity[0] = getCamera()->velocity.getComponent().x;
    ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel()->velocity[1] = getCamera()->velocity.getComponent().y;
    ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel()->velocity[2] = getCamera()->velocity.getComponent().z;

    ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel()->origin[0] = getCamera()->getPosition().x;
    ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel()->origin[1] = getCamera()->getPosition().y;
    ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel()->origin[2] = getCamera()->getPosition().z;

    /*ComponentsManager::get()->getComponentBSP()->getBSPCollider()->SV_AddGravity (
            ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel(),
            Brakeza3D::get()->getDeltaTime()
    );*/

    //ComponentsManager::get()->getComponentBSP()->getBSPCollider()->SV_CheckStuck ( ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel() );

    ComponentsManager::get()->getComponentBSP()->getBSPCollider()->SV_WalkMove(
            ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel(),
            Brakeza3D::get()->getDeltaTime()
    );


    getCamera()->velocity.vertex2.x += ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel()->velocity[0];
    getCamera()->velocity.vertex2.y += ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel()->velocity[1];
    getCamera()->velocity.vertex2.z += ComponentsManager::get()->getComponentBSP()->getBSPCollider()->getPlayerModel()->velocity[2];
}

void ComponentCamera::postUpdate()
{
    if (getCamera()->getFollowTo() != nullptr) {
        getCamera()->setPosition( getCamera()->getFollowTo()->getPosition() );
        getCamera()->setRotation( getCamera()->getFollowTo()->getRotation() );
    } else {
        getCamera()->setPosition( ComponentsManager::get()->getComponentCollisions()->finalVelocity );
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



