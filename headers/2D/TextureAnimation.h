
#ifndef BRAKEDA3D_TEXTUREANIMATION_H
#define BRAKEDA3D_TEXTUREANIMATION_H

#include <string>
#include "../Render/Texture.h"

#define ANIMATION2D_MAX_FRAMES 10

class TextureAnimation {
public:
    std::string base_file;

    int numFrames;
    int currentFrame = 0;
    Texture *frames[ANIMATION2D_MAX_FRAMES];

    TextureAnimation();

    void setup(std::string file, int num_frames);
    void loadImages();

    int getNumFrames() const;

    Texture *getCurrentFrame();
    void nextFrame();
};


#endif //BRAKEDA3D_TEXTUREANIMATION_H
