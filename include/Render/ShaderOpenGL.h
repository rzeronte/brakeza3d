//
// Created by edu on 7/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGL_H
#define BRAKEZA3D_SHADEROPENGL_H


#include <SDL2/SDL_opengl.h>
#include <string>

class ShaderOpenGL {
    std::string vertexFilename;
    std::string fragmentFilename;
protected:
    GLuint programID;
public:
    ShaderOpenGL(const std::string &vertexFilename, const std::string &fragmentFilename);
    static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

    GLuint getProgramID() const;
};


#endif //BRAKEZA3D_SHADEROPENGL_H
