//
// Created by eduardo on 13/08/23.
//

#include "../../include/2D/Sprite2D.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

Sprite2D::Sprite2D(int x, int y, bool removeWhenEnds, TextureAnimated *animation)
:
    x(x),
    y(y),
    removeWhenEnds(removeWhenEnds),
    animation(animation),
    ttl(nullptr)
{
    setTransparent(true);
}

Sprite2D::Sprite2D(int x, int y, float ttl, TextureAnimated *animation)
    :
    x(x),
    y(y),
    removeWhenEnds(false),
    animation(animation),
    ttl(new Counter(ttl))
{
    this->ttl->setEnabled(true);
    setTransparent(true);
}

void Sprite2D::onUpdate()
{
    animation->update();

    Object3D::onUpdate();

    float alpha = 255;

    if (ttl != nullptr) {
        ttl->update();
        const float ttlPercentage = ttl->currentPercentage();

        if (ttlPercentage <= 15.0f) {
            alpha = (ttlPercentage / 15.0f) * 255;
        } else if (ttlPercentage >= 80.0f) {
            alpha = 255 - ((ttlPercentage - 80.0f) / 20.0f * 255);
        }

        if (ttl->isFinished()) {
            setRemoved(true);
        }
    }

    const auto w = animation->getCurrentFrame()->width();
    const auto h = animation->getCurrentFrame()->height();

    animation->getCurrentFrame()->drawFlatAlpha(
        x - w/2,
        y - h/2,
        alpha/255,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );

    if (removeWhenEnds && animation->isEndAnimation()) {
        this->setRemoved(true);
    }
}

TextureAnimated *Sprite2D::getAnimation() const
{
    return animation;
}

void Sprite2D::updatePosition(int x, int y)
{
    this->x = x;
    this->y = y;
}

const char *Sprite2D::getTypeObject() {
    return "Sprite2D";
}

const char *Sprite2D::getTypeIcon() {
    return "sprite2DIcon";
}

void Sprite2D::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();
    if (ImGui::TreeNode("Sprite2D")) {

        const int range_min_int = 1;
        const int range_max_int = 1280;

        if (ImGui::TreeNode("Screen Position")) {
            ImGui::DragScalar("Offset X", ImGuiDataType_S32, &x,1.f, &range_min_int, &range_max_int, "%d", 1.0f);
            ImGui::DragScalar("Offset Y", ImGuiDataType_S32, &y,1.f, &range_min_int, &range_max_int, "%d", 1.0f);

            ImGui::TreePop();
        }

        animation->drawImGuiProperties();

        ImGui::TreePop();
    }
}

cJSON *Sprite2D::getJSON()
{
    auto root =  Object3D::getJSON();

    cJSON_AddNumberToObject(root, "x", x);
    cJSON_AddNumberToObject(root, "y", y);

    cJSON *animationJSON = cJSON_CreateObject();
    cJSON_AddStringToObject(animationJSON, "sprite", animation->getBaseFilename().c_str());
    cJSON_AddNumberToObject(animationJSON, "width", (int) animation->currentSpriteWidth);
    cJSON_AddNumberToObject(animationJSON, "height", (int) animation->currentspriteHeight);
    cJSON_AddNumberToObject(animationJSON, "numberFrames", (int) animation->numberFramesToLoad);
    cJSON_AddNumberToObject(animationJSON, "fps", (int) animation->fps);

    cJSON_AddItemToObject(root, "animation", animationJSON);

    return root;
}

void Sprite2D::createFromJSON(cJSON *object)
{
    auto animationJSON = cJSON_GetObjectItemCaseSensitive(object, "animation");

    auto spriteFile = cJSON_GetObjectItemCaseSensitive(animationJSON, "sprite")->valuestring;
    auto width = cJSON_GetObjectItemCaseSensitive(animationJSON, "width")->valueint;
    auto height = cJSON_GetObjectItemCaseSensitive(animationJSON, "height")->valueint;
    auto numberFrames = cJSON_GetObjectItemCaseSensitive(animationJSON, "numberFrames")->valueint;
    auto fps = cJSON_GetObjectItemCaseSensitive(animationJSON, "fps")->valueint;

    auto x = cJSON_GetObjectItemCaseSensitive(object, "x")->valueint;
    auto y = cJSON_GetObjectItemCaseSensitive(object, "y")->valueint;

    auto o = new Sprite2D(
        x,
        y,
        false,
        new TextureAnimated(spriteFile,width,height,numberFrames,fps)
    );
    Sprite2D::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void Sprite2D::setPropertiesFromJSON(cJSON *object, Sprite2D *o)
{
    o->setBelongToScene(true);
    Object3D::setPropertiesFromJSON(object, o);
}
