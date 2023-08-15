//
// Created by eduardo on 28/2/22.
//

#ifndef BRAKEDA3D_BUTTON_H
#define BRAKEDA3D_BUTTON_H


#include "../Misc/Image.h"

class Button: public Image {
private:
    int x;
    int y;
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

    void draw();

    void onUpdate();

    bool isClicked();

    void setCallBack(void (*pFunction)());

    void drawSelectedEffect();
};


#endif //BRAKEDA3D_BUTTON_H
