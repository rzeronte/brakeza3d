
#include "../../include/Objects/SpriteDirectional3D.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Logging.h"
#include "../../include/Render/Maths.h"
#include "../../include/ComponentsManager.h"

SpriteDirectional3D::SpriteDirectional3D() {
    this->billboard = new Billboard();
    this->counterAnimations = new Counter();

    this->width = 10; //EngineSetup::get()->BILLBOARD_WIDTH_DEFAULT;
    this->height = 10; //EngineSetup::get()->BILLBOARD_HEIGHT_DEFAULT;

    for (auto & animation : this->animations) {
        animation = new TextureAnimationDirectional();
    }
}

Billboard *SpriteDirectional3D::getBillboard() const {
    return billboard;
}

void SpriteDirectional3D::updateTrianglesCoordinates(Camera3D *cam) {
    Vertex3D up = cam->getRotation().getTranspose() * EngineSetup::getInstance()->up;
    Vertex3D right = cam->getRotation().getTranspose() * EngineSetup::getInstance()->right;

    this->getBillboard()->updateUnconstrainedQuad(this, up, right);
    this->updateTextureFromCameraAngle(this, cam);
}

void
SpriteDirectional3D::addAnimationDirectional2D(const std::string& animation_folder, int numFrames, int fps, bool zeroDirection,
                                               int maxTimes) {
    this->animations[this->numAnimations]->setup(animation_folder, numFrames, fps, maxTimes);

    if (!zeroDirection) {
        this->animations[this->numAnimations]->loadImages();
    } else {
        this->animations[this->numAnimations]->loadImagesForZeroDirection();
        this->animations[this->numAnimations]->isZeroDirection = true;
    }

    this->numAnimations++;
}

void SpriteDirectional3D::updateTextureFromCameraAngle(Object3D *o, Camera3D *cam) {
    if (numAnimations == 0) return;

    float enemyAngle = Maths::getHorizontalAngleBetweenObject3DAndCamera(o, cam);
    int direction = getDirectionForAngle(enemyAngle);

    counterAnimations->update();

    if (counterAnimations->isFinished()) {
        counterAnimations->setEnabled(true);
        getCurrentTextureAnimationDirectional()->nextFrame();
    }

    if (getCurrentTextureAnimationDirectional()->isZeroDirection) {
        this->getBillboard()->setTrianglesTexture(getCurrentTextureAnimationDirectional()->getCurrentFrame(0));
    } else {
        this->getBillboard()->setTrianglesTexture(getCurrentTextureAnimationDirectional()->getCurrentFrame(direction));
    }
}

void SpriteDirectional3D::setAnimation(int indexAnimation) {
    this->currentAnimation = indexAnimation;
    this->updateStep();
    this->counterAnimations->setStep(step);
}

void SpriteDirectional3D::linkTexturesTo(SpriteDirectional3D *clone) {
    this->numAnimations = clone->numAnimations;
    for (int i = 0; i < clone->numAnimations; i++) {
        this->animations[i]->importTextures(clone->animations[i], clone->animations[i]->numFrames);
        this->animations[i]->isZeroDirection = clone->animations[i]->isZeroDirection;
        this->animations[i]->numFrames = clone->animations[i]->numFrames;
        this->animations[i]->base_file = clone->animations[i]->base_file;
        this->animations[i]->maxTimes = clone->animations[i]->maxTimes;
    }
}

TextureAnimationDirectional *SpriteDirectional3D::getCurrentTextureAnimationDirectional() {
    return this->animations[currentAnimation];
}

int SpriteDirectional3D::getDirectionForAngle(float enemyAngle) {
    if (enemyAngle >= 292.5f && enemyAngle < 337.5f)
        return 8;
    else if (enemyAngle >= 22.5f && enemyAngle < 67.5f)
        return 2;
    else if (enemyAngle >= 67.5f && enemyAngle < 112.5f)
        return 3;
    else if (enemyAngle >= 112.5f && enemyAngle < 157.5f)
        return 4;
    else if (enemyAngle >= 157.5f && enemyAngle < 202.5f)
        return 5;
    else if (enemyAngle >= 202.5f && enemyAngle < 247.5f)
        return 6;
    else if (enemyAngle >= 247.5f && enemyAngle < 292.5f)
        return 7;
    else if (enemyAngle >= 337.5f || enemyAngle < 22.5f)
        return 1;
    else return 0;
}

void SpriteDirectional3D::updateStep() {
    step = (float) 1 / (float) this->getCurrentTextureAnimationDirectional()->fps;
    this->counterAnimations->setStep(step);
}

void SpriteDirectional3D::onUpdate() {
    if (!ComponentsManager::get()->getComponentCamera()->getCamera()->frustum->isPointInFrustum(this->getPosition())) {
        return;
    }

    this->updateTrianglesCoordinates(ComponentsManager::get()->getComponentCamera()->getCamera());

    Drawable::drawBillboard(
            this->getBillboard(),
            &ComponentsManager::get()->getComponentRender()->getFrameTriangles()
    );
}
