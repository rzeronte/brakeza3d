#include "../../include/Cache/AnimationData.h"
#include "../../include/3D/Mesh3DAnimation.h"
#include "../../include/Cache/ImageCache.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/Logging.h"

void AnimationData::cloneInto(Mesh3DAnimation& target) const
{
    LOG_MESSAGE("[AnimationData] Cloning '%s' into Mesh3DAnimation '%s'",
        sourceFile.c_str(), target.getName().c_str());

    auto& targetMeshes = target.getMeshData();
    targetMeshes.resize(meshes.size());

    for (size_t i = 0; i < meshes.size(); i++) {
        const auto& src = meshes[i];
        auto& dst = targetMeshes[i];

        dst.vertices = src.vertices;
        dst.uvs = src.uvs;
        dst.normals = src.normals;
        dst.materialIndex = src.materialIndex;
        dst.name = src.name;

        for (size_t f = 0; f < src.triangleVertices.size() / 3; f++) {
            const Vertex3D& V1 = src.triangleVertices[f * 3 + 0];
            const Vertex3D& V2 = src.triangleVertices[f * 3 + 1];
            const Vertex3D& V3 = src.triangleVertices[f * 3 + 2];
            dst.modelTriangles.push_back(new Triangle(V1, V2, V3, &target));
        }

        unsigned int sid = Brakeza::getNextUniqueObjectId();
        dst.submeshPickingId = sid;
        dst.submeshPickingColor = Color::idToColor(sid);

        float maxX = -FLT_MAX, minX = FLT_MAX, maxY = -FLT_MAX, minY = FLT_MAX, maxZ = -FLT_MAX, minZ = FLT_MAX;
        for (const auto& v : dst.vertices) {
            maxX = std::max(maxX, v.x); minX = std::min(minX, v.x);
            maxY = std::max(maxY, v.y); minY = std::min(minY, v.y);
            maxZ = std::max(maxZ, v.z); minZ = std::min(minZ, v.z);
        }
        dst.localAabb.max = Vertex3D(maxX, maxY, maxZ);
        dst.localAabb.min = Vertex3D(minX, minY, minZ);
        dst.localAabb.updateVertices();

        dst.visibleInFrustum = true;
    }

    for (const auto& mat : materials) {
        if (mat.hasDiffuseTexture) {
            Image* img = imageCache.getOrLoad(mat.diffuseTexturePath);
            target.getModelTextures().push_back(img);
        } else {
            target.getModelTextures().push_back(new Image((GLuint)0, 1, 1));
        }
        if (mat.hasSpecularTexture) {
            Image* img = imageCache.getOrLoad(mat.specularTexturePath);
            target.getModelSpecularTextures().push_back(img);
        } else {
            target.getModelSpecularTextures().push_back(new Image((GLuint)0, 1, 1));
        }
    }

    target.setSourceFile(sourceFile);
    target.setSharedTextures(true);
}
