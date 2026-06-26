//
// Created by Eduardo on 19/03/2025.
//

#include "../../include/OpenGL/ShaderOGLBonesTransforms.h"
#include "../../include/Components/Components.h"

ShaderOGLBonesTransforms::ShaderOGLBonesTransforms()
:
    ShaderBaseOpenGL(Config::get()->SHADERS_FOLDER + "BonesTransforms.vs", true)
{
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
}

void ShaderOGLBonesTransforms::PrepareBackground()
{
}

void ShaderOGLBonesTransforms::PrepareMainThread()
{
    LoadUniforms();
}

void ShaderOGLBonesTransforms::LoadUniforms()
{
    gBonesUniform = glGetUniformLocation(programID, "gBones");
}

void ShaderOGLBonesTransforms::render(
    Mesh3DData &meshData,
    const std::vector<glm::mat4>& transformations,
    GLuint fbo
) {
    Components::get()->Render()->ChangeOpenGLFramebuffer(fbo);
    Components::get()->Render()->ChangeOpenGLProgram(programID);
    glBindVertexArray(VertexArrayID);

    setMat4ArrayUniform(gBonesUniform, transformations);

    setVAOAttributes(meshData.vertexBuffer, meshData.vertexBoneDataBuffer, meshData.normalBuffer);

    // Buffer Feedback!
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, meshData.feedbackBuffer);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, meshData.feedbackNormalBuffer);
    glBeginTransformFeedback(GL_TRIANGLES);

    glDrawArrays(GL_TRIANGLES, 0, meshData.vertices.size());

    glEndTransformFeedback();

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLBonesTransforms::setVAOAttributes(GLuint vertexbuffer, GLuint vertexBoneDataBuffer, GLuint normalBuffer)
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

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ShaderOGLBonesTransforms::Destroy() {

}
