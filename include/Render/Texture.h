
#ifndef SDL2_3D_ENGINE_TEXTURE_H
#define SDL2_3D_ENGINE_TEXTURE_H


#include <SDL2/SDL_surface.h>
#include <string>
#include "../Objects/Point2D.h"
#include "../Objects/Vertex3D.h"
#include "../Misc/Image.h"

class Texture {
private:
    Image *image;
public:
    Texture();

    Texture(std::string filename);
    Image *getImage() const;
};


#endif //SDL2_3D_ENGINE_TEXTURE_H
