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

class Image {

    SDL_Surface *surface;
    SDL_Texture *texture;
    std::string fileName;

    bool loaded;
    GLuint texturaID = 0;
    float alpha = 1;

public:
    Image();

    virtual ~Image();

    explicit Image(const std::string& filename);

    void loadFromRaw(const unsigned int *texture, int w, int h);

    void drawFlat(int, int, GLuint framebuffer) const;

    void drawFlatAlpha(int pos_x, int pos_y, float alpha, GLuint framebuffer);

    void loadTGA(const std::string& filename);

    int height() const;

    void* pixels() const;

    bool isLoaded() const;

    void createEmpty(int w, int h);

    float getAreaForVertices(const Vertex3D &A, const Vertex3D &B, const Vertex3D &C, int lod) const;

    Color getColor(int x, int y) const;

    [[nodiscard]] const std::string &getFileName() const;

    [[nodiscard]] SDL_Surface *getSurface() const;

    [[nodiscard]] int width() const;

    void setImage(const std::string &basicString);

    Image(SDL_Surface *surface, SDL_Texture *texture);

    SDL_Texture *getTexture() const;

    [[nodiscard]] GLuint getOGLTextureID() const;

    static GLuint makeOGLImage(const SDL_Surface *surfaceTTF);

    [[nodiscard]] float getAlpha() const;

    void setAlpha(float alpha);

};

#endif //BRAKEDA3D_IMAGE_H
