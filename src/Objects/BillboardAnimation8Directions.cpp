
#include "../../include/Objects/BillboardAnimation8Directions.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Logging.h"
#include "../../include/Render/Maths.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

BillboardAnimation8Directions::BillboardAnimation8Directions(float width, float height)
:
    width(width),
    height(height)
{
    this->billboard = new Billboard(width, height);
    this->counterAnimations = new Counter();

    /*for (auto & animation : this->animations) {
        animation = new TextureAnimatedDirectional();
    }*/
}

void BillboardAnimation8Directions::onUpdate()
{
    if ((int) animations.size() <= 0) return;

    this->updateTrianglesCoordinates(ComponentsManager::get()->getComponentCamera()->getCamera());

    ComponentsManager::get()->getComponentRender()->getShaderOGLRender()->render(
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

void BillboardAnimation8Directions::updateTrianglesCoordinates(Camera3D *cam)
{
    Vertex3D up = cam->getRotation().getTranspose() * EngineSetup::get()->up;
    Vertex3D right = cam->getRotation().getTranspose() * EngineSetup::get()->right.getInverse();

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
        animation->isZeroDirection = true;
    }

    animations.push_back(animation);
}

void BillboardAnimation8Directions::updateTextureFromCameraAngle(Object3D *o, Camera3D *cam)
{
    if ((int) animations.size() <= 0) return;

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
                setAnimation((int) animations.size()-1);
                currentSpriteFileVariableToCreateAnimation = "";
                currentFramesVariableToCreateAnimation = 0;
            }

            ImGui::TreePop();
        }

        ImGui::Separator();

        const char* items[(int) animations.size()];
        for (int i = 0; i < (int) animations.size(); i++) {
            items[i] = animations[i]->base_file.c_str();
        }

        if ((int) animations.size() > 0) {
            ImGui::Combo("Animation", &currentAnimation, items, IM_ARRAYSIZE(items));
        } else {
            ImGui::Text("No animations found!");
        }

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


cJSON *BillboardAnimation8Directions::getJSON()
{
    auto root =  Object3D::getJSON();

    cJSON_AddNumberToObject(root, "width", width);
    cJSON_AddNumberToObject(root, "height", height);

    cJSON *animationsArrayJSON = cJSON_CreateArray();
    for (auto a : animations) {
        if (!a->isLoaded()) continue;
        cJSON *animationJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(animationJSON, "folder", a->base_file.c_str());
        cJSON_AddNumberToObject(animationJSON, "frames", (int) a->numFrames);
        cJSON_AddNumberToObject(animationJSON, "fps", (int) a->fps);
        cJSON_AddItemToArray(animationsArrayJSON, animationJSON);
    }
    cJSON_AddItemToObject(root, "animations", animationsArrayJSON);
    cJSON_AddNumberToObject(root, "currentIndexAnimation", currentAnimation);

    return root;
}

void BillboardAnimation8Directions::createFromJSON(cJSON *object)
{
    auto o = new BillboardAnimation8Directions(1, 1);

    BillboardAnimation8Directions::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void BillboardAnimation8Directions::setPropertiesFromJSON(cJSON *object, BillboardAnimation8Directions *o)
{
    o->setBelongToScene(true);
    Object3D::setPropertiesFromJSON(object, o);

    o->width = (float) cJSON_GetObjectItemCaseSensitive(object, "width")->valueint;
    o->height = (float) cJSON_GetObjectItemCaseSensitive(object, "height")->valueint;

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

void BillboardAnimation8Directions::updateBillboardSize()
{
    billboard->updateSize(width, height);
}

BillboardAnimation8Directions* BillboardAnimation8Directions::create(
        Vertex3D position,
        float width,
        float height,
        const std::string &folderSprite,
        int frames,
        int fps
) {
    auto o = new BillboardAnimation8Directions(width, height);
    o->addAnimationDirectional2D(folderSprite, frames, fps, false, -1);
    o->setAnimation(0);
    o->setPosition(position);

    return o;
}