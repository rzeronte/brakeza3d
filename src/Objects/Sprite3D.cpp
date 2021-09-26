
#include "../../headers/Objects/Sprite3D.h"
#include "../../headers/EngineSetup.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/2D/TextureAnimation.h"
#include "../../headers/ComponentsManager.h"

Sprite3D::Sprite3D() {
    this->billboard = new Billboard();
    this->counter = new Counter();

    for (int i = 0; i < ANIMATEDSPRITE_MAX_ANIMATIONS; i++) {
        this->animations[i] = new TextureAnimation();
    }
}

void Sprite3D::addAnimation(std::string animation2d, int num_frames, int fps) {
    Logging::getInstance()->Log(
            "Loading TextureAnimation: " + animation2d + " (" + std::to_string(num_frames) + " frames)", "BILLBOARD");

    this->animations[this->numAnimations]->setup(animation2d, num_frames, fps);
    this->numAnimations++;
}

void Sprite3D::setAnimation(int index_animation) {
    this->currentAnimationIndex = index_animation;
    this->step = (float) 1 / (float) this->getCurrentTextureAnimation()->getFps();
    this->counter->setStep(step);
}

Billboard *Sprite3D::getBillboard() const {
    return billboard;
}

void Sprite3D::updateTexture() {
    if (numAnimations == 0) return;

    counter->update();

    if (counter->isFinished()) {
        getCurrentTextureAnimation()->nextFrame();
        if (this->isAutoRemoveAfterAnimation() && getCurrentTextureAnimation()->isEndAnimation()) {
            this->setRemoved(true);
        }
    }

    this->getBillboard()->setTrianglesTexture(this->animations[currentAnimationIndex]->getCurrentFrame());
}

void Sprite3D::updateTrianglesCoordinatesAndTexture(Camera3D *cam) {
    Vertex3D up = cam->getRotation().getTranspose() * EngineSetup::getInstance()->up;
    Vertex3D right = cam->getRotation().getTranspose() * EngineSetup::getInstance()->right;

    this->getBillboard()->updateUnconstrainedQuad(this, up, right);
    this->updateTexture();
}

bool Sprite3D::isAutoRemoveAfterAnimation() const {
    return autoRemoveAfterAnimation;
}

void Sprite3D::setAutoRemoveAfterAnimation(bool autoRemoveAfterAnimation) {
    Sprite3D::autoRemoveAfterAnimation = autoRemoveAfterAnimation;
}

void Sprite3D::linkTextureAnimation(Sprite3D *dst) {
    this->numAnimations = dst->numAnimations;

    for (int i = 0; i < dst->numAnimations; i++) {
        this->animations[i]->base_file = dst->animations[i]->base_file;
        this->animations[i]->numFrames = dst->animations[i]->numFrames;
        this->animations[i]->fps = dst->animations[i]->fps;

        for (int j = 0; j < dst->animations[i]->numFrames; j++) {
            this->animations[i]->frames[j] = dst->animations[i]->frames[j];
        }
    }
}

TextureAnimation *Sprite3D::getCurrentTextureAnimation() {
    return this->animations[currentAnimationIndex];
}

void Sprite3D::onUpdate() {
    Object3D::onUpdate();

    this->updateTrianglesCoordinatesAndTexture(ComponentsManager::get()->getComponentCamera()->getCamera());
    Drawable::drawBillboard(
            this->getBillboard(),
            &ComponentsManager::get()->getComponentRender()->getFrameTriangles()
    );
}
