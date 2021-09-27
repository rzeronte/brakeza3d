//
// Created by darkhead on 9/1/20.
//

#include "../../headers/Components/ComponentBSP.h"
#include "../../headers/ComponentsManager.h"

ComponentBSP::ComponentBSP() {
    bsp = new BSPMap();
}

void ComponentBSP::onStart() {
    std::cout << "ComponentBSP onStart" << std::endl;

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

void ComponentBSP::initParallelBSP(const char *bspFilename, std::vector<Triangle *> *frameTriangles) {
    //this->loadingBSP = new std::thread(ParallellInitBSP, bspFilename, frameTriangles);
}


void ComponentBSP::setCameraInBSPStartPosition() const {
    // Load start position from BSP
    Vertex3D bspOriginalPosition = getBSP()->getStartMapPosition();

    int entityID = getBSP()->getIndexOfFirstEntityByClassname("info_player_start");
    btTransform initialTransform;
    initialTransform.setOrigin(btVector3(bspOriginalPosition.x, bspOriginalPosition.y, bspOriginalPosition.z));

    char *angle = getBSP()->getEntityValue(entityID, "angle");
    int angleInt = atoi(std::string(angle).c_str());

    camera->yaw = 90 - angleInt;
    camera->pitch = 0;
    camera->roll = 0;

    camera->setPosition(bspOriginalPosition);
}

void ComponentBSP::setCamera(Camera3D *camera) {
    ComponentBSP::camera = camera;
}

BSPCollider *ComponentBSP::getBSPCollider() const {
    return bspCollider;
}

