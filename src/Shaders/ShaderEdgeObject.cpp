#include "../../include/Shaders/ShaderEdgeObject.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Brakeza3D.h"

ShaderEdgeObject::ShaderEdgeObject(bool active, Color c, float size)
:
    ObjectShaderOpenCL(active, nullptr),
    color(c),
    size(size)
{
    setLabel("ShaderEdgeObject");
}

void ShaderEdgeObject::preUpdate() {
    ShaderOpenCL::preUpdate();
    update();

}

void ShaderEdgeObject::postUpdate() {
    ShaderOpenCL::postUpdate();
}

void ShaderEdgeObject::update()
{
    Shader::update();

    if (!isEnabled()) return;

    if (this->object == nullptr) {
        return;
    }

    if (!this->object->isStencilBufferEnabled()) {
        return;
    }

    auto mesh = dynamic_cast<Mesh3D*> (object);
    if (mesh == nullptr) {
        return;
    }

    executeKernelOpenCL();
}

void ShaderEdgeObject::executeKernelOpenCL()
{
    auto kernel = ComponentsManager::get()->getComponentRender()->getEdgeKernel();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)dynamic_cast<Mesh3D*> (this->object)->getOpenClRenderer()->getClBufferStencil());
    clSetKernelArg(kernel, 5, sizeof(float), &this->color.r);
    clSetKernelArg(kernel, 6, sizeof(float), &this->color.g);
    clSetKernelArg(kernel, 7, sizeof(float), &this->color.b);
    clSetKernelArg(kernel, 8, sizeof(float), &this->size);

    size_t global_item_size[2] = {(size_t) EngineSetup::get()->screenWidth, (size_t) EngineSetup::get()->screenHeight};
    size_t local_item_size[2] = {16, 16};

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);

    this->debugKernel("ShaderEdge");
}

ShaderEdgeObject::~ShaderEdgeObject() {

}

void ShaderEdgeObject::setColor(Color c) {
    this->color = c;
}

void ShaderEdgeObject::drawImGuiProperties()
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

cJSON * ShaderEdgeObject::getJSON()
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

void ShaderEdgeObject::setSize(float size) {
    ShaderEdgeObject::size = size;
}

float ShaderEdgeObject::getSize() const {
    return size;
}

ShaderEdgeObject *ShaderEdgeObject::create() {
    return new ShaderEdgeObject(true, Color::green(), 1);
}
