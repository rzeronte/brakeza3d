
#ifndef BRAKEDA3D_TEXTUREANIMATED_H
#define BRAKEDA3D_TEXTUREANIMATED_H

#include <string>
#include <vector>
#include "../Misc/Counter.h"
#include "../Misc/Image.h"

#define ANIMATION2D_MAX_FRAMES 25

class TextureAnimated {
public:
    std::string base_file;

    int numberFramesToLoad;
    int currentFrame;
    int fps;
    std::vector<Image*> frames;

    bool endAnimation;
    bool paused;

    Counter counter;

    TextureAnimated(std::string baseFile, int numFrames, int fps);

    explicit TextureAnimated(TextureAnimated *textureAnimated);

    TextureAnimated(const std::string &spriteSheetFile, int spriteWidth, int spriteHeight, int frames, int fps);

    [[nodiscard]] int getNumFrames() const;

    void nextFrame();

    Image *getCurrentFrame();

    [[nodiscard]] bool isEndAnimation() const;

    void setEndAnimation(bool endAnimation);

    [[nodiscard]] bool isPaused() const;

    void setPaused(bool paused);

    [[nodiscard]] int getFps() const;

    void setFps(int fps);

    void updateStep();

    void update();

};


#endif //BRAKEDA3D_TEXTUREANIMATED_H
