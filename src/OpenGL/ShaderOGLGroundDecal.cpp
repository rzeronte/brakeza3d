#include "../../include/OpenGL/ShaderOGLGroundDecal.h"
#include "../../include/Config.h"
#include "../../include/Components/Components.h"
#include "../../include/3D/Object3D.h"
#include <SDL_image.h>

struct GDVertex { float x, y, z, u, v; };

ShaderOGLGroundDecal::ShaderOGLGroundDecal()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "GroundDecal.vs",
        Config::get()->SHADERS_FOLDER + "GroundDecal.fs",
        false
    )
{
}

void ShaderOGLGroundDecal::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    glGenVertexArrays(1, &vao);
}

void ShaderOGLGroundDecal::LoadUniforms()
{
    viewUniform       = glGetUniformLocation(programID, "view");
    projectionUniform = glGetUniformLocation(programID, "projection");
    tintUniform       = glGetUniformLocation(programID, "tintColor");
    textureUniform    = glGetUniformLocation(programID, "decalTexture");
}

GLuint ShaderOGLGroundDecal::loadTexture(const std::string& path)
{
    auto it = textureCache.find(path);
    if (it != textureCache.end()) return it->second;

    SDL_Surface* surf = IMG_Load(path.c_str());
    if (!surf) return 0;

    // Flip vertically (OpenGL origin is bottom-left)
    SDL_Surface* flipped = SDL_CreateRGBSurface(0, surf->w, surf->h, 32,
        0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    SDL_BlitSurface(surf, nullptr, flipped, nullptr);
    for (int y = 0; y < surf->h / 2; y++) {
        auto* top = (Uint8*)flipped->pixels + y * flipped->pitch;
        auto* bot = (Uint8*)flipped->pixels + (flipped->h - 1 - y) * flipped->pitch;
        std::vector<Uint8> tmp(flipped->pitch);
        memcpy(tmp.data(), top, flipped->pitch);
        memcpy(top, bot, flipped->pitch);
        memcpy(bot, tmp.data(), flipped->pitch);
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, flipped->w, flipped->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, flipped->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    SDL_FreeSurface(surf);
    SDL_FreeSurface(flipped);

    textureCache[path] = tex;
    return tex;
}

void ShaderOGLGroundDecal::draw(Object3D* obj, const std::string& texturePath, const Color& tint, float radius, GLuint fbo)
{
    if (!obj || !obj->isEnabled()) return;
    if (programID == 0 || vao == 0) return;

    GLuint tex = loadTexture(texturePath);
    if (tex == 0) return;

    auto render = Components::get()->Render();
    auto camera = Components::get()->Camera();

    auto pos = obj->getPosition();
    float cx = pos.x, cy = pos.y + 0.05f, cz = pos.z;
    float r  = radius;

    GDVertex verts[6] = {
        { cx-r, cy, cz-r,  0.0f, 0.0f },
        { cx+r, cy, cz-r,  1.0f, 0.0f },
        { cx+r, cy, cz+r,  1.0f, 1.0f },
        { cx-r, cy, cz-r,  0.0f, 0.0f },
        { cx+r, cy, cz+r,  1.0f, 1.0f },
        { cx-r, cy, cz+r,  0.0f, 1.0f },
    };

    render->ChangeOpenGLFramebuffer(fbo);
    render->ChangeOpenGLProgram(programID);

    setMat4Uniform(viewUniform,       camera->getGLMMat4ViewMatrix());
    setMat4Uniform(projectionUniform, camera->getGLMMat4ProjectionMatrix());
    setVec4Uniform(tintUniform, glm::vec4(tint.r, tint.g, tint.b, tint.a));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(textureUniform, 0);

    GLuint VBO;
    glGenBuffers(1, &VBO);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GDVertex), verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GDVertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GDVertex), (void*)(3 * sizeof(float)));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-10.0f, -10.0f);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    render->ChangeOpenGLFramebuffer(0);
}

void ShaderOGLGroundDecal::Destroy()
{
    for (auto& [path, tex] : textureCache)
        glDeleteTextures(1, &tex);
    textureCache.clear();
    glDeleteVertexArrays(1, &vao);
}
