
#include "../../include/2D/Button.h"
#include "../../include/ComponentsManager.h"

Button::Button(int x, int y, std::string imageFileName, void (*functionCallBack)()) : Texture(imageFileName) {
    setX(x);
    setY(y);
    setCallBack(functionCallBack);
}

int Button::getX() const {
    return x;
}

int Button::getY() const {
    return y;
}

bool Button::isPressed() const {
    return pressed;
}

void Button::setX(int x) {
    Button::x = x;
}

void Button::setY(int y) {
    Button::y = y;
}

void Button::setPressed(bool pressed) {
    Button::pressed = pressed;
}

void Button::draw() {
    getImage()->drawFlat(x, y);
}

void Button::onUpdate() {
    if (isClicked()) {
        (*callback)();
    }
}

bool Button::isClicked() {
    auto *input = ComponentsManager::get()->getComponentInput();

    if (!input->isClickLeft()) {
        return false;
    }

    int mouseX = input->getRelativeRendererMouseX();
    int mouseY = input->getRelativeRendererMouseY();

    int x1 = getX();
    int y1 = getY();
    int x2 = x1 + getImage()->width();
    int y2 = y1 + getImage()->height();

    if (mouseX > x1 && mouseX < x2) {
        if (mouseY > y1 && mouseY < y2) {
            return true;
        }
    }

    return false;
}

void Button::setCallBack(void (*pFunction)()) {
    this->callback = pFunction;
}
