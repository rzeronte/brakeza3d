//
// Created by eduardo on 16/2/22.
//

#ifndef BRAKEDA3D_IMAGE_H
#define BRAKEDA3D_IMAGE_H


#include <string>
#include <SDL_surface.h>

class Image {
public:
    Image();
    explicit Image(std::string filename);
    void drawFlat(int, int) const;
    bool loadImage();
    int width();
    int height();
    void* pixels();
    bool isLoaded();
private:
    SDL_Surface *surface;

    bool loaded;
};

#endif //BRAKEDA3D_IMAGE_H
