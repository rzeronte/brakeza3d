
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

    openClRenderer = new MeshOpenCLRenderer(this, this->billboard->getTriangles());
}

void Sprite3D::onUpdate()
{
    Object3D::onUpdate();

    this->updateTrianglesCoordinatesAndTexture();

    if (isRemoved()) return;

    //ComponentsManager::get()->getComponentRender()->getSpritesTriangles().emplace_back(billboard->getT1());
    //ComponentsManager::get()->getComponentRender()->getSpritesTriangles().emplace_back(billboard->getT2());

    openClRenderer->updateTriangles();

    openClRenderer->onUpdate(billboard->getT1()->getTexture());
}

void Sprite3D::postUpdate()
{
    Object3D::postUpdate();
}

void Sprite3D::addAnimation(const std::string& animation2d, int num_frames, int fps)
{
    this->animations.emplace_back(new TextureAnimated(animation2d, num_frames, fps));
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

    billboard->setTrianglesTexture(this->animations[currentAnimationIndex]->getCurrentFrame());
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
