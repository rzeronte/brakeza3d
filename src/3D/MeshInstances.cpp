#include "../../include/3D/MeshInstances.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../../include/Misc/Logging.h"
#include "../../include/Render/AABB3D.h"
#include "../../include/Render/Frustum.h"

namespace {
    constexpr char    MAGIC[4]         = {'B', 'Z', 'I', 'N'};
    constexpr uint32_t VERSION         = 1;
    constexpr uint32_t MAX_CHUNKS      = 1u << 16;
    constexpr uint32_t MAX_INSTANCES   = 1u << 24;
}

bool MeshInstances::loadFromFile(const std::string &path)
{
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        LOG_ERROR("[MeshInstances] cannot open '%s'", path.c_str());
        return false;
    }

    char magic[4];
    uint32_t version = 0, chunkCount = 0, instanceCount = 0;
    in.read(magic, 4);
    in.read(reinterpret_cast<char *>(&version), 4);
    in.read(reinterpret_cast<char *>(&chunkCount), 4);
    in.read(reinterpret_cast<char *>(&instanceCount), 4);
    if (!in || std::string(magic, 4) != std::string(MAGIC, 4) || version != VERSION ||
        chunkCount > MAX_CHUNKS || instanceCount > MAX_INSTANCES) {
        LOG_ERROR("[MeshInstances] '%s': bad header (magic/version/counts)", path.c_str());
        return false;
    }

    std::vector<Chunk> newChunks(chunkCount);
    for (auto &c : newChunks) {
        float v[6];
        in.read(reinterpret_cast<char *>(v), sizeof(v));
        in.read(reinterpret_cast<char *>(&c.first), 4);
        in.read(reinterpret_cast<char *>(&c.count), 4);
        c.min = Vertex3D(v[0], v[1], v[2]);
        c.max = Vertex3D(v[3], v[4], v[5]);
    }

    std::vector<glm::mat4> newMatrices(instanceCount);
    for (auto &m : newMatrices) {
        float f[10];
        in.read(reinterpret_cast<char *>(f), sizeof(f));
        const glm::quat q(f[6], f[3], f[4], f[5]);   // glm::quat(w, x, y, z); el fichero guarda x, y, z, w
        m = glm::translate(glm::mat4(1.0f), glm::vec3(f[0], f[1], f[2]))
          * glm::mat4_cast(q)
          * glm::scale(glm::mat4(1.0f), glm::vec3(f[7], f[8], f[9]));
    }
    if (!in) {
        LOG_ERROR("[MeshInstances] '%s': truncated file", path.c_str());
        return false;
    }

    // Validar que los chunks cubren instancias existentes
    float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;
    for (const auto &c : newChunks) {
        if (static_cast<uint64_t>(c.first) + c.count > instanceCount) {
            LOG_ERROR("[MeshInstances] '%s': chunk out of range", path.c_str());
            return false;
        }
        minX = std::min(minX, c.min.x); minY = std::min(minY, c.min.y); minZ = std::min(minZ, c.min.z);
        maxX = std::max(maxX, c.max.x); maxY = std::max(maxY, c.max.y); maxZ = std::max(maxZ, c.max.z);
    }

    chunks   = std::move(newChunks);
    matrices = std::move(newMatrices);
    visible.clear();
    visible.reserve(matrices.size());
    visibleChunks = 0;
    if (!chunks.empty()) {
        localMin = Vertex3D(minX, minY, minZ);
        localMax = Vertex3D(maxX, maxY, maxZ);
    }
    LOG_MESSAGE("[MeshInstances] '%s': %u instances in %u chunks", path.c_str(), instanceCount, chunkCount);
    return true;
}

void MeshInstances::cull(const glm::mat4 &model, const Vertex3D &cameraPos, float maxDistance)
{
    visible.clear();
    visibleChunks = 0;

    for (const auto &c : chunks) {
        if (c.count == 0) continue;

        AABB3D box;
        int k = 0;
        float cx = 0.0f, cy = 0.0f, cz = 0.0f;
        for (int ix = 0; ix < 2; ix++) {
            for (int iy = 0; iy < 2; iy++) {
                for (int iz = 0; iz < 2; iz++) {
                    const glm::vec4 p = model * glm::vec4(ix ? c.max.x : c.min.x, iy ? c.max.y : c.min.y, iz ? c.max.z : c.min.z, 1.0f);
                    box.vertices[k] = Vertex3D(p.x / p.w, p.y / p.w, p.z / p.w);
                    cx += box.vertices[k].x; cy += box.vertices[k].y; cz += box.vertices[k].z;
                    k++;
                }
            }
        }

        if (!Frustum::isAABBVisibleInFrustum(&box)) continue;

        if (maxDistance > 0.0f) {
            cx *= 0.125f; cy *= 0.125f; cz *= 0.125f;
            const float dx = box.vertices[7].x - cx, dy = box.vertices[7].y - cy, dz = box.vertices[7].z - cz;
            const float halfDiag = std::sqrt(dx * dx + dy * dy + dz * dz);
            const float ex = cx - cameraPos.x, ey = cy - cameraPos.y, ez = cz - cameraPos.z;
            if (std::sqrt(ex * ex + ey * ey + ez * ez) - halfDiag > maxDistance) continue;
        }

        visibleChunks++;
        for (uint32_t i = c.first; i < c.first + c.count; i++) visible.push_back(model * matrices[i]);
    }
}
