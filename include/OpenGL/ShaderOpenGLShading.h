//
// Created by edu on 12/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLSHADING_H
#define BRAKEZA3D_SHADEROPENGLSHADING_H


#include "ShaderOpenGL.h"

class ShaderOpenGLShading : public ShaderOpenGL {
    GLuint VertexArrayID;
public:
    ShaderOpenGLShading(const std::string &vertexFilename, const std::string &fragmentFilename);

    void render(glm::mat4 ModelMatrix, GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, int size);

    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer);

};


#endif //BRAKEZA3D_SHADEROPENGLSHADING_H
