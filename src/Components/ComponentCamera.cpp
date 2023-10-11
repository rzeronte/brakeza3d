
#include "../../include/Components/ComponentCamera.h"
#include "../../include/Render/Logging.h"

ComponentCamera::ComponentCamera()
:
    camera(new Camera3D()),
    freeLook(false),
    autoScroll(false)
{
}

void ComponentCamera::onStart()
{
    Logging::head("ComponentCamera onStart");
    setFreeLook(true);
    this->getCamera()->updateFrustum();
}

void ComponentCamera::preUpdate()
{
    getCamera()->getVelocity().vertex1 = getCamera()->getPosition();
    getCamera()->getVelocity().vertex2 = getCamera()->getPosition();
}

void ComponentCamera::onUpdate()
{
    if (isAutoScroll()) {
        getCamera()->addToPosition(autoScrollSpeed);
    }
    getCamera()->UpdateVelocity();
}

void ComponentCamera::postUpdate()
{
    if (this->freeLook) {
        getCamera()->UpdateRotation();
        getCamera()->UpdatePositionForVelocity();
    }

    /*if (getCamera()->getFollowTo() != nullptr) {
        updatePositionForTrackingObject();
    }*/

    this->getCamera()->updateFrustum();
}

void ComponentCamera::onEnd() {

}

void ComponentCamera::onSDLPollEvent(SDL_Event *e, bool &finish)
{
}

Camera3D *ComponentCamera::getCamera() const
{
    return camera;
}

bool ComponentCamera::isFreeLookEnabled() const
{
    return freeLook;
}

void ComponentCamera::setFreeLook(bool value)
{
    freeLook = value;
}

Vertex3D &ComponentCamera::getAutoScrollSpeed() {
    return autoScrollSpeed;
}

void ComponentCamera::setAutoScrollSpeed(const Vertex3D &autoScrollSpeed) {
    ComponentCamera::autoScrollSpeed = autoScrollSpeed;
}

bool &ComponentCamera::isAutoScroll(){
    return autoScroll;
}

void ComponentCamera::setAutoScroll(bool autoScroll) {
    ComponentCamera::autoScroll = autoScroll;
}

void ComponentCamera::updateOCLContext()
{
    auto rp = camera->getRotation();
    auto frustum = camera->getFrustum();

    std::vector<OCLPlane> planesOCL;
    for (int i = EngineSetup::get()->NEAR_PLANE ; i <= EngineSetup::get()->BOTTOM_PLANE ; i++) {
        OCVertex3D A( frustum->planes[i].A.x, frustum->planes[i].A.y, frustum->planes[i].A.z );
        OCVertex3D B( frustum->planes[i].B.x, frustum->planes[i].B.y, frustum->planes[i].B.z );
        OCVertex3D C( frustum->planes[i].C.x, frustum->planes[i].C.y, frustum->planes[i].C.z );
        OCVertex3D normal( frustum->planes[i].normal.x, frustum->planes[i].normal.y, frustum->planes[i].normal.z );
        planesOCL.emplace_back(A, B, C, normal);
    }

    cameraData = CameraData(
    OCVertex3D(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z),
    OCVertex3D(rp.getPitch(), rp.getYaw(), rp.getRoll())
    );

    frustumData = FrustumData(
        OCVertex3D(frustum->vNLs.x, frustum->vNLs.y, frustum->vNLs.z ),
        OCVertex3D(frustum->vNRs.x, frustum->vNRs.y, frustum->vNRs.z ),
        OCVertex3D(frustum->vNTs.x, frustum->vNTs.y, frustum->vNTs.z ),
        OCVertex3D(frustum->vNBs.x, frustum->vNBs.y, frustum->vNBs.z ),
        planesOCL
    );

}

const CameraData &ComponentCamera::getCameraData() const {
    return cameraData;
}

const FrustumData &ComponentCamera::getFrustumData() const {
    return frustumData;
}
