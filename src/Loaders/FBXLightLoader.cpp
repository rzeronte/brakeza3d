#include "../../include/Loaders/FBXLightLoader.h"
#include "../../include/3D/LightPoint.h"
#include "../../include/3D/LightSpot.h"
#include "../../include/3D/Vertex3D.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/Logging.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <fstream>
#include <cstring>
#include <vector>

// ── FBX binary light-colour extractor ────────────────────────────────────────
//
// Assimp computes mColorDiffuse = Color × Intensity/100.
// Blender exports Intensity ≈ 0, so every colour appears black.
//
// Fix: scan the raw FBX binary for P-records with name="Color" and type="Color"
// (unique to light NodeAttributes — materials use "DiffuseColor", etc.)
// starting AFTER the Definitions section (which holds the white template default).
// The records inside Objects appear in the same order Assimp produces lights,
// so rawColors[i] maps directly to scene->mLights[i].

static std::vector<glm::vec3> ExtractFBXLightColors(const std::string& path)
{
    std::vector<glm::vec3> colors;

    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) return colors;
    const auto sz = static_cast<size_t>(f.tellg());
    f.seekg(0);

    char magic[21] = {};
    f.read(magic, 21);
    if (std::strncmp(magic, "Kaydara FBX Binary  ", 20) != 0) return colors;

    std::vector<uint8_t> data(sz);
    f.seekg(0);
    f.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(sz));
    f.close();

    // Skip the Definitions section by finding the "Objects" block name tag.
    // In FBX binary a record name is preceded by a 1-byte length, so the tag
    // for "Objects" (7 chars) is: 0x07 'O' 'b' 'j' 'e' 'c' 't' 's'
    const uint8_t objTag[] = {7,'O','b','j','e','c','t','s'};
    auto it = std::search(data.begin() + 27, data.end(), objTag, objTag + 8);
    const size_t scanFrom = (it != data.end()) ? static_cast<size_t>(it - data.begin()) : 27;

    // Binary pattern for a P-record where name="Color" AND type="Color":
    //   'S' uint32_le(5) "Color"  'S' uint32_le(5) "Color"
    const uint8_t sig[] = {
        'S',5,0,0,0,'C','o','l','o','r',
        'S',5,0,0,0,'C','o','l','o','r'
    };
    constexpr size_t sigLen = sizeof(sig);

    for (size_t i = scanFrom; i + sigLen + 32 < sz; ++i) {
        if (std::memcmp(&data[i], sig, sigLen) != 0) continue;

        size_t p = i + sigLen;

        auto skipS = [&]() -> bool {
            if (p + 5 > sz || data[p] != 'S') return false;
            uint32_t len = 0;
            std::memcpy(&len, &data[p + 1], 4);
            p += 5 + len;
            return p <= sz && len < 65536;
        };
        auto readD = [&](double& v) -> bool {
            if (p + 9 > sz || data[p] != 'D') return false;
            std::memcpy(&v, &data[p + 1], 8);
            p += 9;
            return true;
        };

        if (!skipS() || !skipS()) continue;   // label, flags
        double r = 0, g = 0, b = 0;
        if (!readD(r) || !readD(g) || !readD(b)) continue;

        colors.emplace_back(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b));
    }

    return colors;
}

static aiNode* FindNodeByName(aiNode* node, const aiString& name)
{
    if (node->mName == name) return node;
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        aiNode* found = FindNodeByName(node->mChildren[i], name);
        if (found) return found;
    }
    return nullptr;
}

static aiMatrix4x4 GetWorldTransform(const aiNode* node)
{
    if (!node->mParent) return node->mTransformation;
    return GetWorldTransform(node->mParent) * node->mTransformation;
}

// ── Main loader ───────────────────────────────────────────────────────────────

