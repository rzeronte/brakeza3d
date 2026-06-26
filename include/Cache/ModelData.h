#ifndef BRAKEZA3D_MODELDATA_H
#define BRAKEZA3D_MODELDATA_H

#include <vector>
#include <string>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "../3D/Vertex3D.h"
#include "../Misc/FilePaths.h"

class Mesh3D;

struct MeshEntryData {
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<Vertex3D> triangleVertices;
    int materialIndex = 0;
    std::string name;
};

struct MaterialEntryData {
    bool hasDiffuseTexture = false;
    std::string diffuseTexturePath;
    bool hasSpecularTexture = false;
    std::string specularTexturePath;
};

struct ModelData {
    std::vector<MeshEntryData> meshes;
    std::vector<MaterialEntryData> materials;
    std::string sourceFile;

    void cloneInto(Mesh3D& target) const;
};

#endif
