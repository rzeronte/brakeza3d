
#ifndef SDL2_3D_ENGINE_BILLBOARD_H
#define SDL2_3D_ENGINE_BILLBOARD_H

#include "../Objects/Object3D.h"
#include "../Objects/Triangle3D.h"

class Billboard {
private:
    float width;
    float height;

    Triangle T1;
    Triangle T2;

    Vertex3D Q1;
    Vertex3D Q2;
    Vertex3D Q3;
    Vertex3D Q4;

    Image *texture;
    std::vector<Triangle *> triangles;

    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
public:
    Billboard(float width, float height);

    void updateUnconstrainedQuad(Object3D *o, Vertex3D &up, Vertex3D &right);

    void loadTexture(const std::string &name);

    void setTrianglesTexture(Image *);

    void reassignTexture();

    Triangle *getT1();

    Triangle *getT2();

    std::vector<Triangle *> &getTriangles();

    void fillBuffers();

    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint normalbuffer;
    std::vector<glm::vec3> vertices;
};


#endif //SDL2_3D_ENGINE_BILLBOARD_H
