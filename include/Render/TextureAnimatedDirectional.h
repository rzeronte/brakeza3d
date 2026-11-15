/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */

#ifndef BRAKEDA3D_TEXTUREANIMATEDDIRECTIONAL_H
#define BRAKEDA3D_TEXTUREANIMATEDDIRECTIONAL_H

#include "Image.h"

#define ANIMATION2D_MAX_FRAMES 10

class TextureAnimatedDirectional
{
    int numFrames = 0;
    int current = 0;
    int times = 0;
    int maxTimes = -1;
    int fps = 20;
    bool isZeroDirection = false;
    bool loaded = false;

    std::string baseFolder;
    Image *frames[9][ANIMATION2D_MAX_FRAMES] = {};

public:
    TextureAnimatedDirectional(std::string file, int newNumFrames, int newFps, int newMaxTimes);
    void LoadImages();
    void LoadImagesForZeroDirection();
    void NextFrame();
    void ImportTextures(const TextureAnimatedDirectional *origin, int numFrames);
    void DrawImGuiProperties();
    void setIsZeroDirection(bool value);
    Image *getCurrentFrame(int direction);
    [[nodiscard]] bool isLoaded() const;
    [[nodiscard]] bool hasZeroDirection() const;
    [[nodiscard]] int getNumFrames() const;
    [[nodiscard]] int getFps() const;
    [[nodiscard]] std::string getBaseFile() const;
};


#endif //BRAKEDA3D_TEXTUREANIMATEDDIRECTIONAL_H
