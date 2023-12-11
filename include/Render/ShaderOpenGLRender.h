//
// Created by edu on 7/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLRENDER_H
#define BRAKEZA3D_SHADEROPENGLRENDER_H


#include "ShaderOpenGL.h"
#include "../Objects/OpenGLShaderTypes.h"
#include <SDL2/SDL_opengl.h>
#include <ext/matrix_float4x4.hpp>
#include <vector>

class ShaderOpenGLRender: public ShaderOpenGL {
    GLuint VertexArrayID;

    DirLight directionalLight;
    std::vector<PointLight> pointsLights;
    GLuint bufferUBO;

public:
    ShaderOpenGLRender(const std::string &vertexFilename, const std::string &fragmentFilename);
    void render(glm::mat4 ModelMatrix, GLint textureID, GLint textureSpecularID, GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, int size);

    static void selectActiveTextures(GLint textureID, GLint textureSpecularID) ;
    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer) ;

    void fillPointLightsUniform() const;

    void fillPointLightsUBO();

    void getPointLightFromSceneObjects();
};


#endif //BRAKEZA3D_SHADEROPENGLRENDER_H