std::vector<Object3D*> FBXLightLoader::LoadLightsFromFile(
    const FilePath::ModelFile &fileName,
    float posX, float posY, float posZ,
    float rotX, float rotY, float rotZ,
    float scale,
    bool enabledByDefault
)
{
    std::vector<Object3D*> lights;

    if (!Tools::FileExists(fileName.c_str())) {
        LOG_ERROR("[FBXLightLoader] File not found: %s", fileName.c_str());
        return lights;
    }

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene) {
        LOG_ERROR("[FBXLightLoader] Failed to load '%s': %s", fileName.c_str(), importer.GetErrorString());
        return lights;
    }

    if (!scene->HasLights()) {
        LOG_MESSAGE("[FBXLightLoader] No lights in file: %s", fileName.c_str());
        return lights;
    }

    LOG_MESSAGE("[FBXLightLoader] Found %d lights in '%s'", scene->mNumLights, fileName.c_str());

    // Recover raw light colours from FBX binary (Blender exports Intensity≈0,
    // making Assimp's mColorDiffuse = Color×0 = black).
    auto rawColors = ExtractFBXLightColors(fileName.c_str());
    LOG_MESSAGE("[FBXLightLoader] FBX binary: %zu light colors recovered", rawColors.size());

    // Build the same model matrix the scene JSON would apply to a mesh at this transform
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(posX, posY, posZ));
    if (rotX != 0.f) modelMatrix = glm::rotate(modelMatrix, glm::radians(rotX), glm::vec3(1.f, 0.f, 0.f));
    if (rotY != 0.f) modelMatrix = glm::rotate(modelMatrix, glm::radians(rotY), glm::vec3(0.f, 1.f, 0.f));
    if (rotZ != 0.f) modelMatrix = glm::rotate(modelMatrix, glm::radians(rotZ), glm::vec3(0.f, 0.f, 1.f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));

    // Cancel out the FBX root-node transform (unit scale, axis convention, etc.)
    // so light positions match mesh vertices loaded by Assimp for the same file.
    aiMatrix4x4 globalInverse = scene->mRootNode->mTransformation;
    globalInverse.Inverse();

    for (unsigned int i = 0; i < scene->mNumLights; i++) {
        const aiLight *al = scene->mLights[i];
        std::string name = al->mName.C_Str();

        // Get world position from node hierarchy (al->mPosition is in local node space)
        aiVector3D localPos = al->mPosition;
        aiNode* lightNode = FindNodeByName(scene->mRootNode, al->mName);
        if (lightNode) {
            aiMatrix4x4 worldTf = GetWorldTransform(lightNode);
            // Apply globalInverse to cancel the FBX root transform (same as mesh pipeline)
            localPos = globalInverse * worldTf * localPos;
        }

        // Apply the scene-level transform (same as the city mesh)
        glm::vec4 enginePos = modelMatrix * glm::vec4(localPos.x, localPos.y, localPos.z, 1.0f);

        // Assimp sets mColorDiffuse = Color × Intensity/100.
        // Blender may export Intensity near-zero, making mColorDiffuse very dark or black.
        // Always prefer the raw FBX binary colour when available.
        auto toVec4 = [](const aiColor3D& c) { return glm::vec4(c.r, c.g, c.b, 1.0f); };
        glm::vec4 diffuse  = toVec4(al->mColorDiffuse);
        glm::vec4 ambient  = toVec4(al->mColorAmbient);
        glm::vec4 specular = toVec4(al->mColorSpecular);

        if (i < rawColors.size()) {
            // Raw FBX color — not affected by Assimp's Color×Intensity/100 issue
            diffuse = glm::vec4(rawColors[i], 1.0f);
        } else {
            // No raw color available: normalize Assimp's value (removes Intensity scaling)
            // or fall back to white if truly black.
            float maxC = std::max({diffuse.r, diffuse.g, diffuse.b});
            if (maxC < 0.001f)
                diffuse = glm::vec4(1.0f);
            else if (maxC < 1.0f)
                diffuse = glm::vec4(diffuse.r / maxC, diffuse.g / maxC, diffuse.b / maxC, 1.0f);
        }
        if (ambient.r  < 0.01f && ambient.g  < 0.01f && ambient.b  < 0.01f)
            ambient = glm::vec4(diffuse.r * 0.05f, diffuse.g * 0.05f, diffuse.b * 0.05f, 1.0f);
        if (specular.r < 0.01f && specular.g < 0.01f && specular.b < 0.01f)
            specular = diffuse;

        // Guard against all-zero attenuation (would cause div-by-zero in shader).
        float attConst = al->mAttenuationConstant;
        float attLin   = al->mAttenuationLinear;
        float attQuad  = al->mAttenuationQuadratic;
        if (attConst == 0.f && attLin == 0.f && attQuad == 0.f)
            attConst = 1.0f;

        Object3D *light = nullptr;

        switch (al->mType) {
            case aiLightSource_POINT: {
                light = new LightPoint(
                    ambient, diffuse, specular,
                    attConst, attLin, attQuad
                );
                light->setPosition(Vertex3D(enginePos.x, enginePos.y, enginePos.z));
                break;
            }
            case aiLightSource_SPOT: {
                float cutOff      = std::cos(al->mAngleInnerCone);
                float outerCutOff = std::cos(al->mAngleOuterCone);

                auto *spot = new LightSpot(
                    ambient, diffuse, specular,
                    attConst, attLin, attQuad,
                    cutOff,
                    outerCutOff
                );
                spot->setPosition(Vertex3D(enginePos.x, enginePos.y, enginePos.z));

                // Transform direction vector (no translation, no scale)
                if (lightNode) {
                    aiMatrix4x4 worldTf = GetWorldTransform(lightNode);
                    aiVector3D dir = -(worldTf * al->mDirection - worldTf * aiVector3D(0,0,0));
                    if (dir.Length() > 0.001f) {
                        dir.Normalize();
                        glm::vec4 engDir = modelMatrix * glm::vec4(dir.x, dir.y, dir.z, 0.0f);
                        Vertex3D target(enginePos.x + engDir.x, enginePos.y + engDir.y, enginePos.z + engDir.z);
                        spot->LookAt(target);
                    }
                }

                light = spot;
                break;
            }
            case aiLightSource_DIRECTIONAL:
                LOG_MESSAGE("[FBXLightLoader] Skipping directional light '%s'", name.c_str());
                continue;
            default:
                LOG_MESSAGE("[FBXLightLoader] Skipping unknown light type for '%s'", name.c_str());
                continue;
        }

        if (light) {
            light->setName(name);
            light->setEnabled(enabledByDefault);
            lights.push_back(light);
        }
    }

    return lights;
}
