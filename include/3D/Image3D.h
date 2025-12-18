//
// Created by Eduardo on 09/01/2025.
//

#ifndef BRAKEZA3D_IMAGE3D_H
#define BRAKEZA3D_IMAGE3D_H

#include "Object3D.h"
#include "../Render/Image.h"

class Image3DGUI;

class Image3D : public Object3D
{
    float width = 0;
    float height = 0;

    bool towardsCamera = false;
    bool backFaceCulling = true;

    std::string source;

    Vertex3D Q1;
    Vertex3D Q2;
    Vertex3D Q3;
    Vertex3D Q4;

    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec4> vertices;

    GLuint vertexBuffer = 0;
    GLuint normalBuffer = 0;
    GLuint uvBuffer = 0;
    Image *image = nullptr;
public:
    Image3D(const Vertex3D &position, float width, float height, const std::string &filePath);

    Image3D(const Vertex3D &position, float width, float height, Image* image);
    ~Image3D() override;

    void onUpdate() override;
    void DrawPropertiesGUI() override;
    void ResetBuffersToSize(float width, float height);
    void setWidth(float value);

    void setSource(const std::string &source);

    void setHeight(float value);
    void FillBuffers();
    void setImage(Image *value);
    void ShadowMappingPass();
    void LookAtBillboard();

    ObjectType getTypeObject() const override                   { return ObjectType::Image3D;}
    GUIType::Sheet getIcon() override                           { return IconObject::IMAGE_3D;}
    [[nodiscard]] GLuint getVertexBuffer() const                { return vertexBuffer; }
    [[nodiscard]] GLuint getNormalBuffer() const                { return normalBuffer; }
    [[nodiscard]] GLuint getUVBuffer() const                    { return uvBuffer; }
    [[nodiscard]] std::vector<glm::vec4> getVertices() const    { return vertices; }
    [[nodiscard]] Image* getImage() const                       { return image; }

    friend class Image3DSerializer;
    friend class Image3DGUI;
};


#endif //BRAKEZA3D_IMAGE3D_H
