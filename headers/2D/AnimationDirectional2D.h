
#ifndef BRAKEDA3D_ANIMATION2D_H
#define BRAKEDA3D_ANIMATION2D_H


#include "../Render/Texture.h"
#include "../Render/Timer.h"

#define ANIMATION2D_MAX_FRAMES 10

class AnimationDirectional2D {
public:

    std::string base_file;

    int n_frames;
    int current = 0;
    Texture *frames[9][ANIMATION2D_MAX_FRAMES];

    AnimationDirectional2D();
    void setup(std::string file, int num_frames);

    void loadImages();

    int getNumFrames() const;

    Texture *getCurrentFrame(int direction);
    void nextFrame();
};


#endif //BRAKEDA3D_ANIMATION2D_H
