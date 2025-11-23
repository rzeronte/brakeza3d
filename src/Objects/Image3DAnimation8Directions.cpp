
#include "../../include/Objects/Image3DAnimation8Directions.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Logging.h"
#include "../../include/Render/Maths.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

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
        if (ImGui::TreeNode("Add animation")) {
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
                setAnimation(animations.size()-1);
                currentSpriteFileVariableToCreateAnimation = "";
                currentFramesVariableToCreateAnimation = 0;
            }

            ImGui::TreePop();
        }

        ImGui::Separator();

        const char* items[animations.size()];
        for (unsigned int i = 0; i < animations.size(); i++) {
            items[i] = animations[i]->getBaseFile().c_str();
        }

        if (!animations.empty()) {
            ImGui::Combo("Animation", &currentAnimation, items, IM_ARRAYSIZE(items));
        } else {
            ImGui::Text("No animations found!");
        }

        ImGui::TreePop();
    }
}

const char *Image3DAnimation8Directions::getTypeObject()
{
    return "BillboardAnimation8Directions";
}

const char *Image3DAnimation8Directions::getTypeIcon() {
    return "BillboardAnimation8DirectionsIcon";
}

cJSON *Image3DAnimation8Directions::getJSON(Image3DAnimation8Directions *o)
{
    auto root = Object3D::ReadJSONFromObject(o);

    cJSON_AddNumberToObject(root, "width", width);
    cJSON_AddNumberToObject(root, "height", height);

    cJSON *animationsArrayJSON = cJSON_CreateArray();
    for (auto a : animations) {
        if (!a->isLoaded()) continue;
        cJSON *animationJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(animationJSON, "folder", a->getBaseFile().c_str());
        cJSON_AddNumberToObject(animationJSON, "frames", a->getNumFrames());
        cJSON_AddNumberToObject(animationJSON, "fps", a->getFps());
        cJSON_AddItemToArray(animationsArrayJSON, animationJSON);
    }
    cJSON_AddItemToObject(root, "animations", animationsArrayJSON);
    cJSON_AddNumberToObject(root, "currentIndexAnimation", currentAnimation);

    return root;
}

void Image3DAnimation8Directions::createFromJSON(cJSON *object)
{
    auto o = new Image3DAnimation8Directions(Vertex3D(), 1, 1);

    setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void Image3DAnimation8Directions::setPropertiesFromJSON(cJSON *object, Image3DAnimation8Directions *o)
{
    o->setBelongToScene(true);
    Object3D::setPropertiesFromJSON(object, o);

    o->width = static_cast<float>(cJSON_GetObjectItemCaseSensitive(object, "width")->valueint);
    o->height = static_cast<float>(cJSON_GetObjectItemCaseSensitive(object, "height")->valueint);

    if (cJSON_GetObjectItemCaseSensitive(object, "animations") != nullptr) {
        cJSON *currentAnimation;
        cJSON_ArrayForEach(currentAnimation, cJSON_GetObjectItemCaseSensitive(object, "animations")) {
            auto folder = cJSON_GetObjectItemCaseSensitive(currentAnimation, "folder")->valuestring;
            auto frames = cJSON_GetObjectItemCaseSensitive(currentAnimation, "frames")->valueint;
            auto fps = cJSON_GetObjectItemCaseSensitive(currentAnimation, "fps")->valueint;

            o->addAnimationDirectional2D(folder, frames, fps, false, -1);
        }

        o->updateBillboardSize();

        int animationIndex = cJSON_GetObjectItemCaseSensitive(object, "currentIndexAnimation")->valueint;
        o->setAnimation(animationIndex);
        o->updateStep();
    }
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