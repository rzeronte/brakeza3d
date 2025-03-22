//
// Created by Eduardo on 19/03/2025.
//

#include "../../include/OpenGL/ShaderOpenGLBonesTransforms.h"
#include "../../include/ComponentsManager.h"

ShaderOpenGLBonesTransforms::ShaderOpenGLBonesTransforms()
:
    VertexArrayID(0),
    ShaderOpenGL(EngineSetup::get()->SHADERS_FOLDER + "BonesTransforms.vs",true)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
}

void ShaderOpenGLBonesTransforms::render(
    meshData &meshData,
    std::vector<glm::mat4> transformations,
    GLuint framebuffer
) {
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(framebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    setMat4Array("gBones", transformations);

    setVAOAttributes(meshData.vertexbuffer, meshData.vertexBoneDataBuffer);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, meshData.feedbackBuffer);  // Vinculamos el buffer de feedback
    glBeginTransformFeedback(GL_TRIANGLES);  // Especificamos el tipo de primitivas que estamos procesando

    int size = meshData.vertices.size();
    glDrawArrays(GL_TRIANGLES, 0, size);

    glEndTransformFeedback();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    // Copia los datos del feedbackBuffer al vertexbuffer
    glBindBuffer(GL_COPY_READ_BUFFER, meshData.feedbackBuffer);  // Vincula el buffer de feedback como buffer de lectura
    glBindBuffer(GL_ARRAY_BUFFER, meshData.vertexbuffer);      // Vincula el buffer de vértices como buffer de escritura
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, sizeof(glm::vec4) * size);

    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(0);
}

void ShaderOpenGLBonesTransforms::setVAOAttributes(GLuint vertexbuffer, GLuint vertexBoneDataBuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBoneDataBuffer);
    glVertexAttribIPointer(1, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)offsetof(VertexBoneData, IDs));

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBoneDataBuffer);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)offsetof(VertexBoneData, Weights));
}

void ShaderOpenGLBonesTransforms::destroy() {

}
