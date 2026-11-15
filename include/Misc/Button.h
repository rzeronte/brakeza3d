/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by eduardo on 28/2/22.
//

#ifndef BRAKEDA3D_BUTTON_H
#define BRAKEDA3D_BUTTON_H

#include "../Render/Image.h"

class Button: public Image
{
    int x = 0;
    int y = 0;
    bool pressed;
public:
    void (*callback)();
    Button(int x, int y, const std::string& imageFileName, void (*functionCallBack)());

    [[nodiscard]] int getX() const;
    [[nodiscard]] int getY() const;
    [[nodiscard]] bool isPressed() const;
    void setX(int x);
    void setY(int y);
    void setPressed(bool pressed);
    void draw(GLuint fbo);
    void onUpdate();
    bool isClicked();
    void setCallBack(void (*pFunction)());
    void drawSelectedEffect();
};


#endif //BRAKEDA3D_BUTTON_H
