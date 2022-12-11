//
// Created by eduardo on 16/2/22.
//

#ifndef BRAKEDA3D_IMAGE_H
#define BRAKEDA3D_IMAGE_H


#include <string>
#include <SDL_surface.h>
#include <SDL_image.h>
#include "../Objects/Vertex3D.h"
#include "Color.h"

class Image {
public:
    Image();
    explicit Image(std::string filename);
    void drawFlat(int, int) const;
    void drawFlatAlpha(int pos_x, int pos_y, float alpha);

    void loadTGA(std::string filename);
    int width() const;
    int height();
    void* pixels();
    bool isLoaded();

    void setSurface(SDL_Surface *surface);
    void createEmpty(int w, int h);

    [[nodiscard]] SDL_Surface *getSurface() const;

    float getAreaForVertices(Vertex3D A, Vertex3D B, Vertex3D C, int lod);

    Color getColor(int x, int y);

    void setTextureAlpha(int value);
private:
    SDL_Surface *surface;
    SDL_Texture *texture;
    std::string fileName;
    bool loaded;

    void loadFromRaw(unsigned int *texture, int w, int h);
public:
    const std::string &getFileName() const;

    virtual ~Image();
};

#endif //BRAKEDA3D_IMAGE_H
