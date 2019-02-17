
#include "../../headers/Objects/SpriteDirectional3D.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Maths.h"

SpriteDirectional3D::SpriteDirectional3D()
{
    this->billboard = new Billboard();

    this->width = EngineSetup::getInstance()->SPRITE3D_DEFAULT_WIDTH;
    this->height = EngineSetup::getInstance()->SPRITE3D_DEFAULT_HEIGHT;

    for (int i = 0; i< BILLBOARD3D_MAX_ANIMATIONS; i++) {
        this->animations[i] = new AnimationDirectional2D();
    }
}

Billboard *SpriteDirectional3D::getBillboard() const
{
    return billboard;
}

void SpriteDirectional3D::updateTrianglesCoordinates(Camera3D *cam)
{
    Vertex3D up = cam->getRotation().getTranspose() * EngineSetup::getInstance()->up;
    Vertex3D right = cam->getRotation().getTranspose() * EngineSetup::getInstance()->right;

    this->getBillboard()->updateUnconstrainedQuad( this->width, this->height, this, up, right );
    this->updateTextureFromCameraAngle(this, cam);
}

void SpriteDirectional3D::draw(Camera3D *cam)
{
    if (EngineSetup::getInstance()->RENDER_OBJECTS_AXIS) {
        Drawable::drawObject3DAxis(this, cam, true, true, true);
    }

    Drawable::drawBillboard(this->billboard, cam );
}

void SpriteDirectional3D::setTimer(Timer *timer)
{
    this->timer = timer;
}

void SpriteDirectional3D::addAnimationDirectional2D(std::string animation_folder, int num_frames)
{
    std::string full_animation_folder = EngineSetup::getInstance()->SPRITES_FOLDER + animation_folder;

    Logging::getInstance()->Log("Loading AnimationDirectional2D: " + animation_folder + " ("+ std::to_string(num_frames)+" frames)", "BILLBOARD");

    this->animations[this->num_animations]->setup(full_animation_folder, num_frames);
    this->animations[this->num_animations]->loadImages();
    this->num_animations++;
}

void SpriteDirectional3D::updateTextureFromCameraAngle(Object3D *o, Camera3D *cam)
{
    if (num_animations == 0) return;

    float angle = (int) Maths::getHorizontalAngleBetweenObject3DAndCamera(o, cam);

    int direction;

    if (angle >= 0   && angle < 45)  { direction = DIR_S;  }
    if (angle >= 45  && angle < 90)  { direction = DIR_SW; }
    if (angle >= 90  && angle < 135) { direction = DIR_W;  }
    if (angle >= 135 && angle < 180) { direction = DIR_NW; }
    if (angle >= 180 && angle < 225) { direction = DIR_N;  }
    if (angle >= 225 && angle < 270) { direction = DIR_NE; }
    if (angle >= 270 && angle < 315) { direction = DIR_E;  }
    if (angle >= 315 && angle < 360) { direction = DIR_SE; }

    // Frame secuence control
    float deltatime = this->timer->getTicks() - this->last_ticks;
    this->last_ticks = this->timer->getTicks();
    timerCurrent += (deltatime/1000.f);

    float step = (float) 1 / this->fps;

    if (timerCurrent > step) {
        this->animations[current_animation]->nextFrame();
        timerCurrent = 0;
    }

    this->getBillboard()->setTrianglesTexture( this->animations[current_animation]->getCurrentFrame(direction) );
}

void SpriteDirectional3D::setAnimation(int index_animation)
{
    this->current_animation = index_animation;
}
