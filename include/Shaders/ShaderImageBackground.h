#ifndef BRAKEDA3D_SHADERIMAGEBACKGROUND_H
#define BRAKEDA3D_SHADERIMAGEBACKGROUND_H

#include "../Render/Shader.h"
#include "../Misc/Color.h"
#include "../EngineBuffers.h"
#include "../Misc/Image.h"

enum ShaderImageBackgroundTypes {
    FULLSCREEN = 0,
    PORTION = 1,
    CENTER = 2
};

class ShaderImageBackground: public Shader {
private:
    Vertex3D autoScrollSpeed;

public:
    ShaderImageBackground() {
        setType(ShaderImageBackgroundTypes::FULLSCREEN);
    };

    ShaderImageBackground(const char *filename) {
        setType(ShaderImageBackgroundTypes::FULLSCREEN);
        image = new Image(filename);
    }

    void onUpdate() override {
        if (!isEnabled()) {
            return;
        }

        if (!image->isLoaded()) {
            return;
        }

        if (autoScrollEnabled) {
            autoScrollCursorX += autoScrollSpeed.x;
            autoScrollCursorY += autoScrollSpeed.y;
            setupFlatPortion(
                xDrawPos, yDrawPos ,
                autoScrollCursorX, autoScrollCursorY,
                wImage, hImage
            );
        }

        switch(type) {
            case ShaderImageBackgroundTypes::FULLSCREEN:
                image->drawFlat(0, 0);
                break;
            case ShaderImageBackgroundTypes::PORTION:
                drawFlatPortion();
                break;
            case ShaderImageBackgroundTypes::CENTER:
                int x = EngineSetup::get()->screenWidth/2 - image->width()/2;
                int y = EngineSetup::get()->screenHeight/2 - image->height()/2;
                image->drawFlat(x, y);
                break;
        }
    }

    void setType(int type) {
        this->type = type;
    }

    int getType() const {
        return this->type;
    }

    void setupFlatPortion(int xDrawPos, int yDrawPos, int xImage, int yImage, int wImage, int hImage) {

        this->xDrawPos = xDrawPos;
        this->yDrawPos = yDrawPos;
        this->xImage = xImage;
        this->yImage = yImage;
        this->wImage = wImage;
        this->hImage = hImage;

        if (this->yImage > image->height() - hImage) {
            //this->yImage = image->height() - hImage;
            this->autoScrollCursorY = 0;
        }
    }

    void drawFlatPortion() {

        auto *buffer = EngineBuffers::getInstance();

        auto *pixels = (Uint32 *) image->pixels();

        for (int i = 0; i < hImage; i++) {
            for (int j = 0; j < wImage; j++) {
                if (Tools::isPixelInWindow(j, i)) {
                    const int bufferX = xImage + j;
                    const int bufferY = yImage + i;

                    const int bufferIndex = bufferY * image->width() + bufferX;

                    int x = j + xDrawPos;
                    int y = i + yDrawPos;
                    if (Tools::isPixelInWindow(x, y)) {
                        buffer->setVideoBuffer(x, y, pixels[bufferIndex]);

                    }
                }
            }
        }
    }

    Vertex3D &getAutoScrollSpeed() {
        return autoScrollSpeed;
    }

    void setAutoScrollSpeed(const Vertex3D &autoScrollSpeed) {
        ShaderImageBackground::autoScrollSpeed = autoScrollSpeed;
    }

    bool isAutoScrollEnabled() const {
        return autoScrollEnabled;
    }

    void setAutoScrollEnabled(bool autoScrollEnabled) {
        ShaderImageBackground::autoScrollEnabled = autoScrollEnabled;
    }

    int xDrawPos;
    int yDrawPos;
    int xImage;
    int yImage;
    int wImage;
    int hImage;
    bool autoScrollEnabled;

    float autoScrollCursorX = 0;
    float autoScrollCursorY = 0;
private:
    Image* image;
    int type;

};
#endif //BRAKEDA3D_SHADERIMAGEBACKGROUND_H
