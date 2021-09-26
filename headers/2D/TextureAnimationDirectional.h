
#ifndef BRAKEDA3D_TEXTUREANIMATIONDIRECTIONAL_H
#define BRAKEDA3D_TEXTUREANIMATIONDIRECTIONAL_H

#include "../Render/Texture.h"
#include "../Misc/Timer.h"

#define ANIMATION2D_MAX_FRAMES 10

class TextureAnimationDirectional {
public:

    std::string base_file;

    int numFrames;
    int current = 0;
    int times = 0;
    int maxTimes = -1;

    int fps = 20;

    bool isZeroDirection = false;
    Texture *frames[9][ANIMATION2D_MAX_FRAMES];

    TextureAnimationDirectional();

    void setup(std::string file, int numFrames, int fps, int maxTimes);

    void loadImages();

    void loadImagesForZeroDirection();

    Texture *getCurrentFrame(int direction);

    int getNumFrames() const;

    void nextFrame();

    void importTextures(TextureAnimationDirectional *origin, int numFrames);
};


#endif //BRAKEDA3D_TEXTUREANIMATIONDIRECTIONAL_H
