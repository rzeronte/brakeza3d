//
// Created by edu on 7/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLRENDER_H
#define BRAKEZA3D_SHADEROPENGLRENDER_H


#include "ShaderOpenGL.h"
#include <SDL2/SDL_opengl.h>
#include <ext/matrix_float4x4.hpp>
#include <vector>

struct AreaLight
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

class ShaderOpenGLRender: public ShaderOpenGL {
    GLuint VertexArrayID;

    GLint MatrixUniform;
    GLint ViewMatrixUniform;
    GLint ModelMatrixUniform;
    GLint TextureUniform;
    GLint pointLightPositionsUniform;
    GLint numPointLightsUniform;
    GLint viewPosUniform;

    GLint LightIDUniform;

    std::vector<AreaLight> pointLightPositions;

public:
    ShaderOpenGLRender(const std::string &vertexFilename, const std::string &fragmentFilename);
    void render(glm::mat4 ModelMatrix, GLint textureID, GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, int size);

    GLuint getVertexArrayID() const;
};


#endif //BRAKEZA3D_SHADEROPENGLRENDER_H
