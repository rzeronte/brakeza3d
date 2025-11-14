#define GL_GLEXT_PROTOTYPES

#include <glm/gtc/type_ptr.hpp>
#include "../../include/OpenGL/ShaderOpenGLRender.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ShaderOpenGLRender::ShaderOpenGLRender()
    :
    bufferUBOSpotLights(0),
    bufferUBOLightPoints(0),
    directionalLight(DirLightOpenGL{
        glm::vec3(0, 0, 1),
        glm::vec3(0.3f, 0.3f, 0.3f),
        glm::vec3(0.4f, 0.4f, 0.4f),
        glm::vec3(0.5f, 0.5f, 0.5f)
    }),
    VertexArrayID(0),
    ShaderOpenGL(
        EngineSetup::get()->SHADERS_FOLDER + "Render.vs",
        EngineSetup::get()->SHADERS_FOLDER + "Render.fs",
        false
    )
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    matrixProjectionUniform = glGetUniformLocation(programID, "projection");
    matrixViewUniform = glGetUniformLocation(programID, "view");
    matrixModelUniform = glGetUniformLocation(programID, "model");

    viewPositionUniform = glGetUniformLocation(programID, "viewPos");

    numLightPointsUniform = glGetUniformLocation(programID, "numLights");
    numSpotLightsUniform = glGetUniformLocation(programID, "numSpotLights");

    directionalLightDirectionUniform = glGetUniformLocation(programID, "dirLight.direction");
    directionalLightAmbientUniform = glGetUniformLocation(programID, "dirLight.ambient");
    directionalLightDiffuseUniform = glGetUniformLocation(programID, "dirLight.diffuse");
    directionalLightSpecularUniform = glGetUniformLocation(programID, "dirLight.specular");

    materialTextureDiffuseUniform = glGetUniformLocation(programID, "material.diffuse");
    materialTextureSpecularUniform = glGetUniformLocation(programID, "material.specular");
    materialShininessUniform = glGetUniformLocation(programID, "material.shininess");

    alphaUniform = glGetUniformLocation(programID, "alpha");
}

void ShaderOpenGLRender::render(
    Object3D *o,
    GLint textureID,
    GLint textureSpecularID,
    GLuint vertexbuffer,
    GLuint uvbuffer,
    GLuint normalbuffer,
    int size,
    float alpha,
    GLuint framebuffer
)
{
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);

    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    auto camera = ComponentsManager::get()->getComponentCamera();
    auto cameraPosition = camera->getCamera()->getPosition().toGLM();

    setMat4Uniform(matrixProjectionUniform, camera->getGLMMat4ProjectionMatrix());
    setMat4Uniform(matrixViewUniform, camera->getGLMMat4ViewMatrix());
    setMat4Uniform(matrixModelUniform, o->getModelMatrix());

    setVec3Uniform(viewPositionUniform, cameraPosition);

    // textures
    setIntUniform(materialTextureDiffuseUniform, 0);
    setIntUniform(materialTextureSpecularUniform, 1);
    setFloatUniform(materialShininessUniform, 32.0f);

    setVec3Uniform(directionalLightDirectionUniform, directionalLight.direction);
    setVec3Uniform(directionalLightAmbientUniform, directionalLight.ambient);
    setVec3Uniform(directionalLightDiffuseUniform, directionalLight.diffuse);
    setVec3Uniform(directionalLightSpecularUniform, directionalLight.specular);

    setIntUniform(numLightPointsUniform, (int) pointsLights.size());
    setIntUniform(numSpotLightsUniform, (int) spotLights.size());
    setFloatUniform(alphaUniform, alpha);

    setVec3("drawOffset", o->getDrawOffset().toGLM());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSpecularID);

    setVAOAttributes(vertexbuffer, uvbuffer, normalbuffer);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "PointLightsBlock"), 0);
    glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "SpotLightsBlock"), 1);

    glDrawArrays(GL_TRIANGLES, 0, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(0);
}

