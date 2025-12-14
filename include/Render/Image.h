//
// Created by eduardo on 16/2/22.
//

#ifndef BRAKEDA3D_IMAGE_H
#define BRAKEDA3D_IMAGE_H

#include <string>
#include <SDL2/SDL_surface.h>
#include <GL/glew.h>
#include "../3D/Vertex3D.h"
#include "Color.h"

class Image {

    bool loaded = false;
    float alpha = 1;
    GLuint textureId = 0;
    std::string fileName;
    SDL_Surface *surface = nullptr;
    SDL_Texture *texture = nullptr;
public:
    Image() = default;
    explicit Image(std::string filename);
    Image(SDL_Surface *surface, SDL_Texture *texture);
    virtual ~Image();

    void CreateSDLTexture();
    void LoadSDLSurface();
    void MakeAutoOGLImage();
    void LoadFromRaw(const unsigned int *texture, int w, int h);
    void DrawFlat(int, int, GLuint framebuffer) const;
    void DrawFlatAlpha(int pos_x, int pos_y, float alpha, GLuint framebuffer);
    void DrawFlatAlpha(int x, int y, int w, int h, float alpha, GLuint fbo);
    void DrawFlat(int x, int y, int width, int height, GLuint fbo) const;

    void setAlpha(float alpha);
    void setOGLTextureID(GLuint value);
    void setImage(const std::string &basicString);
    void setAlreadyLoaded();
    void setFilePath(std::string &path);

    [[nodiscard]] float getAreaForVertices(const Vertex3D &A, const Vertex3D &B, const Vertex3D &C) const;

    [[nodiscard]] float getAlpha() const                        { return alpha; }
    [[nodiscard]] bool isLoaded() const                         { return loaded; }
    [[nodiscard]] int height() const                            { return surface->h; }
    [[nodiscard]] int width() const                             { return surface->w; }
    [[nodiscard]] void* pixels() const                          { return surface->pixels; }
    [[nodiscard]] SDL_Texture *getTexture() const               { return texture; }
    [[nodiscard]] SDL_Surface *getSurface() const               { return surface; }
    [[nodiscard]] ImTextureID getOGLImTexture() const           { return (ImTextureID) textureId; }
    [[nodiscard]] GLuint getOGLTextureID() const                { return textureId; }
    [[nodiscard]] const std::string &getFileName() const        { return fileName; }
    [[nodiscard]] Color getPixelColor(int x, int y) const;

    static GLuint MakeOGLImage(const SDL_Surface *surfaceTTF);
};

#endif //BRAKEDA3D_IMAGE_H
