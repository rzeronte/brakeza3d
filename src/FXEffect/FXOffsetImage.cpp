#include "../../include/FXEffect/FXOffsetImage.h"
#include "../../include/LUAManager.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

FXOffsetImage::FXOffsetImage(const std::string& filename)
:
    FXEffectOpenGL(true),
    image(Image(filename)),
    offsetX(0),
    offsetY(0),
    alpha(1.0f),
    intensity(0.75f)
{
    setLabel("FXOffsetImage");
}

void FXOffsetImage::preUpdate()
{
    if (!this->enabled) return;
    update();
}

void FXOffsetImage::postUpdate()
{
}

void FXOffsetImage::update()
{
    FXEffectBase::update();
}

void FXOffsetImage::update(float newOffsetX, float newOffsetY)
{
    FXEffectBase::update();

    if (!isEnabled()) return;

    offsetX += newOffsetX;
    offsetY += newOffsetY;

    limitOffset();

    auto game = ComponentsManager::get()->getComponentGame();
    auto window = ComponentsManager::get()->getComponentWindow();

    game->getShaderOGLImageOffset()->render(
        this->image.getOGLTextureID(),
        offsetX,
        offsetY,
        intensity,
        alpha,
        window->getBackgroundFramebuffer()
    );
}

void FXOffsetImage::limitOffset()
{
    const float maxOffset = 0.1f;
    const float minOffset = -0.1f;

    offsetX = fminf(maxOffset, fmaxf(minOffset, offsetX));
    offsetY = fminf(maxOffset, fmaxf(minOffset, offsetY));
}

void FXOffsetImage::resetOffsets()
{
    offsetX = 0;
    offsetY = 0;
}

Image &FXOffsetImage::getImage() {
    return image;
}

void FXOffsetImage::drawImGuiProperties()
{
    const float range_min = -0;
    const float range_max = 1;
    const float range_sensibility = 0.001;

    ImGui::DragScalar("Intensity", ImGuiDataType_Float, &intensity, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
    ImGui::DragScalar("OffsetX", ImGuiDataType_Float, &offsetX, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
    ImGui::DragScalar("OffsetY", ImGuiDataType_Float, &offsetY, range_sensibility ,&range_min, &range_max, "%f", 1.0f);

    ImGui::TreePop();
}

cJSON *FXOffsetImage::getJSON()
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "type", this->getLabel().c_str());
    cJSON_AddStringToObject(root, "image", image.getFileName().c_str());
    cJSON_AddNumberToObject(root, "offsetX", offsetX);
    cJSON_AddNumberToObject(root, "offsetY", offsetY);
    cJSON_AddNumberToObject(root, "intensity", intensity);

    return root;
}

void FXOffsetImage::setOffsets(float x, float y) {
    offsetX = x;
    offsetY = y;
}