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
    Image *image;
public:
    Image3D(const Vertex3D &position, float width, float height, Image* image);
    ~Image3D() override;

    ObjectTypes getTypeObject() const override;
    GUISheet getIcon() override;
    void onUpdate() override;
    void DrawPropertiesGUI() override;
    void setSize(float width, float height);
    void setWidth(float value);
    void setHeight(float value);
    void fillBuffers();
    void setImage(Image *value);

    void shadowMappingPass();

    void lookAtBillboard(Object3D *o);

    [[nodiscard]] GLuint getVertexBuffer() const;
    [[nodiscard]] GLuint getNormalBuffer() const;
    [[nodiscard]] GLuint getUVBuffer() const;
    [[nodiscard]] std::vector<glm::vec4> getVertices() const;
    [[nodiscard]] Image* getImage() const;
    static Image3D* create(const Vertex3D &p, float w, float h, const std::string &file);

    friend class Image3DSerializer;
    friend class Image3DGUI;
};


#endif //BRAKEZA3D_IMAGE3D_H
