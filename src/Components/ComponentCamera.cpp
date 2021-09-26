
#include "../../headers/Components/ComponentCamera.h"
#include "../../headers/ComponentsManager.h"
#include "../../headers/Brakeza3D.h"

ComponentCamera::ComponentCamera() {
    this->camera = new Camera3D();
}

void ComponentCamera::onStart() {

    std::cout << "ComponentCamera onStart" << std::endl;
}

void ComponentCamera::preUpdate() {
    getCamera()->velocity.vertex1 = getCamera()->getPosition();
    getCamera()->velocity.vertex2 = getCamera()->getPosition();
}

void ComponentCamera::onUpdate() {

    // debug hull contents flag
    if (EngineSetup::getInstance()->DRAW_BSP_CAMERA_HULL_CONTENTS &&
        ComponentsManager::get()->getComponentBSP()->getBSP()->isLoaded()
            ) {
        vec3_t p;
        BSPCollider::Vertex3DToVec3(getCamera()->getPosition(), p);
        int contents = pointHullContent(p);
        Logging::getInstance()->Log("Camera Hull CONTENT: " + std::to_string(contents));
    }

    getCamera()->UpdateVelocity();

    if (ComponentsManager::get()->getComponentBSP()->getBSP()->isLoaded()) {
        updateCameraBSPCollider();
    }
}

void ComponentCamera::postUpdate() {
    if (this->is_fly_mode) {

        getCamera()->UpdateRotation();
        getCamera()->UpdatePositionForVelocity();

        if (ComponentsManager::get()->getComponentBSP()->getBSP()->isLoaded()) {
            // BSP teleporting update position
            BSPCollider *bspCollider = ComponentsManager::get()->getComponentBSP()->getBSPCollider();
            if (!bspCollider->getPlayerModel()->teleportingCounter.isEnabled()) {
                getCamera()->UpdatePositionForVelocity();
            } else {
                bspCollider->getPlayerModel()->teleportingCounter.update();
                if (bspCollider->getPlayerModel()->teleportingCounter.isFinished()) {
                    bspCollider->getPlayerModel()->teleportingCounter.setEnabled(false);
                }
            }
        }
    }

    this->getCamera()->UpdateFrustum();
}

void ComponentCamera::drawCheckTrace(std::string o1, std::string o2) {
    vec3_t mins = {1, 1, 1};
    vec3_t maxs = {1, 1, 1};

    ComponentsManager::get()->getComponentBSP()->getBSPCollider()->checkTrace(
            Brakeza3D::get()->getObjectByLabel(o1)->getPosition(),
            Brakeza3D::get()->getObjectByLabel(o2)->getPosition(),
            mins,
            maxs
    );
}

void ComponentCamera::updateCameraBSPCollider() {
    BSPCollider *bspCollider = ComponentsManager::get()->getComponentBSP()->getBSPCollider();
    model_collision_t *player = bspCollider->getPlayerModel();

    // velocity
    Vertex3D currentVelocity = BSPCollider::QuakeToVertex3D(player->velocity);
    Vertex3D velocity = getCamera()->velocity.getComponent() + currentVelocity;
    BSPCollider::Vertex3DToVec3(velocity, player->velocity);

    // camera position
    Vertex3D cameraPosition = getCamera()->getPosition();
    BSPCollider::Vertex3DToVec3(cameraPosition, player->origin);

    // do it!
    float deltaTime = Brakeza3D::get()->getDeltaTime();

    bspCollider->SV_CheckVelocity(player);
    bspCollider->SV_ClientThink(player, deltaTime);
    bspCollider->SV_AddGravity(player, deltaTime);
    bspCollider->SV_CheckStuck(player);
    bspCollider->SV_WalkMove(player, deltaTime);

    //bspCollider->SV_FlyMove(player, deltaTime, NULL);

    // update velocity
    getCamera()->velocity.vertex2 = BSPCollider::QuakeToVertex3D(player->origin);
}

int ComponentCamera::pointHullContent(vec3_t p) {
    BSPCollider *bspCollider = ComponentsManager::get()->getComponentBSP()->getBSPCollider();

    int test = bspCollider->SV_HullPointContents(&bspCollider->getWorldModel()->hulls[0], 0, p);

    Logging::getInstance()->Log("SV_HullPointContents: " + std::to_string(test));

    return test;
}

void ComponentCamera::onEnd() {

}

void ComponentCamera::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

Camera3D *ComponentCamera::getCamera() const {
    return camera;
}

bool ComponentCamera::isFlyMode() const {
    return is_fly_mode;
}

void ComponentCamera::setIsFlyMode(bool isFlyMode) {
    is_fly_mode = isFlyMode;
}