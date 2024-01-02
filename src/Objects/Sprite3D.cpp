
#include "../../include/Objects/Sprite3D.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"

Sprite3D::Sprite3D(float width, float height):
    billboard(new Billboard(width, height)),
    currentAnimationIndex(0),
    step(0),
    autoRemoveAfterAnimation(false),
    sharedTextures(false)
{
    this->counter = Counter();
    this->counter.setEnabled(true);
    setTransparent(true);
}

void Sprite3D::onUpdate()
{
    Object3D::onUpdate();

    this->updateTrianglesCoordinatesAndTexture();

    ComponentsManager::get()->getComponentWindow()->getShaderOGLRender()->render(
        this,
        this->getCurrentTextureAnimation()->getCurrentFrame()->getOGLTextureID(),
        this->getCurrentTextureAnimation()->getCurrentFrame()->getOGLTextureID(),
        billboard->vertexbuffer,
        billboard->uvbuffer,
        billboard->normalbuffer,
        (int) billboard->vertices.size(),
        ComponentsManager::get()->getComponentWindow()->getBackgroundFramebuffer()
    );

    if (EngineSetup::get()->TRIANGLE_MODE_WIREFRAME){
        ComponentsManager::get()->getComponentWindow()->getShaderOglWireframe()->render(
            getModelMatrix(),
            billboard->vertexbuffer,
            billboard->uvbuffer,
            billboard->normalbuffer,
            (int) billboard->vertices.size()
        );
    }
}

void Sprite3D::addAnimation(const std::string& spriteSheetFile, int spriteWidth, int spriteHeight, int numFrames, int fps)
{
    this->animations.emplace_back(new TextureAnimated(spriteSheetFile, spriteWidth, spriteHeight, numFrames, fps));
}

void Sprite3D::setAnimation(int index_animation)
{
    this->currentAnimationIndex = index_animation;
    updateStep();
}

void Sprite3D::updateStep()
{
    this->step = (float) 1 / (float) this->getCurrentTextureAnimation()->getFps();
    this->counter.setStep(step);
}

void Sprite3D::updateTexture()
{
    if ((int) animations.size() == 0) return;

    counter.update();

    if (counter.isFinished()) {
        counter.setEnabled(true);
        getCurrentTextureAnimation()->nextFrame();
        if (this->isAutoRemoveAfterAnimation() && getCurrentTextureAnimation()->isEndAnimation()) {
            this->setRemoved(true);
        }
    }

    billboard->setTexture(this->animations[currentAnimationIndex]->getCurrentFrame());
}

void Sprite3D::updateTrianglesCoordinatesAndTexture()
{
    M3 rotationTranspose = ComponentsManager::get()->getComponentCamera()->getCamera()->getRotation().getTranspose();

    Vertex3D up = rotationTranspose * EngineSetup::get()->up;
    Vertex3D right = rotationTranspose * EngineSetup::get()->right;

    billboard->updateUnconstrainedQuad(this, up, right);
    updateTexture();
}

bool Sprite3D::isAutoRemoveAfterAnimation() const {
    return autoRemoveAfterAnimation;
}

void Sprite3D::setAutoRemoveAfterAnimation(bool value) {
    Sprite3D::autoRemoveAfterAnimation = value;
}

void Sprite3D::linkTextureAnimation(Sprite3D *dst)
{
    animations.clear();

    for (auto animation : dst->animations) {
        animations.push_back(animation);
    }

    sharedTextures = true;
}

TextureAnimated *Sprite3D::getCurrentTextureAnimation()
{
    return this->animations[currentAnimationIndex];
}

Sprite3D::~Sprite3D()
{
    delete billboard;

    if (!sharedTextures) {
        for (auto animation : animations) {
            delete animation;
        }
    }
}

const char *Sprite3D::getTypeObject() {
    return "Sprite3D";
}

const char *Sprite3D::getTypeIcon() {
    return "sprite3DIcon";
}
