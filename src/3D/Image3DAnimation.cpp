
#include "../../include/3D/Image3DAnimation.h"
#include "../../include/Components/Components.h"
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

    this->UpdateTrianglesCoordinatesAndTexture();

    Components::get()->Render()->getShaders()->shaderOGLRender->render(
        this,
        getCurrentTextureAnimation()->getCurrentFrame()->getOGLTextureID(),
        getCurrentTextureAnimation()->getCurrentFrame()->getOGLTextureID(),
        billboard->getVertexBuffer(),
        billboard->getUVBuffer(),
        billboard->getNormalBuffer(),
        billboard->getVertices().size(),
        Components::get()->Window()->getForegroundFramebuffer()
    );

    if (Config::get()->TRIANGLE_MODE_WIREFRAME) {
        Components::get()->Render()->getShaders()->shaderOGLWireframe->render(
            getModelMatrix(),
            billboard->getVertexBuffer(),
            billboard->getUVBuffer(),
            billboard->getNormalBuffer(),
            billboard->getVertices().size(),
            Color::gray(),
            Components::get()->Window()->getSceneFramebuffer()
        );
    }
}

void Image3DAnimation::CreateAnimation(const std::string& sprite, int w, int h, int numFrames, int fps)
{
    this->animations.emplace_back(new TextureAnimated(sprite, w, h, numFrames, fps));
}

void Image3DAnimation::UpdateTexture()
{
    if (animations.empty()) return;

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

void Image3DAnimation::UpdateTrianglesCoordinatesAndTexture()
{
    billboard->ResetBuffersToSize(width, height);
    UpdateTexture();
}

void Image3DAnimation::LoadAnimationFiles() const
{
    for (auto &a : animations) {
        a->LoadCurrentSetup();
    }
}

void Image3DAnimation::LinkTextureIntoAnotherImage3DAnimation(const Image3DAnimation *to)
{
    animations.clear();

    for (auto animation : to->animations) {
        animations.push_back(animation);
    }

    sharedTextures = true;
}

void Image3DAnimation::UpdateBillboardSize() const
{
    billboard->setWidth(width);
    billboard->setHeight(height);
}

void Image3DAnimation::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    Image3DAnimationGUI::DrawPropertiesGUI(this);
}

void Image3DAnimation::setAnimation(int value)
{
    this->currentAnimationIndex = value;
}

void Image3DAnimation::setAutoRemoveAfterAnimation(bool value)
{
    autoRemoveAfterAnimation = value;
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
