
#include "../../include/Objects/BillboardAnimation.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

BillboardAnimation::BillboardAnimation(float width, float height)
:
    billboard(new Billboard(width, height)),
    width(width),
    height(height),
    currentAnimationIndex(0),
    autoRemoveAfterAnimation(false),
    sharedTextures(false)
{
    luaEnvironment["this"] = this;
}

void BillboardAnimation::onUpdate()
{
    Object3D::onUpdate();

    this->updateTrianglesCoordinatesAndTexture();

    ComponentsManager::get()->getComponentWindow()->getShaderOGLRender()->render(
        this,
        (int) getCurrentTextureAnimation()->getCurrentFrame()->getOGLTextureID(),
        (int) getCurrentTextureAnimation()->getCurrentFrame()->getOGLTextureID(),
        billboard->vertexbuffer,
        billboard->uvbuffer,
        billboard->normalbuffer,
        (int) billboard->vertices.size(),
        1.0f,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );

    if (EngineSetup::get()->TRIANGLE_MODE_WIREFRAME) {
        ComponentsManager::get()->getComponentWindow()->getShaderOglWireframe()->render(
            getModelMatrix(),
            billboard->vertexbuffer,
            billboard->uvbuffer,
            billboard->normalbuffer,
            (int) billboard->vertices.size(),
            ComponentsManager::get()->getComponentWindow()->getSceneFramebuffer()
        );
    }
}

void BillboardAnimation::addAnimation(const std::string& spriteSheetFile, int spriteWidth, int spriteHeight, int numFrames, int fps)
{
    this->animations.emplace_back(new TextureAnimated(spriteSheetFile, spriteWidth, spriteHeight, numFrames, fps));
}

void BillboardAnimation::setAnimation(int index_animation)
{
    this->currentAnimationIndex = index_animation;
}

void BillboardAnimation::updateTexture()
{
    if ((int) animations.size() == 0) return;

    getCurrentTextureAnimation()->counter.update();

    if (getCurrentTextureAnimation()->counter.isFinished()) {
        getCurrentTextureAnimation()->counter.setEnabled(true);
        getCurrentTextureAnimation()->nextFrame();
        if (this->isAutoRemoveAfterAnimation() && getCurrentTextureAnimation()->isEndAnimation()) {
            this->setRemoved(true);
        }
    }

    billboard->setTexture(this->animations[currentAnimationIndex]->getCurrentFrame());
}

void BillboardAnimation::updateTrianglesCoordinatesAndTexture()
{
    M3 rotationTranspose = ComponentsManager::get()->getComponentCamera()->getCamera()->getRotation().getTranspose();

    Vertex3D up = rotationTranspose * EngineSetup::get()->up;
    Vertex3D right = rotationTranspose * EngineSetup::get()->right;

    billboard->updateUnconstrainedQuad(this, up, right);
    updateTexture();
}

bool BillboardAnimation::isAutoRemoveAfterAnimation() const {
    return autoRemoveAfterAnimation;
}

void BillboardAnimation::setAutoRemoveAfterAnimation(bool value) {
    BillboardAnimation::autoRemoveAfterAnimation = value;
}

void BillboardAnimation::linkTextureAnimation(BillboardAnimation *dst)
{
    animations.clear();

    for (auto animation : dst->animations) {
        animations.push_back(animation);
    }

    sharedTextures = true;
}

TextureAnimated *BillboardAnimation::getCurrentTextureAnimation()
{
    return this->animations[currentAnimationIndex];
}

BillboardAnimation::~BillboardAnimation()
{
    delete billboard;

    if (!sharedTextures) {
        for (auto animation : animations) {
            delete animation;
        }
    }
}

const char *BillboardAnimation::getTypeObject()
{
    return "BillboardAnimation";
}

const char *BillboardAnimation::getTypeIcon()
{
    return "BillboardAnimationIcon";
}

void BillboardAnimation::updateBillboardSize()
{
    billboard->updateSize(width, height);
}

