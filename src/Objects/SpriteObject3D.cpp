
#include "../../headers/SpriteObject3D.h"
#include "../../headers/EngineSetup.h"
#include "../../headers/Logging.h"
#include "../../headers/Drawable.h"

SpriteObject3D::SpriteObject3D()
{
    this->billboard = new Billboard();

    for (int i = 0; i< ANIMATEDSPRITE_MAX_ANIMATIONS; i++) {
        this->animations[i] = new Animation2D();
    }
}

void SpriteObject3D::addAnimation(std::string animation2d, int num_frames)
{
    Logging::getInstance()->Log("Loading Animation2D: " + animation2d + " ("+ std::to_string(num_frames)+" frames)", "BILLBOARD");

    this->animations[this->num_animations]->setup(animation2d, num_frames);
    this->num_animations++;
}

void SpriteObject3D::setAnimation(int index_animation)
{
    this->current_animation = index_animation;
}

void SpriteObject3D::setTimer(Timer *timer)
{
    this->timer = timer;
}

void SpriteObject3D::draw(Camera *cam)
{
    Drawable::drawBillboard( this->billboard, cam );

}

Billboard *SpriteObject3D::getBillboard() const
{
    return billboard;
}

void SpriteObject3D::updateTexture()
{
    if (num_animations == 0) return;

    // Frame secuence control
    float deltatime = this->timer->getTicks() - this->last_ticks;
    this->last_ticks = this->timer->getTicks();
    timerCurrent += (deltatime/1000.f);

    float step = (float) 1 / this->fps;

    if (timerCurrent > step) {
        this->animations[current_animation]->nextFrame();
        timerCurrent = 0;
    }

    this->getBillboard()->setTrianglesTexture( this->animations[current_animation]->getCurrentFrame() );
}

void SpriteObject3D::updateTrianglesCoordinatesAndTexture(Camera *cam)
{
    this->getBillboard()->updateUnconstrainedQuad( this->width, this->height, this, cam->upVector(), cam->rightVector() );
    this->updateTexture();
}