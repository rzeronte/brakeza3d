#define GL_GLEXT_PROTOTYPES

#include "../../include/Render/Billboard.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"

Billboard::Billboard(float width, float height)
:
    width(width),
    height(height),
    texture(nullptr)
{
    glGenBuffers(1, &vertexbuffer);
    glGenBuffers(1, &uvbuffer);
    glGenBuffers(1, &normalbuffer);
}

void Billboard::updateUnconstrainedQuad(Object3D *o, Vertex3D &U, Vertex3D &R)
{
    Vertex3D X;
    X.x = (width / 2) * R.x;
    X.y = (width / 2) * R.y;
    X.z = (width / 2) * R.z;

    Vertex3D Y;
    Y.x = (height / 2) * U.x;
    Y.y = (height / 2) * U.y;
    Y.z = (height / 2) * U.z;

    Q1.x = o->getPosition().x + X.x + Y.x;
    Q1.y = o->getPosition().y + X.y + Y.y;
    Q1.z = o->getPosition().z + X.z + Y.z;

    Q2.x = o->getPosition().x - X.x + Y.x;
    Q2.y = o->getPosition().y - X.y + Y.y;
    Q2.z = o->getPosition().z - X.z + Y.z;

    Q3.x = o->getPosition().x - X.x - Y.x;
    Q3.y = o->getPosition().y - X.y - Y.y;
    Q3.z = o->getPosition().z - X.z - Y.z;

    Q4.x = o->getPosition().x + X.x - Y.x;
    Q4.y = o->getPosition().y + X.y - Y.y;
    Q4.z = o->getPosition().z + X.z - Y.z;

    Q1 = Transforms::objectToLocal(Q1, o);
    Q2 = Transforms::objectToLocal(Q2, o);
    Q3 = Transforms::objectToLocal(Q3, o);
    Q4 = Transforms::objectToLocal(Q4, o);

    Q1.u = 1.0f;
    Q1.v = 1.0f;
    Q2.u = 0.0001;
    Q2.v = 1.0f;
    Q3.u = 0.0001;
    Q3.v = 0.0001;
    Q4.u = 1.0f;
    Q4.v = 0.0001;

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

void Billboard::loadTexture(const std::string &fileName)
{
    texture->loadTGA(fileName);
}

void Billboard::fillBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
}

void Billboard::setTexture(Image *texture) {
    Billboard::texture = texture;
}

void Billboard::updateSize(float w, float h)
{
    width = w;
    height = h;
}
