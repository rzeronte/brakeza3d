
#ifndef BRAKEDA3D_TEXTUREANIMATED_H
#define BRAKEDA3D_TEXTUREANIMATED_H

#include <string>
#include <vector>
#include "../Misc/Counter.h"
#include "Image.h"

class TextureAnimated {
public:
    std::string baseFilename;

    int numberFramesToLoad;
    int currentFrame = 0;
    int fps;
    std::vector<Image*> frames;

    bool endAnimation = false;
    bool paused = false;

    Counter counter;

    int currentSpriteWidth = 0;
    int currentspriteHeight = 0;

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

    [[nodiscard]] const std::string &getBaseFilename() const;

    void setup(const std::string &spriteSheetFile, int spriteWidth, int spriteHeight, int numFrames, int fps);

    void deleteFrames();

    void drawImGuiProperties();
};


#endif //BRAKEDA3D_TEXTUREANIMATED_H
