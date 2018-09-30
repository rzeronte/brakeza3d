
#ifndef SDL2_3D_ENGINE_TEXTURE_H
#define SDL2_3D_ENGINE_TEXTURE_H


#include <SDL_surface.h>
#include <string>

class Texture {
public:
    bool loaded;
    std::string filename;

    Texture();

    SDL_Surface *texture_surface;

    void drawFlat(SDL_Surface *surface);

    void loadBMP(const char *file);
    void loadJPG(const char *file);
    void drawUVFlat(SDL_Surface *surface, float x, float y, Uint32);

    void loadTGA(const char *file);

    void consoleInfo();
};


#endif //SDL2_3D_ENGINE_TEXTURE_H
