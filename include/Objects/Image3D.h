//
// Created by Eduardo on 09/01/2025.
//

#ifndef BRAKEZA3D_IMAGE3D_H
#define BRAKEZA3D_IMAGE3D_H

#include "Object3D.h"
#include "../include/Misc/Image.h"

class Image3D : public Object3D
{
    float width = 0;
    float height = 0;

    Vertex3D Q1;
    Vertex3D Q2;
    Vertex3D Q3;
    Vertex3D Q4;

    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec4> vertices;

    GLuint vertexbuffer = 0;
    GLuint normalbuffer = 0;
    GLuint uvbuffer = 0;
    Image *image;
public:
    Image3D(const Vertex3D &position, float width, float height, Image* image);
    ~Image3D() override;

    void onUpdate() override;
    void drawImGuiProperties() override;
    const char *getTypeObject() override;
    const char *getTypeIcon() override;
    void setSize(float width, float height, const Vertex3D& U, const Vertex3D& R);
    void setWidth(float value);
    void setHeight(float value);
    void fillBuffers();
    void checkClickObject(Vector3D ray, Object3D *&foundObject, float &lastDepthFound) override;
    cJSON *getJSON() override;
    static void createFromJSON(cJSON *object);
    static void setPropertiesFromJSON(cJSON *object, Image3D *o);
    static Image3D* create(Vertex3D p, float w, float h, const std::string &file);
    [[nodiscard]] GLuint getVertexBuffer() const;
    [[nodiscard]] GLuint getNormalBuffer() const;
    [[nodiscard]] GLuint getUVBuffer() const;
    [[nodiscard]] std::vector<glm::vec4> getVertices() const;
    [[nodiscard]] Image* getImage() const;

    void setImage(Image *value);
};


#endif //BRAKEZA3D_IMAGE3D_H
