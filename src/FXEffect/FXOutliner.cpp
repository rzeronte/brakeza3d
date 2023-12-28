#include "../../include/FXEffect/FXOutliner.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Logging.h"

FXOutliner::FXOutliner(bool active, Object3D *object, const Color &color, float size)
:
    FXEffectOpenGLObject(active, object),
    color(color),
    size(size)

{
    setLabel("FXOutliner");
}

void FXOutliner::preUpdate()
{
    FXEffectOpenGL::preUpdate();
    update();
}

void FXOutliner::postUpdate()
{
    FXEffectOpenGL::postUpdate();
}

void FXOutliner::update()
{
    FXEffectBase::update();

    if (!isEnabled()) return;

    if (this->object == nullptr) {
        return;
    }

    auto mesh = dynamic_cast<Mesh3D*> (object);
    if (mesh == nullptr) {
        return;
    }

    Drawable::drawOutline(mesh);
}

void FXOutliner::setColor(Color c) {
    this->color = c;
}

void FXOutliner::drawImGuiProperties()
{
    const float range_size_min = 0;
    const float range_size_max = 50;
    const float range_size_sensibility = 0.1f;
    if (ImGui::TreeNode(("Edge size###"+ getLabel()).c_str())) {
        ImGui::DragScalar("Size", ImGuiDataType_Float, &size, range_size_sensibility, &range_size_min, &range_size_max, "%f", 1.0f);
        ImGui::TreePop();
    }

    // Color
    if (ImGui::TreeNode(("Edge color##" + getLabel()).c_str())) {
        static ImVec4 imguiColor;
        imguiColor.x = color.r/255;
        imguiColor.y = color.g/255;
        imguiColor.z = color.b/255;

        bool changed_color = ImGui::ColorEdit4("Color", (float *) &imguiColor, ImGuiColorEditFlags_NoOptions);
        if (changed_color) {
            setColor(
                    Color(
                            (int) (imguiColor.x * 256),
                            (int) (imguiColor.y * 256),
                            (int) (imguiColor.z * 256)
                    )
            );
        }
        ImGui::TreePop();
    }
}

cJSON * FXOutliner::getJSON()
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "type", this->getLabel().c_str());
    cJSON_AddNumberToObject(root, "size", size);
    cJSON *colorJSON = cJSON_CreateObject();
    cJSON_AddNumberToObject(colorJSON, "r", color.r);
    cJSON_AddNumberToObject(colorJSON, "g", color.g);
    cJSON_AddNumberToObject(colorJSON, "b", color.b);
    cJSON_AddItemToObject(root, "color", colorJSON);

    return root;
}

void FXOutliner::setSize(float size) {
    FXOutliner::size = size;
}

float FXOutliner::getSize() const {
    return size;
}

FXOutliner *FXOutliner::create()
{
    return new FXOutliner(true, nullptr, Color::green(), 1);
}
