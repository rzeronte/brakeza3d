
#ifndef SDL2_3D_ENGINE_LINE2D_H
#define SDL2_3D_ENGINE_LINE2D_H


#include <SDL.h>

class Line2D {
    int x1, y1, x2, y2;

public:
    bool debug = false;

    Line2D();
    Line2D(int, int, int, int);

    void setup(int, int, int, int);
    void draw(SDL_Surface *, Uint32);
    void draw(SDL_Surface *screen);
};


#endif //SDL2_3D_ENGINE_LINE2D_H
