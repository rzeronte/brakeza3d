#ifndef BRAKEZA3D_ANIMATIONDATA_H
#define BRAKEZA3D_ANIMATIONDATA_H

#include <vector>
#include <string>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "../3D/Vertex3D.h"
#include "../Misc/FilePaths.h"
#include "ModelData.h"

class Mesh3DAnimation;

struct AnimationMeshEntry {
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<Vertex3D> triangleVertices;
    int materialIndex = 0;
    std::string name;
};

struct AnimationData {
    std::vector<AnimationMeshEntry> meshes;
    std::vector<MaterialEntryData> materials;
    std::string sourceFile;

    void cloneInto(Mesh3DAnimation& target) const;
};

#endif
