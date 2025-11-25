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
    Image();
    virtual ~Image();
    Image(SDL_Surface *surface, SDL_Texture *texture);
    explicit Image(const std::string& filename);

    int height() const;
    float getAreaForVertices(const Vertex3D &A, const Vertex3D &B, const Vertex3D &C) const;
    void loadFromRaw(const unsigned int *texture, int w, int h);
    void drawFlat(int, int, GLuint framebuffer) const;
    void drawFlatAlpha(int pos_x, int pos_y, float alpha, GLuint framebuffer);
    void loadTGA(const std::string& filename);
    void* pixels() const;
    bool isLoaded() const;
    void createEmpty(int w, int h);
    void setAlpha(float alpha);
    void setImage(const std::string &basicString);
    Color getColor(int x, int y) const;
    SDL_Texture *getTexture() const;
    ImTextureID getOGLImTexture() const;
    [[nodiscard]] float getAlpha() const;
    [[nodiscard]] GLuint getOGLTextureID() const;
    [[nodiscard]] const std::string &getFileName() const;
    [[nodiscard]] SDL_Surface *getSurface() const;
    [[nodiscard]] int width() const;
    static GLuint makeOGLImage(const SDL_Surface *surfaceTTF);
};

#endif //BRAKEDA3D_IMAGE_H
