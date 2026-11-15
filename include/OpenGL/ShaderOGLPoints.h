/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

#ifndef BRAKEZA3D_SHADEROPENGLPOINTS_H
#define BRAKEZA3D_SHADEROPENGLPOINTS_H

#include "Base/ShaderBaseOpenGL.h"
#include "../3D/Mesh3D.h"
#include "../3D/Mesh3DAnimation.h"

class ShaderOGLPoints : public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;
public:
    ShaderOGLPoints();
    void PrepareMainThread() override;
    void LoadUniforms() override;

    void render(glm::mat4 modelMatrix, GLuint particlesBuffer, int numberPoints, Color c, GLuint fbo, GLuint indexBuffer = 0, GLsizei indexCount = 0) const;
    void Destroy() override;
    void renderMesh(Mesh3D *mesh, bool useFeedbackBuffer, GLuint fbo);
    void renderMeshAnimation(Mesh3DAnimation *mesh, GLuint fbo) const;
    static void setVAOAttributes(GLuint particlesBuffer) ;
};


#endif //BRAKEZA3D_SHADEROPENGLPOINTS_H
