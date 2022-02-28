
#ifndef BRAKEDA3D_TEXTUREANIMATION_H
#define BRAKEDA3D_TEXTUREANIMATION_H

#include <string>
#include <vector>
#include "../Render/Texture.h"

#define ANIMATION2D_MAX_FRAMES 25

class TextureAnimation {
public:
    std::string base_file;

    int numberFramesToLoad;
    int currentFrame;
    int fps;
    std::vector<Texture*> frames;

    bool endAnimation;
    bool paused;

    TextureAnimation();

    void setup(std::string file, int num_frames, int fps);

    void loadImages();

    int getNumFrames() const;

    void nextFrame();

    Texture *getCurrentFrame();

    bool isEndAnimation() const;

    void setEndAnimation(bool endAnimation);

    bool isPaused() const;

    void setPaused(bool paused);

    int getFps() const;

    void setFps(int fps);
};


#endif //BRAKEDA3D_TEXTUREANIMATION_H
