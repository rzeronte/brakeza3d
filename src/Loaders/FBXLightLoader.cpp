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
#include <algorithm>

// ── FBX binary light-colour extractor ────────────────────────────────────────
//
// Assimp's mColorDiffuse for an FBX light comes through as Color × Energy (Blender's raw
// wattage, e.g. a 1000W red light gives (1000,0,0), confirmed empirically 2026-08-20) --
// not a normalized 0-1 hue. This extractor recovers the pure hue directly from the FBX
// binary's own Color property (independent of energy) so LoadLightsFromFile can derive hue
// and brightness separately instead of using this possibly energy-scaled value as a colour.
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

    // Recover raw (energy-independent) light hues from the FBX binary -- see comment above
    // ExtractFBXLightColors for why al->mColorDiffuse alone isn't a usable colour.
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

    // Energía de referencia POR ARCHIVO, no una constante absoluta (2026-08-25):
    // cada FBX exporta la "Intensity" de sus luces en la escala que le dio Blender al
    // hornear ese archivo en concreto -- HALL.fbx mide ~20-110, pero LIGHTS.fbx (farolas
    // de ciudad) mide ~1.3-2.9, dos órdenes de magnitud por debajo. Con un REFERENCE_ENERGY
    // fijo calibrado para uno, el otro cae entero al suelo de brillo (comprobado: 42% de las
    // luces de LIGHTS.fbx pegadas al mínimo 0.1). Usamos la mediana de energía de ESTE
    // archivo como referencia: una luz "típica" del archivo sale a brillo ~1.0 sin importar
    // en qué unidades exportó Blender, y las luces relativamente más/menos potentes dentro
    // del mismo archivo se siguen diferenciando entre sí.
    std::vector<float> fileEnergies;
    fileEnergies.reserve(scene->mNumLights);
    for (unsigned int i = 0; i < scene->mNumLights; i++) {
        const aiColor3D& c = scene->mLights[i]->mColorDiffuse;
        float e = std::max({c.r, c.g, c.b});
        if (e > 0.001f) fileEnergies.push_back(e);
    }
    float fileReferenceEnergy = 100.0f;
    if (!fileEnergies.empty()) {
        std::nth_element(fileEnergies.begin(), fileEnergies.begin() + fileEnergies.size() / 2, fileEnergies.end());
        fileReferenceEnergy = fileEnergies[fileEnergies.size() / 2];
    }

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

        // Assimp sets mColorDiffuse = Color × Energy (Blender's raw wattage, NOT normalized) --
        // confirmed empirically (a 1000W red Blender light comes through as (1000,0,0)). The pure
        // hue (0-1, independent of energy) comes from the raw FBX binary Color property when
        // available (ExtractFBXLightColors), since Assimp's own colour can be black for some
        // Blender/export combinations. Brightness is derived separately below from whichever
        // colour source's magnitude reflects the light's actual power.
        // NOTA: al->mColorSpecular viene, en la práctica, IDÉNTICO a al->mColorDiffuse (mismo
        // valor sin escalar por energía -- Blender no exporta un specular independiente para
        // point/spot lights) -- por eso specular se deriva SIEMPRE de `diffuse` ya escalado más
        // abajo, nunca de al->mColorSpecular directo (eso colaba el color crudo sin normalizar,
        // p.ej. (1000,0,0), directo al shader -- origen real del "demasiado intensas").
        auto toVec4 = [](const aiColor3D& c) { return glm::vec4(c.r, c.g, c.b, 1.0f); };
        glm::vec4 assimpDiffuse = toVec4(al->mColorDiffuse);
        glm::vec4 ambient       = toVec4(al->mColorAmbient);

        float rawMaxC = std::max({assimpDiffuse.r, assimpDiffuse.g, assimpDiffuse.b});

        glm::vec3 hue;
        if (i < rawColors.size()) {
            hue = rawColors[i];
        } else {
            hue = (rawMaxC > 0.001f) ? glm::vec3(assimpDiffuse) / rawMaxC : glm::vec3(1.0f);
        }

        // Brillo relativo, curva raíz cuadrada en vez de lineal (2026-08-20, recalibrado con
        // datos reales): luces de interior en producción (HALL.fbx) miden ~20-110W de energía,
        // MUY por debajo de las luces de prueba (500-1000W) usadas para calibrar el mapeo lineal
        // original -- con esa escala TODAS las luces de HALL caían por debajo del suelo mínimo y
        // se planchaban al mismo brillo (el "casi grises" reportado: sin distinción entre ellas).
        // sqrt comprime el extremo alto (1000W no queda 10x más brillante que 100W, solo ~3x) y
        // expande el extremo bajo (20W y 110W siguen siendo visualmente distintos entre sí, no
        // ambos aplastados contra un suelo plano) -- más estable en un rango amplio de energías
        // sin perder la diferencia relativa entre luces cercanas en potencia.
        // fileReferenceEnergy (mediana de energía de ESTE archivo) reemplaza la constante fija --
        // ver comentario junto a su cálculo, arriba del bucle.
        constexpr float MIN_BRIGHTNESS = 0.1f;
        constexpr float MAX_BRIGHTNESS = 3.0f;
        float brightness = (rawMaxC > 0.001f)
            ? std::clamp(std::sqrt(rawMaxC / fileReferenceEnergy), MIN_BRIGHTNESS, MAX_BRIGHTNESS)
            : 1.0f;

        glm::vec4 diffuse  = glm::vec4(hue * brightness, 1.0f);
        glm::vec4 specular = diffuse;
        if (ambient.r  < 0.01f && ambient.g  < 0.01f && ambient.b  < 0.01f)
            ambient = glm::vec4(diffuse.r * 0.05f, diffuse.g * 0.05f, diffuse.b * 0.05f, 1.0f);

        // Atenuación: SIEMPRE la tabla estable por defecto del motor (LightPointSerializer.cpp),
        // no los coeficientes crudos del FBX -- el modelo de decaimiento de Blender/FBX no tiene
        // término constante (constant=0), lo que dispara la atenuación casi a infinito muy cerca
        // del foco (1/(0+0+quad*d²) en RenderCommons.glsl) y no tiene nada que ver con la escala
        // que espera el motor. El brillo relativo ya lo aporta `brightness` de arriba.
        float attConst = 1.0f;
        float attLin   = 0.09f;
        float attQuad  = 0.032f;

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
                // al->mAngleInnerCone/mAngleOuterCone son el ángulo COMPLETO del cono (así lo
                // exporta Blender vía spot_size/spot_blend -- verificado: outer coincide exacto
                // con spot_size sin dividir). El shader (RenderCommons.glsl) compara cutOff contra
                // dot(lightDir, -direction), que da el coseno del ÁNGULO DESDE EL EJE (medio-ángulo)
                // -- los valores por defecto del motor para un LightSpot manual (LightSpotSerializer)
                // ya son cosenos de medio-ángulo (0.9763/0.9659 = 12.5°/15°). Sin dividir entre 2 el
                // cono renderizado salía con el doble de ancho del definido en Blender.
                float cutOff      = std::cos(al->mAngleInnerCone * 0.5f);
                float outerCutOff = std::cos(al->mAngleOuterCone * 0.5f);

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
