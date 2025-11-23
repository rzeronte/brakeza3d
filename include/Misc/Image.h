//
// Created by eduardo on 16/2/22.
//

#ifndef BRAKEDA3D_IMAGE_H
#define BRAKEDA3D_IMAGE_H


#include <string>
#include <SDL2/SDL_surface.h>
#include <GL/glew.h>
#include "../Objects/Vertex3D.h"
#include "Color.h"

class Image
{
    bool loaded;
    float alpha = 1;

    SDL_Surface *surface;
    SDL_Texture *texture;
    std::string fileName;

    GLuint texturaID = 0;

public:
    Image();
    Image(SDL_Surface *surface, SDL_Texture *texture);
    virtual ~Image();
    explicit Image(const std::string& filename);
    float getAreaForVertices(const Vertex3D &A, const Vertex3D &B, const Vertex3D &C) const;
    int height() const;
    bool isLoaded() const;
    void loadFromRaw(const unsigned int *texture, int w, int h);
    void drawFlat(int, int, GLuint fbo) const;
    void drawFlatAlpha(int pos_x, int pos_y, float alpha, GLuint fbo);
    void loadTGA(const std::string& filename);
    void* pixels() const;
    void createEmpty(int w, int h);
    void setImage(const std::string &basicString);
    void setAlpha(float alpha);
    SDL_Texture *getTexture() const;
    ImTextureID getOGLImTexture() const;
    Color getColor(int x, int y) const;
    [[nodiscard]] GLuint getOGLTextureID() const;
    [[nodiscard]] float getAlpha() const;
    [[nodiscard]] const std::string &getFileName() const;
    [[nodiscard]] SDL_Surface *getSurface() const;
    [[nodiscard]] int width() const;
    static GLuint makeOGLImage(const SDL_Surface *surfaceTTF);
};

#endif //BRAKEDA3D_IMAGE_H
