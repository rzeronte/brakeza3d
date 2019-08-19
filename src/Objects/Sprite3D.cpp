
#include "../../headers/Objects/Sprite3D.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/2D/TextureAnimation.h"

Sprite3D::Sprite3D()
{
    this->billboard = new Billboard();

    for (int i = 0; i < ANIMATEDSPRITE_MAX_ANIMATIONS; i++) {
        this->animations[i] = new TextureAnimation();
    }
}

void Sprite3D::addAnimation(std::string animation2d, int num_frames)
{
    Logging::getInstance()->Log("Loading TextureAnimation: " + animation2d + " ("+ std::to_string(num_frames)+" animations)", "BILLBOARD");

    this->animations[this->numAnimations]->setup(animation2d, num_frames);
    this->numAnimations++;
}

void Sprite3D::setAnimation(int index_animation)
{
    this->currentAnimationIndex = index_animation;
}

void Sprite3D::setTimer(Timer *timer)
{
    this->timer = timer;
}

void Sprite3D::draw(Camera3D *cam)
{
    Drawable::drawBillboard( this->billboard, cam );
}

Billboard *Sprite3D::getBillboard() const
{
    return billboard;
}

void Sprite3D::updateTexture()
{
    if (numAnimations == 0) return;

    // Frame secuence control
    float deltatime = this->timer->getTicks() - this->timerLastTicks;
    this->timerLastTicks = this->timer->getTicks();
    timerCurrent += (deltatime/1000.f);

    float step = (float) 1 / this->fps;

    if (timerCurrent > step) {
        timerCurrent = 0;
        this->animations[currentAnimationIndex]->nextFrame();
        if (this->autoRemoveAfterAnimation && this->animations[currentAnimationIndex]->isEndAnimation()) {
            this->setRemoved(true);
            return;
        }
    }

    this->getBillboard()->setTrianglesTexture( this->animations[currentAnimationIndex]->getCurrentFrame() );
}

void Sprite3D::updateTrianglesCoordinatesAndTexture(Camera3D *cam)
{
    Vertex3D up = cam->getRotation().getTranspose() * EngineSetup::getInstance()->up;
    Vertex3D right = cam->getRotation().getTranspose() * EngineSetup::getInstance()->right;

    this->getBillboard()->updateUnconstrainedQuad(this, up, right );
    this->updateTexture();
}