
#ifndef BRAKEDA3D_TEXTUREANIMATEDDIRECTIONAL_H
#define BRAKEDA3D_TEXTUREANIMATEDDIRECTIONAL_H

#include "../Misc/Timer.h"
#include "../Misc/Image.h"

#define ANIMATION2D_MAX_FRAMES 10

class TextureAnimatedDirectional {
public:

    std::string base_file;

    int numFrames;
    int current = 0;
    int times = 0;
    int maxTimes = -1;

    int fps = 20;

    bool isZeroDirection = false;
    Image *frames[9][ANIMATION2D_MAX_FRAMES];

    TextureAnimatedDirectional();

    void setup(std::string file, int newNumFrames, int newFps, int newMaxTimes);

    void loadImages();

    void loadImagesForZeroDirection();

    Image *getCurrentFrame(int direction);

    [[nodiscard]] int getNumFrames() const;

    void nextFrame();

    void importTextures(TextureAnimatedDirectional *origin, int numFrames);
};


#endif //BRAKEDA3D_TEXTUREANIMATEDDIRECTIONAL_H
