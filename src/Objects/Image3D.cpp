//
// Created by Eduardo on 09/01/2025.
//

#include "../../include/Objects/Image3D.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Brakeza3D.h"

Image3D::Image3D(Vertex3D position, float width, float height, Image* image)
:
    width(width),
    height(height),
    image(image)
{
    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &uvbuffer);
    glGenBuffers(1, &normalbuffer);

    setPosition(position);

    M3 rotationTranspose = ComponentsManager::get()->getComponentCamera()->getCamera()->getRotation().getTranspose();
    Vertex3D up = rotationTranspose * EngineSetup::get()->up;
    Vertex3D right = rotationTranspose * EngineSetup::get()->right;

    setSize(width, height, up, right);
}

void Image3D::setSize(float width, float height, Vertex3D U, Vertex3D  R)
{
    setWidth(width);
    setHeight(height);

    Vertex3D X;
    X.x = (width / 2) * R.x;
    X.y = (width / 2) * R.y;
    X.z = (width / 2) * R.z;

    Vertex3D Y;
    Y.x = (height / 2) * U.x;
    Y.y = (height / 2) * U.y;
    Y.z = (height / 2) * U.z;

    Q1.x = getPosition().x + X.x + Y.x;
    Q1.y = getPosition().y + X.y + Y.y;
    Q1.z = getPosition().z + X.z + Y.z;

    Q2.x = getPosition().x - X.x + Y.x;
    Q2.y = getPosition().y - X.y + Y.y;
    Q2.z = getPosition().z - X.z + Y.z;

    Q3.x = getPosition().x - X.x - Y.x;
    Q3.y = getPosition().y - X.y - Y.y;
    Q3.z = getPosition().z - X.z - Y.z;

    Q4.x = getPosition().x + X.x - Y.x;
    Q4.y = getPosition().y + X.y - Y.y;
    Q4.z = getPosition().z + X.z - Y.z;

    Q1 = Transforms::objectToLocal(Q1, this);
    Q2 = Transforms::objectToLocal(Q2, this);
    Q3 = Transforms::objectToLocal(Q3, this);
    Q4 = Transforms::objectToLocal(Q4, this);

    Q1.u = 1.0f;
    Q1.v = 1.0f;
    Q2.u = 0.0001f;
    Q2.v = 1.0f;
    Q3.u = 0.0001f;
    Q3.v = 0.0001f;
    Q4.u = 1.0f;
    Q4.v = 0.0001f;

    auto normal = U % R;
    vertices.clear();
    uvs.clear();
    normals.clear();

    vertices.emplace_back(Q3.x, Q3.y, Q3.z);
    vertices.emplace_back(Q2.x, Q2.y, Q2.z);
    vertices.emplace_back(Q1.x, Q1.y, Q1.z);
    vertices.emplace_back(Q4.x, Q4.y, Q4.z);
    vertices.emplace_back(Q3.x, Q3.y, Q3.z);
    vertices.emplace_back(Q1.x, Q1.y, Q1.z);

    uvs.emplace_back(Q3.u, Q3.v);
    uvs.emplace_back(Q2.u, Q2.v);
    uvs.emplace_back(Q1.u, Q1.v);
    uvs.emplace_back(Q4.u, Q4.v);
    uvs.emplace_back(Q3.u, Q3.v);
    uvs.emplace_back(Q1.u, Q1.v);

    normals.emplace_back(normal.x, normal.y, normal.z);
    normals.emplace_back(normal.x, normal.y, normal.z);
    normals.emplace_back(normal.x, normal.y, normal.z);
    normals.emplace_back(normal.x, normal.y, normal.z);
    normals.emplace_back(normal.x, normal.y, normal.z);
    normals.emplace_back(normal.x, normal.y, normal.z);

    fillBuffers();
}

void Image3D::onUpdate()
{
    Object3D::onUpdate();

    if (!image->isLoaded()) return;

    /*image->drawFlatAlpha(
            10,
            10,
            255,
            ComponentsManager::get()->getComponentWindow()->getSceneFramebuffer()
    );*/

    ComponentsManager::get()->getComponentWindow()->getShaderOGLRender()->render(
        this,
        image->getOGLTextureID(),
        image->getOGLTextureID(),
        vertexbuffer,
        uvbuffer,
        normalbuffer,
        (int) vertices.size(),
        1.0f,
        ComponentsManager::get()->getComponentWindow()->getSceneFramebuffer()
    );
}

Image3D::~Image3D()
{
}


const char *Image3D::getTypeObject() {
    return "Image2D";
}

const char *Image3D::getTypeIcon() {
    return "image2DIcon";
}

void Image3D::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();
    if (ImGui::TreeNode("Image3D")) {

        const float range_min_int = 1.0;
        const float range_max_int = 1000;

        if (ImGui::TreeNode("Size")) {
            ImGui::DragScalar("Width", ImGuiDataType_Float, &width,1.f, &range_min_int, &range_max_int, "%f", 1.0f);
            ImGui::DragScalar("Height", ImGuiDataType_Float, &height,1.f, &range_min_int, &range_max_int, "%f", 1.0f);

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Image")) {
            if (image->isLoaded()) {
                ImGui::Image((ImTextureID) image->getOGLTextureID(),ImVec2(32, 32));
            } else {
                ImGui::Text("No image selected. Drag a texture here!");
            }

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE_ITEM")) {
                    Logging::Message("Dropping image (%s) in emitter %s", payload->Data, getLabel().c_str());
                    IM_ASSERT(payload->DataSize == sizeof(int));
                    auto selection = (char*) payload->Data;
                    auto fullPath = EngineSetup::get()->IMAGES_FOLDER + selection;
                    if (image == nullptr) {
                        image = new Image(fullPath);
                    } else {
                        image->setImage(fullPath);
                    }
                    Logging::Message("File %s", selection);
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}

cJSON *Image3D::getJSON()
{
    auto root= Object3D::getJSON();

    cJSON_AddNumberToObject(root, "width", width);
    cJSON_AddNumberToObject(root, "height", height);
    cJSON_AddStringToObject(root, "image", image->getFileName().c_str());

    return root;
}

void Image3D::createFromJSON(cJSON *object)
{
    auto name = cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring;
    auto width = (float) cJSON_GetObjectItemCaseSensitive(object, "width")->valueint;
    auto height = (float) cJSON_GetObjectItemCaseSensitive(object, "height")->valueint;
    auto image = cJSON_GetObjectItemCaseSensitive(object, "image")->valuestring;

    auto o = new Image3D(
        Vertex3D(),
        width,
        height,
        new Image(image)
    );

    Image3D::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, name);
}

void Image3D::setPropertiesFromJSON(cJSON *object, Image3D *o)
{
    o->setBelongToScene(true);

    Object3D::setPropertiesFromJSON(object, o);
}

void Image3D::setWidth(float width)
{
    this->width = width;
}

void Image3D::setHeight(float height)
{
    this->height = height;
}

void Image3D::fillBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
}
