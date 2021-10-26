#include "../../include/Components/ComponentBSP.h"
#include "../../include/ComponentsManager.h"

ComponentBSP::ComponentBSP() {
    bsp = new BSPMap();
}

void ComponentBSP::onStart() {
    Logging::Log("ComponentBSP onStart", "ComponentBSP");
}

void ComponentBSP::initMap(const char *bspFilename) {
    bsp->Initialize(bspFilename, "palette.lmp", camera);
    setCameraInBSPStartPosition();

    bspCollider = new BSPCollider();
}

void ComponentBSP::preUpdate() {
    if (bsp->isLoaded()) {
        dleaf_t *leaf = bsp->FindLeaf(camera->getPosition(), true);
        bsp->setVisibleSet(leaf);
    }
}

void ComponentBSP::onUpdate() {

}

void ComponentBSP::postUpdate() {

}

void ComponentBSP::onEnd() {

}

void ComponentBSP::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

BSPMap *ComponentBSP::getBSP() const {
    return bsp;
}

void ComponentBSP::setCameraInBSPStartPosition() const {
    // Load start position from BSP
    Vertex3D bspOriginalPosition = getBSP()->getStartMapPosition();

    int entityID = getBSP()->getIndexOfFirstEntityByClassname("info_player_start");
    btTransform initialTransform;
    initialTransform.setOrigin(btVector3(bspOriginalPosition.x, bspOriginalPosition.y, bspOriginalPosition.z));

    char *angle = getBSP()->getEntityValue(entityID, "angle");
    int angleInt = atoi(std::string(angle).c_str());

    camera->yaw = (float) (90 -angleInt);
    camera->pitch = 0;
    camera->roll = 0;

    camera->setPosition(bspOriginalPosition);
}

void ComponentBSP::setCamera(Camera3D *newCamera) {
    ComponentBSP::camera = newCamera;
}

BSPCollider *ComponentBSP::getBSPCollider() const {
    return bspCollider;
}

