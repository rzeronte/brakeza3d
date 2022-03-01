//
// Created by eduardo on 28/2/22.
//

#ifndef BRAKEDA3D_BUTTON_H
#define BRAKEDA3D_BUTTON_H


#include "../Render/Texture.h"

class Button: public Texture {
private:
    int x;
    int y;
    bool pressed;
public:
    void (*callback)();
    Button(int x, int y, std::string imageFileName, void (*functionCallBack)());

    int getX() const;

    int getY() const;

    bool isPressed() const;

    void setX(int x);

    void setY(int y);

    void setPressed(bool pressed);

    void draw();

    void onUpdate();

    bool isClicked();

    void setCallBack(void (*pFunction)());

    void drawSelectedEffect();
};


#endif //BRAKEDA3D_BUTTON_H
