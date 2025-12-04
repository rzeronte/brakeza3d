
#include "../../include/3D/Image3DAnimation.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Objects/Image3DAnimationGUI.h"

Image3DAnimation::Image3DAnimation(const Vertex3D &position, float w, float h)
:
    width(w),
    height(h),
    currentAnimationIndex(0),
    billboard(new Image3D(position, w, h, nullptr))
{
    setPosition(position);
    luaEnvironment["this"] = this;
}

void Image3DAnimation::onUpdate()
{
    Object3D::onUpdate();

    this->updateTrianglesCoordinatesAndTexture();

    ComponentsManager::get()->getComponentRender()->getShaderOGLRenderForward()->render(
        this,
        static_cast<int>(getCurrentTextureAnimation()->getCurrentFrame()->getOGLTextureID()),
        static_cast<int>(getCurrentTextureAnimation()->getCurrentFrame()->getOGLTextureID()),
        billboard->getVertexBuffer(),
        billboard->getUVBuffer(),
        billboard->getNormalBuffer(),
        static_cast<int>(billboard->getVertices().size()),
        1.0f,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );

    if (Config::get()->TRIANGLE_MODE_WIREFRAME) {
        ComponentsManager::get()->getComponentRender()->getShaderOGLWireframe()->render(
            getModelMatrix(),
            billboard->getVertexBuffer(),
            billboard->getUVBuffer(),
            billboard->getNormalBuffer(),
            static_cast<int>(billboard->getVertices().size()),
            Color::gray(),
            ComponentsManager::get()->getComponentWindow()->getSceneFramebuffer()
        );
    }
}

void Image3DAnimation::addAnimation(const std::string& sprite, int w, int h, int numFrames, int fps)
{
    this->animations.emplace_back(new TextureAnimated(sprite, w, h, numFrames, fps));
}

void Image3DAnimation::setAnimation(int value)
{
    this->currentAnimationIndex = value;
}

void Image3DAnimation::updateTexture()
{
    if (static_cast<int>(animations.size()) == 0) return;

    getCurrentTextureAnimation()->counter.update();

    if (getCurrentTextureAnimation()->counter.isFinished()) {
        getCurrentTextureAnimation()->counter.setEnabled(true);
        getCurrentTextureAnimation()->nextFrame();
        if (this->isAutoRemoveAfterAnimation() && getCurrentTextureAnimation()->isEndAnimation()) {
            this->setRemoved(true);
        }
    }

    billboard->setImage(this->animations[currentAnimationIndex]->getCurrentFrame());
}

void Image3DAnimation::updateTrianglesCoordinatesAndTexture()
{
    billboard->setSize(width, height);
    updateTexture();
}

bool Image3DAnimation::isAutoRemoveAfterAnimation() const
{
    return autoRemoveAfterAnimation;
}

void Image3DAnimation::setAutoRemoveAfterAnimation(bool value)
{
    autoRemoveAfterAnimation = value;
}

void Image3DAnimation::LinkTextureIntoAnotherImage3DAnimation(const Image3DAnimation *to)
{
    animations.clear();

    for (auto animation : to->animations) {
        animations.push_back(animation);
    }

    sharedTextures = true;
}

TextureAnimated *Image3DAnimation::getCurrentTextureAnimation() const
{
    return this->animations[currentAnimationIndex];
}

Image3DAnimation::~Image3DAnimation()
{
    delete billboard;

    if (!sharedTextures) {
        for (auto animation : animations) {
            delete animation;
        }
    }
}

ObjectTypes Image3DAnimation::getTypeObject() const
{
    return ObjectTypes::Image3DAnimation;
}

GUISheet Image3DAnimation::getIcon()
{
    return IconObject::IMAGE_3D_ANIMATION;
}

void Image3DAnimation::updateBillboardSize() const
{
    billboard->setWidth(width);
    billboard->setHeight(height);
}

void Image3DAnimation::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    Image3DAnimationGUI::DrawPropertiesGUI(this);
}

Image3DAnimation* Image3DAnimation::create(
    const Vertex3D &position,
    float width,
    float height,
    const std::string &imageFile,
    int spriteWidth,
    int spriteHeight,
    int frames,
    int fps
) {
    auto o = new Image3DAnimation(position, width, height);
    o->addAnimation(imageFile, spriteWidth, spriteHeight, frames, fps);
    o->setAnimation(0);
    o->setPosition(position);

    return o;
}
