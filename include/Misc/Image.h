//
// Created by eduardo on 16/2/22.
//

#ifndef BRAKEDA3D_IMAGE_H
#define BRAKEDA3D_IMAGE_H


#include <string>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <CL/cl.h>
#include <SDL_opengl.h>
#include "../Objects/Vertex3D.h"
#include "Color.h"

class Image {
private:

    SDL_Surface *surface;
    SDL_Texture *texture;
    std::string fileName;
    cl_mem openClTexture;

    bool loaded;
    GLuint texturaID;

public:
    Image();

    virtual ~Image();

    explicit Image(const std::string& filename);

    void loadFromRaw(unsigned int *texture, int w, int h);

    void drawFlat(int, int) const;

    void drawFlatAlpha(int pos_x, int pos_y, float alpha);

    void loadTGA(const std::string& filename);

    int height();

    void* pixels();
    bool isLoaded();

    void setSurface(SDL_Surface *surface);

    void createEmpty(int w, int h);

    float getAreaForVertices(Vertex3D A, Vertex3D B, Vertex3D C, int lod);

    Color getColor(int x, int y);

    void setTextureAlpha(int value);

    [[nodiscard]] const std::string &getFileName() const;

    [[nodiscard]] SDL_Surface *getSurface();

    [[nodiscard]] int width() const;

    cl_mem *getOpenClTexture();

    void loadOpenCLBuffer();

    void setImage(const std::string &basicString);

    Image(SDL_Surface *surface, SDL_Texture *texture);

    SDL_Texture *getTexture();

    [[nodiscard]] GLuint getOGLTextureID() const;

    static GLuint makeOGLImage(SDL_Surface *surfaceTTF);

};

#endif //BRAKEDA3D_IMAGE_H