void BillboardAnimation::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();

    if (ImGui::TreeNode("BillboardAnimation")) {
        if (ImGui::TreeNode("Size")) {
            const float range_min = 0;
            const float range_max = 1000;
            const float range_sensibility = 0.1;

            if (ImGui::DragScalar("Width", ImGuiDataType_Float, &width, range_sensibility ,&range_min, &range_max, "%f", 1.0f)){
                updateBillboardSize();
            }
            if (ImGui::DragScalar("Height", ImGuiDataType_Float, &height, range_sensibility ,&range_min, &range_max, "%f", 1.0f)) {
                updateBillboardSize();
            }
            ImGui::TreePop();
        }

        ImGui::Separator();
        if (ImGui::TreeNode("Add animation")) {
            static char name[256];
            strncpy(name, currentSpriteFileVariableToCreateAnimation.c_str(), sizeof(name));

            if (ImGui::InputText("Sprite Folder##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
                currentSpriteFileVariableToCreateAnimation = name;
            }
            ImGui::InputInt("Width", &currentWidthVariableToCreateAnimation);
            ImGui::InputInt("Height", &currentHeightVariableToCreateAnimation);
            ImGui::InputInt("Nº Frames", &currentFramesVariableToCreateAnimation);

            if (ImGui::Button(std::string("Load animation").c_str())) {
                if (!currentSpriteFileVariableToCreateAnimation.empty()) {
                    addAnimation(
                        currentSpriteFileVariableToCreateAnimation,
                        currentWidthVariableToCreateAnimation,
                        currentHeightVariableToCreateAnimation,
                        currentFramesVariableToCreateAnimation,
                        24
                    );
                    setAnimation((int) animations.size() - 1);
                    currentSpriteFileVariableToCreateAnimation = "";
                    currentFramesVariableToCreateAnimation = 0;
                    currentWidthVariableToCreateAnimation = 0;
                    currentHeightVariableToCreateAnimation = 0;
                }
            }

            ImGui::TreePop();
        }
        ImGui::Separator();

        const char* items[(int) animations.size()];
        for (int i = 0; i < (int) animations.size(); i++) {
            items[i] = animations[i]->getBaseFilename().c_str();
        }
        ImGui::Combo("Animation", &currentAnimationIndex, items, IM_ARRAYSIZE(items));

        getCurrentTextureAnimation()->drawImGuiProperties();

        ImGui::TreePop();
    }
}

cJSON *BillboardAnimation::getJSON()
{
    auto root =  Object3D::getJSON();

    cJSON_AddNumberToObject(root, "width", width);
    cJSON_AddNumberToObject(root, "height", height);

    cJSON *animationsArrayJSON = cJSON_CreateArray();
    for (auto a : animations) {
        cJSON *animationJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(animationJSON, "sprite", a->getBaseFilename().c_str());
        cJSON_AddNumberToObject(animationJSON, "width", (int) a->currentSpriteWidth);
        cJSON_AddNumberToObject(animationJSON, "height", (int) a->currentspriteHeight);
        cJSON_AddNumberToObject(animationJSON, "numberFrames", (int) a->numberFramesToLoad);
        cJSON_AddNumberToObject(animationJSON, "fps", (int) a->fps);
        cJSON_AddItemToArray(animationsArrayJSON, animationJSON);
    }
    cJSON_AddItemToObject(root, "animations", animationsArrayJSON);
    cJSON_AddNumberToObject(root, "currentIndexAnimation", currentAnimationIndex);

    return root;
}

void BillboardAnimation::setPropertiesFromJSON(cJSON *object, BillboardAnimation *o)
{
    o->setBelongToScene(true);
    Object3D::setPropertiesFromJSON(object, o);

    if (cJSON_GetObjectItemCaseSensitive(object, "animations") != nullptr) {
        cJSON *currentAnimation;
        cJSON_ArrayForEach(currentAnimation, cJSON_GetObjectItemCaseSensitive(object, "animations")) {
            auto spriteFile = cJSON_GetObjectItemCaseSensitive(currentAnimation, "sprite")->valuestring;
            auto width = cJSON_GetObjectItemCaseSensitive(currentAnimation, "width")->valueint;
            auto height = cJSON_GetObjectItemCaseSensitive(currentAnimation, "height")->valueint;
            auto fps = cJSON_GetObjectItemCaseSensitive(currentAnimation, "fps")->valueint;
            auto numberFrames = cJSON_GetObjectItemCaseSensitive(currentAnimation, "numberFrames")->valueint;

            o->addAnimation(spriteFile, width, height, numberFrames, fps);
        }

        o->width = (float) cJSON_GetObjectItemCaseSensitive(object, "width")->valuedouble;
        o->height = (float) cJSON_GetObjectItemCaseSensitive(object, "height")->valuedouble;

        o->updateBillboardSize();

        int animationIndex = cJSON_GetObjectItemCaseSensitive(object, "currentIndexAnimation")->valueint;
        o->setAnimation(animationIndex);
        o->getCurrentTextureAnimation()->updateStep();
    }
}

void BillboardAnimation::createFromJSON(cJSON *object)
{
    auto o = new BillboardAnimation(1, 1);

    BillboardAnimation::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

BillboardAnimation* BillboardAnimation::create(
    Vertex3D position,
    float width,
    float height,
    const std::string &imageFile,
    int spriteWidth,
    int spriteHeight,
    int frames,
    int fps
) {
    auto o = new BillboardAnimation(width, height);
    o->addAnimation(imageFile, spriteWidth, spriteHeight, frames, fps);
    o->setAnimation(0);
    o->setPosition(position);

    return o;
}
