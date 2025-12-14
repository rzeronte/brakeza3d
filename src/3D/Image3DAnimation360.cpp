
#include "../../include/3D/Image3DAnimation360.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Misc/ToolsMaths.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/Image3DAnimation360GUI.h"

Image3DAnimation360::Image3DAnimation360(const Vertex3D &position, float width, float height)
:
    billboard(new Image3D(position, width, height, nullptr)),
    width(width),
    height(height)
{
    counterAnimations = new Counter();
}

void Image3DAnimation360::onUpdate()
{
    if ((int) animations.size() <= 0) return;

    UpdateTrianglesCoordinates(Components::get()->Camera()->getCamera());

    Components::get()->Render()->getShaders()->shaderOGLRender->render(
        this,
        billboard->getImage()->getOGLTextureID(),
        billboard->getImage()->getOGLTextureID(),
        billboard->getVertexBuffer(),
        billboard->getUVBuffer(),
        billboard->getNormalBuffer(),
        static_cast<int>(billboard->getVertices().size()),
        1.0f,
        Components::get()->Window()->getForegroundFramebuffer()
    );
}

void Image3DAnimation360::UpdateTrianglesCoordinates(Camera3D *cam)
{
    UpdateTextureFromCameraAngle(this, cam);
}

void Image3DAnimation360::CreateAnimationDirectional2D(TextureAnimatedDirectional* animation, bool zeroDirection)
{
    if (!zeroDirection) {
        animation->LoadImages();
    } else {
        animation->LoadImagesForZeroDirection();
        animation->setIsZeroDirection(true);
    }
    animations.push_back(animation);
}

void Image3DAnimation360::CreateAnimationDirectional2D(const std::string& folder, int numFrames,int fps, bool zeroDirection, int maxTimes)
{
    Logging::Message("[Image3DAnimation360] Adding directional texture to SpriteDirectional(%s)...", folder.c_str(), getName().c_str());

    animations.push_back(new TextureAnimatedDirectional(folder, numFrames, fps, maxTimes));
}

void Image3DAnimation360::LoadAnimations() const
{
    for (auto &a: animations) {
        if (a->hasZeroDirection()) {
            a->LoadImagesForZeroDirection();
            a->setIsZeroDirection(true);
        } else {
            a->LoadImages();
        }
    }
}

void Image3DAnimation360::UpdateTextureFromCameraAngle(Object3D *o, Camera3D *cam) const
{
    if (animations.empty()) return;

    float enemyAngle = ToolsMaths::getHorizontalAngleBetweenObject3DAndCamera(o, cam);
    int direction = getDirectionForAngle(enemyAngle);

    counterAnimations->update();

    if (counterAnimations->isFinished()) {
        counterAnimations->setEnabled(true);
        getCurrentTextureAnimationDirectional()->NextFrame();
    }

    if (getCurrentTextureAnimationDirectional()->hasZeroDirection()) {
        billboard->setImage(getCurrentTextureAnimationDirectional()->getCurrentFrame(0));
    } else {
        auto t = getCurrentTextureAnimationDirectional()->getCurrentFrame(direction);
        billboard->setImage(t);
    }
}

void Image3DAnimation360::setAnimation(int indexAnimation)
{
    if (indexAnimation < 0 ) return;

    this->currentIndexAnimation = indexAnimation;
    this->UpdateStep();
    this->counterAnimations->setStep(step);
}

int Image3DAnimation360::getDirectionForAngle(float enemyAngle)
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

void Image3DAnimation360::UpdateStep()
{
    step = 1.0f / (float) this->getCurrentTextureAnimationDirectional()->getFps();
    counterAnimations->setStep(step);
}

void Image3DAnimation360::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    Image3DAnimation360GUI::DrawPropertiesGUI(this);
}

void Image3DAnimation360::UpdateBillboardSize() const
{
    billboard->setWidth(width);
    billboard->setHeight(height);
}