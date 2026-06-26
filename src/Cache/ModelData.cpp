#include "../../include/Cache/ModelData.h"
#include "../../include/3D/Mesh3D.h"
#include "../../include/Brakeza.h"
#include "../../include/Cache/ImageCache.h"
#include "../../include/Misc/Logging.h"

void ModelData::cloneInto(Mesh3D& target) const
{
    LOG_MESSAGE("[ModelData] Cloning '%s' into Mesh3D '%s'", sourceFile.c_str(), target.getName().c_str());

    target.getMeshData().resize(meshes.size());

    for (size_t i = 0; i < meshes.size(); i++) {
        const auto& src = meshes[i];
        auto& dst = target.getMeshData()[i];

        dst.vertices = src.vertices;
        dst.uvs = src.uvs;
        dst.normals = src.normals;
        dst.materialIndex = src.materialIndex;
        dst.name = src.name;

        for (size_t f = 0; f < src.triangleVertices.size() / 3; f++) {
            const Vertex3D& V1 = src.triangleVertices[f * 3 + 0];
            const Vertex3D& V2 = src.triangleVertices[f * 3 + 1];
            const Vertex3D& V3 = src.triangleVertices[f * 3 + 2];
            dst.modelTriangles.push_back(new Triangle(V3, V2, V1, &target));
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

    target.setSourceFile(sourceFile);

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

    target.setSharedTextures(true);
}
