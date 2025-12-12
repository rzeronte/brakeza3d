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
    virtual ~Image();
    Image() = default;
    Image(SDL_Surface *surface, SDL_Texture *texture);

    void CreateSDLTexture();

    void LoadSDLSurface();

    explicit Image(std::string filename);

    static Image *Create(std::string file);

    void LoadFromRaw(const unsigned int *texture, int w, int h);
    void DrawFlat(int, int, GLuint framebuffer) const;
    void DrawFlatAlpha(int pos_x, int pos_y, float alpha, GLuint framebuffer);
    void setAlpha(float alpha);

    void setOGLTextureID(GLuint value);

    void setImage(std::string basicString);

    void setAlreadyLoaded();

    [[nodiscard]] bool isLoaded() const;
    [[nodiscard]] float getAlpha() const;
    [[nodiscard]] int height() const;
    [[nodiscard]] int width() const;
    [[nodiscard]] void* pixels() const;
    [[nodiscard]] float getAreaForVertices(const Vertex3D &A, const Vertex3D &B, const Vertex3D &C) const;
    [[nodiscard]] Color getColor(int x, int y) const;
    [[nodiscard]] SDL_Texture *getTexture() const;
    [[nodiscard]] ImTextureID getOGLImTexture() const;

    void MakeAutoOGLImage();

    [[nodiscard]] GLuint getOGLTextureID() const;
    [[nodiscard]] const std::string &getFileName() const;
    [[nodiscard]] SDL_Surface *getSurface() const;
    static GLuint MakeOGLImage(const SDL_Surface *surfaceTTF);
};

#endif //BRAKEDA3D_IMAGE_H
