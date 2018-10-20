
#ifndef BRAKEDA3D_ANIMATION2D_H
#define BRAKEDA3D_ANIMATION2D_H

#include <string>
#include "Texture.h"

#define ANIMATION2D_MAX_FRAMES 10

class Animation2D {
public:
    std::string base_file;

    int n_frames;
    int current = 0;
    Texture *frames[ANIMATION2D_MAX_FRAMES];

    Animation2D();

    void setup(std::string file, int num_frames);
    void loadImages();

    int getNumFrames() const;

    Texture *getCurrentFrame();
    void nextFrame();
};


#endif //BRAKEDA3D_ANIMATION2D_H
