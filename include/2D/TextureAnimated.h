
#ifndef BRAKEDA3D_TEXTUREANIMATED_H
#define BRAKEDA3D_TEXTUREANIMATED_H

#include <string>
#include <vector>
#include "../Render/Texture.h"

#define ANIMATION2D_MAX_FRAMES 25

class TextureAnimated {
public:
    std::string base_file;

    int numberFramesToLoad;
    int currentFrame;
    int fps;
    std::vector<Texture*> frames;

    bool endAnimation;
    bool paused;

    TextureAnimated(std::string baseFile, int numFrames, int fps);

    [[nodiscard]] int getNumFrames() const;

    void nextFrame();

    Texture *getCurrentFrame();

    [[nodiscard]] bool isEndAnimation() const;

    void setEndAnimation(bool endAnimation);

    [[nodiscard]] bool isPaused() const;

    void setPaused(bool paused);

    [[nodiscard]] int getFps() const;

    void setFps(int fps);
};


#endif //BRAKEDA3D_TEXTUREANIMATED_H
