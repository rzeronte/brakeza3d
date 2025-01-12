
#include "../../include/Objects/BillboardAnimation8Directions.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Logging.h"
#include "../../include/Render/Maths.h"
#include "../../include/ComponentsManager.h"

BillboardAnimation8Directions::BillboardAnimation8Directions(float width, float height)
:
    width(width),
    height(height)
{
    this->billboard = new Billboard(width, height);
    this->counterAnimations = new Counter();

    for (auto & animation : this->animations) {
        animation = new TextureAnimatedDirectional();
    }
}

void BillboardAnimation8Directions::onUpdate()
{
    if (numAnimations <= 0) return;

    this->updateTrianglesCoordinates(ComponentsManager::get()->getComponentCamera()->getCamera());

    ComponentsManager::get()->getComponentWindow()->getShaderOGLRender()->render(
            this,
            billboard->getTexture()->getOGLTextureID(),
            billboard->getTexture()->getOGLTextureID(),
            billboard->vertexbuffer,
            billboard->uvbuffer,
            billboard->normalbuffer,
            (int) billboard->vertices.size(),
            1.0f,
            ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );
}

Billboard *BillboardAnimation8Directions::getBillboard() const
{
    return billboard;
}

void BillboardAnimation8Directions::updateTrianglesCoordinates(Camera3D *cam)
{
    Vertex3D up = cam->getRotation().getTranspose() * EngineSetup::get()->up;
    Vertex3D right = cam->getRotation().getTranspose() * EngineSetup::get()->right;

    billboard->updateUnconstrainedQuad(this, up, right);
    updateTextureFromCameraAngle(this, cam);
}

void BillboardAnimation8Directions::addAnimationDirectional2D(
    const std::string& animation_folder,
    int numFrames,
    int fps,
    bool zeroDirection,
    int maxTimes
)
{
    Logging::Message("Adding TextureAnimatedDirectional to SpriteDirectional(%s)...", animation_folder.c_str(), getLabel().c_str());

    this->animations[this->numAnimations]->setup(
        animation_folder,
        numFrames,
        fps,
        maxTimes
    );

    if (!zeroDirection) {
        this->animations[this->numAnimations]->loadImages();
    } else {
        this->animations[this->numAnimations]->loadImagesForZeroDirection();
        this->animations[this->numAnimations]->isZeroDirection = true;
    }

    this->numAnimations++;
}

void BillboardAnimation8Directions::updateTextureFromCameraAngle(Object3D *o, Camera3D *cam)
{
    if (numAnimations == 0) return;

    float enemyAngle = Maths::getHorizontalAngleBetweenObject3DAndCamera(o, cam);
    int direction = getDirectionForAngle(enemyAngle);

    counterAnimations->update();

    if (counterAnimations->isFinished()) {
        counterAnimations->setEnabled(true);
        getCurrentTextureAnimationDirectional()->nextFrame();
    }

    if (getCurrentTextureAnimationDirectional()->isZeroDirection) {
        billboard->setTexture(getCurrentTextureAnimationDirectional()->getCurrentFrame(0));
    } else {
        auto t = getCurrentTextureAnimationDirectional()->getCurrentFrame(direction);
        billboard->setTexture(t);
    }
}

void BillboardAnimation8Directions::setAnimation(int indexAnimation)
{
    this->currentAnimation = indexAnimation;
    this->updateStep();
    this->counterAnimations->setStep(step);
}

void BillboardAnimation8Directions::linkTexturesTo(BillboardAnimation8Directions *clone)
{
    this->numAnimations = clone->numAnimations;
    for (int i = 0; i < clone->numAnimations; i++) {
        this->animations[i]->importTextures(clone->animations[i], clone->animations[i]->numFrames);
        this->animations[i]->isZeroDirection = clone->animations[i]->isZeroDirection;
        this->animations[i]->numFrames = clone->animations[i]->numFrames;
        this->animations[i]->base_file = clone->animations[i]->base_file;
        this->animations[i]->maxTimes = clone->animations[i]->maxTimes;
    }
}

TextureAnimatedDirectional *BillboardAnimation8Directions::getCurrentTextureAnimationDirectional()
{
    return this->animations[currentAnimation];
}

int BillboardAnimation8Directions::getDirectionForAngle(float enemyAngle)
{
    if (enemyAngle >= 292.5f && enemyAngle < 337.5f)
        return 8;
    else if (enemyAngle >= 22.5f && enemyAngle < 67.5f)
        return 2;
    else if (enemyAngle >= 67.5f && enemyAngle < 112.5f)
        return 3;
    else if (enemyAngle >= 112.5f && enemyAngle < 157.5f)
        return 4;
    else if (enemyAngle >= 157.5f && enemyAngle < 202.5f)
        return 5;
    else if (enemyAngle >= 202.5f && enemyAngle < 247.5f)
        return 6;
    else if (enemyAngle >= 247.5f && enemyAngle < 292.5f)
        return 7;
    else if (enemyAngle >= 337.5f || enemyAngle < 22.5f)
        return 1;

    return 1;
}

void BillboardAnimation8Directions::updateStep()
{
    step = (float) 1 / (float) this->getCurrentTextureAnimationDirectional()->fps;
    this->counterAnimations->setStep(step);
}

void BillboardAnimation8Directions::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();

    if (ImGui::TreeNode("BillboardAnimation8Directions")) {
        if (ImGui::TreeNode("Size")) {
            const float range_min = 0;
            const float range_max = 1000;
            const float range_sensibility = 0.1;

            if (ImGui::DragScalar("Width", ImGuiDataType_Float, &width, range_sensibility ,&range_min, &range_max, "%f", 1.0f)){
            }
            if (ImGui::DragScalar("Height", ImGuiDataType_Float, &height, range_sensibility ,&range_min, &range_max, "%f", 1.0f)) {
            }
            ImGui::TreePop();
        }

        ImGui::Separator();

        static char name[256];
        strncpy(name, currentSpriteFileVariableToCreateAnimation.c_str(), sizeof(name));

        if (ImGui::InputText("Sprite Folder##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
            currentSpriteFileVariableToCreateAnimation = name;
        }
        if (ImGui::InputInt("Num. Frames", &currentFramesVariableToCreateAnimation)) {
        }

        if (ImGui::Button(std::string("Load directional animation").c_str())) {
            addAnimationDirectional2D(
                currentSpriteFileVariableToCreateAnimation,
                currentFramesVariableToCreateAnimation,
                24,
                false,
                -1
            );
            setAnimation(this->numAnimations-1);
            currentSpriteFileVariableToCreateAnimation = "";
            currentFramesVariableToCreateAnimation = 0;
        }

        ImGui::Separator();

        const char* items[numAnimations];
        for (int i = 0; i < numAnimations; i++) {
            items[i] = animations[i]->base_file.c_str();
        }
        ImGui::Combo("Animation", &currentAnimation, items, IM_ARRAYSIZE(items));

        getCurrentTextureAnimationDirectional()->drawImGuiProperties();

        ImGui::TreePop();
    }
}

const char *BillboardAnimation8Directions::getTypeObject()
{
    return "BillboardAnimation8Directions";
}

const char *BillboardAnimation8Directions::getTypeIcon() {
    return "BillboardAnimation8DirectionsIcon";
}