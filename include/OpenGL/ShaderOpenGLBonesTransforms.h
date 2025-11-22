//
// Created by Eduardo on 19/03/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H
#define BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H


#include "ShaderBaseOpenGL.h"
#include "../Objects/Mesh3DAnimation.h"

class ShaderOpenGLBonesTransforms : public ShaderBaseOpenGL
{
    GLuint VertexArrayID;

    GLint gBonesUniform;

public:
    ShaderOpenGLBonesTransforms();

    void render(
        Mesh3DData &meshData,
        std::vector<glm::mat4> transformations,
        GLuint framebuffer
    );

    void setVAOAttributes(GLuint vertexbuffer, GLuint vertexBoneDataBuffer);

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H
