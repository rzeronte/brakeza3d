
#ifndef BRAKEDA3D_TEXTUREANIMATIONDIRECTIONAL_H
#define BRAKEDA3D_TEXTUREANIMATIONDIRECTIONAL_H

#include "../Render/Texture.h"
#include "../Render/Timer.h"

#define ANIMATION2D_MAX_FRAMES 10

class TextureAnimationDirectional {
public:

    std::string base_file;

    int numFrames;
    int current = 0;
    int times = 0;
    int maxTimes = -1;

    bool isZeroDirection = false;
    Texture *frames[9][ANIMATION2D_MAX_FRAMES];

    TextureAnimationDirectional();
    void setup(std::string file, int num_frames, int maxTimes);

    void loadImages();
    void loadImagesForZeroDirection();

    int getNumFrames() const;

    Texture *getCurrentFrame(int direction);
    void nextFrame();
    void importTextures(TextureAnimationDirectional *origin, int numFrames);
};


#endif //BRAKEDA3D_TEXTUREANIMATIONDIRECTIONAL_H
