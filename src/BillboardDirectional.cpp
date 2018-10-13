
#include "../headers/BillboardDirectional.h"
#include "../headers/Core/Logging.h"


BillboardDirectional::BillboardDirectional()
{
    for (int i = 0; i < BILLBOARD_MAX_ANIMATIONS ; i++) {
        this->animations[i] = new AnimationDirectional2D();
    }

}

void BillboardDirectional::addAnimationDirectional2D(std::string animation_folder, int num_frames)
{
    std::string full_animation_folder = EngineSetup::getInstance()->SPRITES_FOLDER + animation_folder;

    Logging::getInstance()->Log("Loading Animation2D: " + animation_folder + " ("+ std::to_string(num_frames)+" frames)", "BILLBOARD");

    this->animations[this->num_animations]->setup(full_animation_folder, num_frames);
    this->animations[this->num_animations]->loadImages();
    this->num_animations++;
}

void BillboardDirectional::updateTextureFromCameraAngle(Object3D *o, Camera *cam)
{
    if (num_animations == 0) return;

    float angle = (int) Tools::getHorizontalAngleBetweenObject3DAndCamera(o, cam);

    int direction;

    if (angle >= 0   && angle < 45)  { direction = DIR_S;  }
    if (angle >= 45  && angle < 90)  { direction = DIR_SW; }
    if (angle >= 90  && angle < 135) { direction = DIR_W;  }
    if (angle >= 135 && angle < 180) { direction = DIR_NW; }
    if (angle >= 180 && angle < 225) { direction = DIR_N;  }
    if (angle >= 225 && angle < 270) { direction = DIR_NE; }
    if (angle >= 270 && angle < 315) { direction = DIR_E;  }
    if (angle >= 315 && angle < 360) { direction = DIR_SE; }


    float deltatime = this->timer->getTicks() - this->last_ticks;

    this->last_ticks = this->timer->getTicks();

    timerCurrent += (deltatime/1000.f);

    float step = (float) 1 / this->fps;

    if (timerCurrent > step) {
        this->animations[current_animation]->nextFrame();
        timerCurrent = 0;
    }

    this->setTrianglesTexture( this->animations[current_animation]->getCurrentFrame(direction) );
}

void BillboardDirectional::setAnimation(int index_animation)
{
    this->current_animation = index_animation;
}

void BillboardDirectional::setTimer(LTimer *timer)
{
    this->timer = timer;
}