
#include "../../headers/Objects/Sprite3D.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/2D/Animation2D.h"

Sprite3D::Sprite3D()
{
    this->billboard = new Billboard();

    for (int i = 0; i< ANIMATEDSPRITE_MAX_ANIMATIONS; i++) {
        this->animations[i] = new Animation2D();
    }
}

void Sprite3D::addAnimation(std::string animation2d, int num_frames)
{
    Logging::getInstance()->Log("Loading Animation2D: " + animation2d + " ("+ std::to_string(num_frames)+" frames)", "BILLBOARD");

    this->animations[this->num_animations]->setup(animation2d, num_frames);
    this->num_animations++;
}

void Sprite3D::setAnimation(int index_animation)
{
    this->current_animation = index_animation;
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

void Sprite3D::updateTrianglesCoordinatesAndTexture(Camera3D *cam)
{
    this->getBillboard()->updateUnconstrainedQuad( this->width, this->height, this, cam->AxisUp(), cam->AxisRight() );
    this->updateTexture();
}