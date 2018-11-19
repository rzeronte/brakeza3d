
#ifndef SDL2_3D_ENGINE_TEXTURE_H
#define SDL2_3D_ENGINE_TEXTURE_H


#include <SDL_surface.h>
#include <string>

class Texture {
private:
    SDL_Surface *texture_surface;
    std::string filename;
public:
    bool loaded = false;

    Texture();
    Texture(int, int);

    void drawFlat(int, int);

    void loadBMP(const char *file);
    void loadJPG(const char *file);
    void loadTGA(const char *file);
    void loadFromRaw(unsigned int *texture, int, int);

    SDL_Surface *getSurface();
    void consoleInfo();

    const std::string &getFilename() const;
    void setFilename(const std::string &filename);
};


#endif //SDL2_3D_ENGINE_TEXTURE_H
