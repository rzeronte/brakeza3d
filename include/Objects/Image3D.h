//
// Created by Eduardo on 09/01/2025.
//

#ifndef BRAKEZA3D_IMAGE3D_H
#define BRAKEZA3D_IMAGE3D_H

#include "../Render/Billboard.h"

class Image3D : public Object3D {
    float width;
    float height;
    Image *image;

    Vertex3D Q1;
    Vertex3D Q2;
    Vertex3D Q3;
    Vertex3D Q4;

    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    std::vector<glm::vec3> vertices;

    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
public:
    Image3D(Vertex3D position, float width, float height, Image* image);

    void onUpdate() override;

    const char *getTypeObject() override;
    const char *getTypeIcon() override;
    void drawImGuiProperties() override;
    cJSON *getJSON() override;
    static void createFromJSON(cJSON *object);
    static void setPropertiesFromJSON(cJSON *object, Image3D *o);

    ~Image3D() override;

    void setSize(float width, float height, Vertex3D U, Vertex3D  R);

    void setWidth(float width);

    void setHeight(float height);

    void fillBuffers();

    static Image3D* create(Vertex3D p, float w, float h, const std::string &file);
};


#endif //BRAKEZA3D_IMAGE3D_H