void ShaderOpenGLRender::setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ShaderOpenGLRender::createUBOFromLights()
{
    pointsLights.resize(0);
    spotLights.resize(0);
    shadowMappingLights.resize(0);

    if (EngineSetup::get()->ENABLE_LIGHTS) {
        for (auto o : Brakeza3D::get()->getSceneObjects()) {
            if (!o->isEnabled()) continue;
            extractLights(o);
            for (auto a: o->getAttached()) {
                if (!a->isEnabled()) continue;
                extractLights(a);
            }
        }
    }

    fillUBOLights();
}

DirLightOpenGL &ShaderOpenGLRender::getDirectionalLight()
{
    return directionalLight;
}

void ShaderOpenGLRender::destroy()
{
}

void ShaderOpenGLRender::renderMesh(Mesh3D *o, GLuint framebuffer)
{
    for (const auto& m: o->meshes) {
        render(
            o,
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            m.vertexbuffer,
            m.uvbuffer,
            m.normalbuffer,
            (int) m.vertices.size(),
            o->getAlpha(),
            framebuffer
        );
    }
}

void ShaderOpenGLRender::fillUBOLights()
{
    glDeleteBuffers(1, &bufferUBOLightPoints);
    glDeleteBuffers(1, &bufferUBOSpotLights);

    glGenBuffers(1, &bufferUBOLightPoints);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOLightPoints);
    glBufferData(GL_UNIFORM_BUFFER, (int) (pointsLights.size() * sizeof(PointLightOpenGL)), pointsLights.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, bufferUBOLightPoints);

    glGenBuffers(1, &bufferUBOSpotLights);
    glBindBuffer(GL_UNIFORM_BUFFER, bufferUBOSpotLights);
    glBufferData(GL_UNIFORM_BUFFER, (int) (spotLights.size() * sizeof(SpotLightOpenGL)), spotLights.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, bufferUBOSpotLights);
}

void ShaderOpenGLRender::extractLights(Object3D *o)
{
    Vertex3D forward = o->getRotation().Z().getNormalize();

    auto s = dynamic_cast<SpotLight3D*>(o);
    if (s != nullptr) {
        spotLights.push_back(SpotLightOpenGL{
            glm::vec4(o->getPosition().toGLM(), 1),
            glm::vec4(forward.toGLM(), 0),
            s->ambient,
            s->diffuse,
            s->specular,
            s->constant,
            s->linear,
            s->quadratic,
            s->cutOff,
            s->outerCutOff
        });
        return;
    }

    auto l = dynamic_cast<LightPoint3D*>(o);
    if (l != nullptr) {
        shadowMappingLights.push_back(l);
        pointsLights.push_back({
            glm::vec4(o->getPosition().toGLM(), 1),
            l->ambient,
            l->diffuse,
            l->specular,
            l->constant,
            l->linear,
            l->quadratic
        });
    }
}

void ShaderOpenGLRender::setGlobalIlluminationDirection(Vertex3D d)
{
    this->directionalLight.direction = d.toGLM();
}

void ShaderOpenGLRender::setGlobalIlluminationAmbient(Vertex3D a)
{
    this->directionalLight.ambient = a.toGLM();
}

void ShaderOpenGLRender::setGlobalIlluminationDiffuse(Vertex3D d)
{
    this->directionalLight.diffuse = d.toGLM();

}

void ShaderOpenGLRender::setGlobalIlluminationSpecular(Vertex3D s)
{
    this->directionalLight.specular = s.toGLM();
}

void ShaderOpenGLRender::renderAnimatedMesh(Mesh3D *o, GLuint framebuffer)
{
    for (const auto& m: o->meshes) {
        render(
            o,
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            o->getModelTextures()[m.materialIndex]->getOGLTextureID(),
            m.feedbackBuffer,
            m.uvbuffer,
            m.normalbuffer,
            (int) m.vertices.size(),
            o->getAlpha(),
            framebuffer
        );
    }
}

int ShaderOpenGLRender::getNumLightPoints() const {
    return pointsLights.size();
}

int ShaderOpenGLRender::getNumSpotLights() const {
    return spotLights.size();
}

[[nodiscard]] std::vector<LightPoint3D *> &ShaderOpenGLRender::getShadowMappingLightPoints() {
    return shadowMappingLights;
}