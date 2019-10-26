
#include "../../headers/Objects/SpriteDirectional3D.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Maths.h"

SpriteDirectional3D::SpriteDirectional3D()
{
    this->billboard = new Billboard();

    this->width = 10; //EngineSetup::get()->BILLBOARD_WIDTH_DEFAULT;
    this->height = 10; //EngineSetup::get()->BILLBOARD_HEIGHT_DEFAULT;

    for (int i = 0; i< BILLBOARD3D_MAX_ANIMATIONS; i++) {
        this->animations[i] = new TextureAnimationDirectional();
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

    this->getBillboard()->updateUnconstrainedQuad( this, up, right );
    this->updateTextureFromCameraAngle(this, cam);
}

void SpriteDirectional3D::setTimer(Timer *timer)
{
    this->timer = timer;
}

void SpriteDirectional3D::addAnimationDirectional2D(std::string animation_folder, int numFrames, int fps, bool zeroDirection, int maxTimes)
{
    std::string full_animation_folder = EngineSetup::getInstance()->SPRITES_FOLDER + animation_folder;

    Logging::getInstance()->Log("Loading TextureAnimationDirectional: " + animation_folder + " (" + std::to_string(numFrames) + " animations)", "BILLBOARD");

    this->animations[this->num_animations]->setup(full_animation_folder, numFrames, fps, maxTimes);

    if (!zeroDirection) {
        this->animations[this->num_animations]->loadImages();
    } else {
        this->animations[this->num_animations]->loadImagesForZeroDirection();
        this->animations[this->num_animations]->isZeroDirection = true;
    }

    this->num_animations++;
}

void SpriteDirectional3D::updateTextureFromCameraAngle(Object3D *o, Camera3D *cam)
{
    if (num_animations == 0) return;

    float enemyAngle = (int) Maths::getHorizontalAngleBetweenObject3DAndCamera(o, cam);
    float direction;

    if (enemyAngle >= 292.5f && enemyAngle < 337.5f)
        direction= 8;
    else if (enemyAngle >= 22.5f && enemyAngle < 67.5f)
        direction= 2;
    else if (enemyAngle >= 67.5f && enemyAngle < 112.5f)
        direction= 3;
    else if (enemyAngle >= 112.5f && enemyAngle < 157.5f)
        direction= 4;
    else if (enemyAngle >= 157.5f && enemyAngle < 202.5f)
        direction= 5;
    else if (enemyAngle >= 202.5f && enemyAngle < 247.5f)
        direction= 6;
    else if (enemyAngle >= 247.5f && enemyAngle < 292.5f)
        direction= 7;
    else if (enemyAngle >= 337.5f || enemyAngle < 22.5f)
        direction= 1;
    else direction= 0;


    // Frame secuence control
    float deltatime = this->timer->getTicks() - this->timerLastTicks;
    this->timerLastTicks = this->timer->getTicks();
    timerCurrent += (deltatime/1000.f);

    float step = (float) 1 / this->getCurrentTextureAnimationDirectional()->fps;

    if (timerCurrent > step) {
        getCurrentTextureAnimationDirectional()->nextFrame();
        timerCurrent = 0;
    }

    if (getCurrentTextureAnimationDirectional()->isZeroDirection) {
        this->getBillboard()->setTrianglesTexture(getCurrentTextureAnimationDirectional()->getCurrentFrame(0) );
    } else {
        this->getBillboard()->setTrianglesTexture(getCurrentTextureAnimationDirectional()->getCurrentFrame(direction) );
    }
}

void SpriteDirectional3D::setAnimation(int index_animation)
{
    this->current_animation = index_animation;
}

void SpriteDirectional3D::linkTexturesTo(SpriteDirectional3D *clone)
{
    this->num_animations = clone->num_animations;
    for (int i = 0; i < clone->num_animations; i++) {
        this->animations[i]->importTextures(clone->animations[i], clone->animations[i]->numFrames);
        this->animations[i]->isZeroDirection = clone->animations[i]->isZeroDirection;
        this->animations[i]->numFrames = clone->animations[i]->numFrames;
        this->animations[i]->base_file = clone->animations[i]->base_file;
        this->animations[i]->maxTimes = clone->animations[i]->maxTimes;
    }
}

TextureAnimationDirectional* SpriteDirectional3D::getCurrentTextureAnimationDirectional()
{
    return this->animations[current_animation];
}