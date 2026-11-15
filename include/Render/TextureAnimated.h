/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

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

    SDL_Surface* spriteSheetSurface = nullptr;

    TextureAnimated(std::string baseFile, int numFrames, int fps);

    explicit TextureAnimated(const TextureAnimated *textureAnimated);

    TextureAnimated(const std::string &spriteSheetFile, int spriteWidth, int spriteHeight, int frames, int fps);

    void LoadCurrentSetup();

    [[nodiscard]] int getNumFrames() const;

    void nextFrame();

    Image *getCurrentFrame() const;

    [[nodiscard]] bool isEndAnimation() const;

    void setEndAnimation(bool endAnimation);

    [[nodiscard]] bool isPaused() const;

    void setPaused(bool paused);

    [[nodiscard]] int getFps() const;

    void setFps(int fps);

    void UpdateStep();

    void update();

    [[nodiscard]] const std::string &getBaseFilename() const;

    void Apply(const std::string &spriteSheetFile, int spriteWidth, int spriteHeight, int numFrames, int fps);

    void DeleteFrames();

    void drawImGuiProperties();
};


#endif //BRAKEDA3D_TEXTUREANIMATED_H
