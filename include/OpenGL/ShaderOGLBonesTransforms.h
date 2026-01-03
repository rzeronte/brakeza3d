//
// Created by Eduardo on 19/03/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H
#define BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H


#include "Base/ShaderBaseOpenGL.h"
#include "../3D/Mesh3D.h"

class ShaderOGLBonesTransforms : public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;
    GLuint gBonesUniform = 0;
public:
    ShaderOGLBonesTransforms();

    void PrepareBackground() override;
    void PrepareMainThread() override;

    void LoadUniforms() override;
    void render(Mesh3DData &meshData, std::vector<glm::mat4> transformations,GLuint fbo);
    void setVAOAttributes(GLuint vertexbuffer, GLuint vertexBoneDataBuffer);

    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLBONESTRANSFORMS_H
