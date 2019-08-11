
#ifndef BRAKEDA3D_TEXTUREANIMATION_H
#define BRAKEDA3D_ANIMATION2D_H


#include "../Render/Texture.h"
#include "../Render/Timer.h"

#define ANIMATION2D_MAX_FRAMES 10

class TextureAnimationDirectional {
public:

    std::string base_file;

    int numFrames;
    int current = 0;
    bool isZeroDirection = false;
    Texture *frames[9][ANIMATION2D_MAX_FRAMES];

    TextureAnimationDirectional();
    void setup(std::string file, int num_frames);

    void loadImages();
    void loadImagesForZeroDirection();

    int getNumFrames() const;

    Texture *getCurrentFrame(int direction);
    void nextFrame();
    void importTextures(TextureAnimationDirectional *origin, int numFrames);
};


#endif //BRAKEDA3D_TEXTUREANIMATION_H
