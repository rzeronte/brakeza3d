//
// Created by eduardo on 10/10/23.
//

#ifndef BRAKEZA3D_SCENELOADER_H
#define BRAKEZA3D_SCENELOADER_H

#include <string>
#include <map>
#include "../Objects/Vertex3D.h"
#include "cJSON.h"

enum class SceneObjectLoaderMapping {
    Object3D,
    Mesh3D,
    LightPoint3D,
    Mesh3DGhost,
    Mesh3DBody
};

enum class Mesh3DShaderLoaderMapping {
    ShaderEdgeObject,
};

enum class SceneShaderLoaderMapping {
    ShaderImage,
    ShaderDepthOfField,
    ShaderBilinear,
    ShaderParticlesBlurBuffer
};

class SceneLoader {
    std::map<std::string, SceneObjectLoaderMapping> sceneTypes = {
            {"Object3D", SceneObjectLoaderMapping::Object3D},
            {"Mesh3D", SceneObjectLoaderMapping::Mesh3D},
            {"LightPoint3D", SceneObjectLoaderMapping::LightPoint3D},
            {"Mesh3DGhost", SceneObjectLoaderMapping::Mesh3DGhost},
            {"Mesh3DBody", SceneObjectLoaderMapping::Mesh3DBody}
    };

    std::map<std::string, Mesh3DShaderLoaderMapping> mesh3DShaderTypes = {
            {"ShaderEdgeObject", Mesh3DShaderLoaderMapping::ShaderEdgeObject}
    };

    std::map<std::string, SceneShaderLoaderMapping> sceneShaderTypes = {
            {"ShaderEdgeObject", SceneShaderLoaderMapping::ShaderImage},
            {"ShaderDepthOfField", SceneShaderLoaderMapping::ShaderDepthOfField},
            {"ShaderBilinear", SceneShaderLoaderMapping::ShaderBilinear},
            {"ShaderParticlesBlurBuffer", SceneShaderLoaderMapping::ShaderParticlesBlurBuffer}
    };
public:
    explicit SceneLoader();
    void loadScene(const std::string& filename);
    static void saveScene(const std::string& filename);

    static Vertex3D parseVertex3DJSON(cJSON *vertex3DJSON);

    static void clearScene();

    static void createObjectInScene();

    static void createLightPointInScene();

    static void createMesh3DBodyToScene(const std::string &filename, const char *name);

    static void createGhostBody3DToScene(const std::string &filename, const char *name);

    std::map<std::string, SceneObjectLoaderMapping> &getSceneTypes();

    std::map<std::string, Mesh3DShaderLoaderMapping> &getMesh3DShaderTypes();

    std::map<std::string, SceneShaderLoaderMapping> &getSceneShaderTypes();
};


#endif //BRAKEZA3D_SCENELOADER_H
