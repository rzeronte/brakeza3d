
#include "../../include/3D/Image3DAnimation8Directions.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Render/Maths.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/Image3DAnimation8DirectionsGUI.h"

Image3DAnimation8Directions::Image3DAnimation8Directions(const Vertex3D &position, float width, float height)
:
    billboard(new Image3D(position, width, height, nullptr)),
    width(width),
    height(height)
{
    this->counterAnimations = new Counter();

    /*for (auto & animation : this->animations) {
        animation = new TextureAnimatedDirectional();
    }*/
}

void Image3DAnimation8Directions::onUpdate()
{
    if (static_cast<int>(animations.size()) <= 0) return;

    this->updateTrianglesCoordinates(ComponentsManager::get()->getComponentCamera()->getCamera());

    ComponentsManager::get()->getComponentRender()->getShaderOGLRenderForward()->render(
            this,
            billboard->getImage()->getOGLTextureID(),
            billboard->getImage()->getOGLTextureID(),
            billboard->getVertexBuffer(),
            billboard->getUVBuffer(),
            billboard->getNormalBuffer(),
            static_cast<int>(billboard->getVertices().size()),
            1.0f,
            ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );
}

void Image3DAnimation8Directions::updateTrianglesCoordinates(Camera3D *cam)
{
    updateTextureFromCameraAngle(this, cam);
}

void Image3DAnimation8Directions::addAnimationDirectional2D(
    const std::string& animation_folder,
    int numFrames,
    int fps,
    bool zeroDirection,
    int maxTimes
)
{
    Logging::Message("Adding TextureAnimatedDirectional to SpriteDirectional(%s)...", animation_folder.c_str(), getLabel().c_str());
    auto animation = new TextureAnimatedDirectional();

    animation->setup(
        animation_folder,
        numFrames,
        fps,
        maxTimes
    );

    if (!zeroDirection) {
        animation->loadImages();
    } else {
        animation->loadImagesForZeroDirection();
        animation->setIsZeroDirection(true);
    }

    animations.push_back(animation);
}

void Image3DAnimation8Directions::updateTextureFromCameraAngle(Object3D *o, Camera3D *cam)
{
    if (animations.empty()) return;

    float enemyAngle = Maths::getHorizontalAngleBetweenObject3DAndCamera(o, cam);
    int direction = getDirectionForAngle(enemyAngle);

    counterAnimations->update();

    if (counterAnimations->isFinished()) {
        counterAnimations->setEnabled(true);
        getCurrentTextureAnimationDirectional()->nextFrame();
    }

    if (getCurrentTextureAnimationDirectional()->hasZeroDirection()) {
        billboard->setImage(getCurrentTextureAnimationDirectional()->getCurrentFrame(0));
    } else {
        auto t = getCurrentTextureAnimationDirectional()->getCurrentFrame(direction);
        billboard->setImage(t);
    }
}

void Image3DAnimation8Directions::setAnimation(int indexAnimation)
{
    this->currentAnimation = indexAnimation;
    this->updateStep();
    this->counterAnimations->setStep(step);
}

TextureAnimatedDirectional *Image3DAnimation8Directions::getCurrentTextureAnimationDirectional() const
{
    return this->animations[currentAnimation];
}

int Image3DAnimation8Directions::getDirectionForAngle(float enemyAngle)
{
    if (enemyAngle >= 292.5f && enemyAngle < 337.5f)
        return 8;
    if (enemyAngle >= 22.5f && enemyAngle < 67.5f)
        return 2;
    if (enemyAngle >= 67.5f && enemyAngle < 112.5f)
        return 3;
    if (enemyAngle >= 112.5f && enemyAngle < 157.5f)
        return 4;
    if (enemyAngle >= 157.5f && enemyAngle < 202.5f)
        return 5;
    if (enemyAngle >= 202.5f && enemyAngle < 247.5f)
        return 6;
    if (enemyAngle >= 247.5f && enemyAngle < 292.5f)
        return 7;
    if (enemyAngle >= 337.5f || enemyAngle < 22.5f)
        return 1;

    return 1;
}

void Image3DAnimation8Directions::updateStep()
{
    step = 1 / this->getCurrentTextureAnimationDirectional()->getFps();
    this->counterAnimations->setStep(step);
}

void Image3DAnimation8Directions::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();
    Image3DAnimation8DirectionsGUI::drawImGuiProperties(this);
}

const char *Image3DAnimation8Directions::getTypeObject()
{
    return SceneObjectTypes::BILLBOARD_ANIMATION_8DIR;
}

const char *Image3DAnimation8Directions::getTypeIcon()
{
    return IconsByObject::BILLBOARD_ANIMATION_8DIR;
}

void Image3DAnimation8Directions::updateBillboardSize() const
{
    billboard->setWidth(width);
    billboard->setHeight(height);
}

Image3DAnimation8Directions* Image3DAnimation8Directions::create(
        Vertex3D position,
        float width,
        float height,
        const std::string &folderSprite,
        int frames,
        int fps
) {
    auto o = new Image3DAnimation8Directions(position, width, height);
    o->addAnimationDirectional2D(folderSprite, frames, fps, false, -1);
    o->setAnimation(0);
    o->setPosition(position);

    return o;
}