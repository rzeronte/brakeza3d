//
// Created by edu on 7/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLRENDER_H
#define BRAKEZA3D_SHADEROPENGLRENDER_H


#include "ShaderOpenGL.h"
#include "../Objects/OpenGLShaderTypes.h"
#include "../../src/Objects/SpotLight3D.h"
#include <SDL2/SDL_opengl.h>
#include <ext/matrix_float4x4.hpp>
#include <vector>

class ShaderOpenGLRender: public ShaderOpenGL {
    GLuint VertexArrayID;

    DirLightOpenGL directionalLight;
    std::vector<PointLightOpenGL> pointsLights;
    std::vector<SpotLightOpenGL> spotLights;
    GLuint bufferUBO;
    GLuint bufferUBOSpot;

public:
    ShaderOpenGLRender();
    void render(Object3D *o, GLint textureID, GLint textureSpecularID, GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, int size, GLuint framebuffer);

    static void selectActiveTextures(GLint textureID, GLint textureSpecularID) ;
    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer) ;

    void fillPointLightsUBO();

    void getPointLightFromSceneObjects();

    DirLightOpenGL *getDirectionalLight();

};


#endif //BRAKEZA3D_SHADEROPENGLRENDER_H
