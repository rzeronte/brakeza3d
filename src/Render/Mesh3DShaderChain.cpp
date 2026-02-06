#include "../../include/Render/Mesh3DShaderChain.h"
#include "../../include/3D/Mesh3D.h"
#include "../../include/OpenGL/Nodes/ShaderNodesMesh3D.h"
#include "../../include/Components/Components.h"
#include <iostream>
#include <GL/glew.h>

void Mesh3DShaderChain::ProcessChain(const Mesh3D* mesh, const std::vector<ShaderBaseCustom*>& shaders, GLuint finalFBO)
{
    if (!isInitialized() || shaders.empty()) return;

    GLuint inputTex = 0;
    bool writeToPing = true;

    // Encontrar último shader habilitado
    int lastEnabled = -1;
    for (int i = static_cast<int>(shaders.size()) - 1; i >= 0; i--) {
        if (shaders[i] && shaders[i]->isEnabled()) {
            lastEnabled = i;
            break;
        }
    }
    if (lastEnabled < 0) return;

    // Limpiar errores OpenGL antes de empezar
    while (glGetError() != GL_NO_ERROR) {}

    for (size_t i = 0; i < shaders.size(); i++) {
        auto shader = shaders[i];
        if (!shader || !shader->isEnabled()) continue;

        bool isLast = (i == static_cast<size_t>(lastEnabled));

        GLuint outFBO = 0;
        GLuint outAlbedoTex = 0;  // La textura de albedo del FBO intermedio

        if (isLast) {
            outFBO = finalFBO;
            outAlbedoTex = 0;
        } else {
            if (writeToPing) {
                outFBO = pingFBO;
                outAlbedoTex = pingAlbedoTex;
            } else {
                outFBO = pongFBO;
                outAlbedoTex = pongAlbedoTex;
            }
        }

        if (outFBO != finalFBO && (outFBO == 0 || outAlbedoTex == 0)) {
            std::cerr << "ERROR: Invalid framebuffer configuration in shader chain!" << std::endl;
            return;
        }

        // Limpiar FBOs intermedios antes de renderizar
        if (!isLast) {
            glBindFramebuffer(GL_FRAMEBUFFER, outFBO);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Manejar shaders de nodos para mesh
        if (auto* nodesShader = dynamic_cast<ShaderNodesMesh3D*>(shader)) {
            auto nodeManager = nodesShader->GetNodeManager();
            if (!nodeManager) {
                std::cerr << "ERROR: nodeManager is null for shader in chain!" << std::endl;
                continue;
            }

            // InternalTexture (Chain Texture):
            // - Primer shader: textura original del mesh como fallback
            // - Shaders subsiguientes: albedo del shader anterior
            GLuint chainInputTex = inputTex;
            if (chainInputTex == 0 && !mesh->getModelTextures().empty() && mesh->getModelTextures()[0]) {
                chainInputTex = mesh->getModelTextures()[0]->getOGLTextureID();
            }

            if (chainInputTex != 0) {
                nodeManager->UpdateChainOutputTexture(chainInputTex);
            }

            // Renderizar cada parte del mesh
            for (size_t meshIdx = 0; meshIdx < mesh->getMeshData().size(); meshIdx++) {
                const auto& m = mesh->getMeshData()[meshIdx];

                // MeshTexture: siempre la textura original del material
                if (mesh->getModelTextures().size() > m.materialIndex && mesh->getModelTextures()[m.materialIndex]) {
                    nodeManager->UpdateMeshTextures(
                        mesh->getModelTextures()[m.materialIndex]->getOGLTextureID(),
                        mesh->getModelTextures()[m.materialIndex]->getOGLTextureID()
                    );
                }

                nodeManager->Update();

                if (nodeManager->GetShaderProgram() == 0) {
                    std::cerr << "ERROR: Shader program is 0 after Update() in chain!" << std::endl;
                    continue;
                }

                nodeManager->RenderMesh(
                    outFBO,
                    m.vertexBuffer,
                    m.uvBuffer,
                    m.normalBuffer,
                    m.vertices.size(),
                    mesh->getModelMatrix(),
                    Components::get()->Camera()->getGLMMat4ViewMatrix(),
                    Components::get()->Camera()->getGLMMat4ProjectionMatrix()
                );
            }
        } else {
            // Shaders normales (no de nodos)
            try {
                shader->Render(outFBO, inputTex);

                GLenum glErr = glGetError();
                if (glErr != GL_NO_ERROR) {
                    std::cerr << "OpenGL error after shader render: " << glErr << std::endl;
                    return;
                }
            } catch (const std::exception& e) {
                std::cerr << "Exception during shader render: " << e.what() << std::endl;
                return;
            }
        }

        if (!isLast) {
            // Usar la textura de ALBEDO (attachment 2) como input para el siguiente shader
            inputTex = outAlbedoTex;
            writeToPing = !writeToPing;
        } else {
            if (mesh) mesh->SetChainTempTexture(inputTex);
            lastOutputTexture = inputTex;
        }
    }
}
