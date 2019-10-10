
#ifndef BRAKEDA3D_TEXTUREANIMATION_H
#define BRAKEDA3D_TEXTUREANIMATION_H

#include <string>
#include "../Render/Texture.h"

#define ANIMATION2D_MAX_FRAMES 25

class TextureAnimation {
public:
    std::string base_file;

    int numFrames;
    int currentFrame;
    Texture *frames[ANIMATION2D_MAX_FRAMES];

    bool endAnimation;
    bool paused;

    TextureAnimation();

    void setup(std::string file, int num_frames);
    void loadImages();

    int getNumFrames() const;
    void nextFrame();

    Texture *getCurrentFrame();

    bool isEndAnimation() const;
    void setEndAnimation(bool endAnimation);

    bool isPaused() const;

    void setPaused(bool paused);
};


#endif //BRAKEDA3D_TEXTUREANIMATION_H
