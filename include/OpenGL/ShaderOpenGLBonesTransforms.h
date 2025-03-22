//
// Created by Eduardo on 19/03/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H
#define BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H


#include "ShaderOpenGL.h"
#include "../Objects/Mesh3DAnimation.h"

class ShaderOpenGLBonesTransforms : public ShaderOpenGL
{
    GLuint VertexArrayID;
public:
    ShaderOpenGLBonesTransforms();

    void render(
        meshData &meshData,
        std::vector<glm::mat4> transformations,
        GLuint framebuffer
    );

    void setVAOAttributes(GLuint vertexbuffer, GLuint vertexBoneDataBuffer);

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H
