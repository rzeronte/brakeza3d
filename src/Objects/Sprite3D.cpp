
#include "../../include/Objects/Sprite3D.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

Sprite3D::Sprite3D(float width, float height)
:
    width(width),
    height(height),
    billboard(new Billboard(width, height)),
    currentAnimationIndex(0),
    autoRemoveAfterAnimation(false),
    sharedTextures(false)
{
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
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
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
}

void Sprite3D::updateTexture()
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

void Sprite3D::updateBillboardSize()
{
    billboard->updateSize(width, height);
}

void Sprite3D::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();

    if (ImGui::TreeNode("Sprite3D")) {
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

        const char* items[(int) animations.size()];

        for (int i = 0; i < (int) animations.size(); i++) {
            items[i] = animations[i]->getBaseFilename().c_str();
        }
        ImGui::Combo("Type", &currentAnimationIndex, items, IM_ARRAYSIZE(items));
        getCurrentTextureAnimation()->drawImGuiProperties();

        ImGui::TreePop();
    }
}

cJSON *Sprite3D::getJSON()
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

void Sprite3D::setPropertiesFromJSON(cJSON *object, Sprite3D *o)
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

void Sprite3D::createFromJSON(cJSON *object)
{
    auto o = new Sprite3D(1, 1);

    Sprite3D::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

