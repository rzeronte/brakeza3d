//
// Created by eduardo on 16/2/22.
//

#ifndef BRAKEDA3D_IMAGE_H
#define BRAKEDA3D_IMAGE_H


#include <string>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
#include "../Objects/Vertex3D.h"
#include "Color.h"

class Image {
private:

    SDL_Surface *surface;
    SDL_Texture *texture;
    std::string fileName;

    bool loaded;
    GLuint texturaID;
    float alpha = 0;

public:
    Image();

    virtual ~Image();

    explicit Image(const std::string& filename);

    void loadFromRaw(unsigned int *texture, int w, int h);

    void drawFlat(int, int, GLuint framebuffer) const;

    void drawFlatAlpha(int pos_x, int pos_y, float alpha, GLuint framebuffer);

    void loadTGA(const std::string& filename);

    int height();

    void* pixels();
    bool isLoaded();

    void createEmpty(int w, int h);

    float getAreaForVertices(Vertex3D A, Vertex3D B, Vertex3D C, int lod);

    Color getColor(int x, int y);

    [[nodiscard]] const std::string &getFileName() const;

    [[nodiscard]] SDL_Surface *getSurface();

    [[nodiscard]] int width() const;

    void setImage(const std::string &basicString);

    Image(SDL_Surface *surface, SDL_Texture *texture);

    SDL_Texture *getTexture();

    [[nodiscard]] GLuint getOGLTextureID() const;

    static GLuint makeOGLImage(SDL_Surface *surfaceTTF);

    float getAlpha() const;

    void setAlpha(float alpha);

};

#endif //BRAKEDA3D_IMAGE_H
