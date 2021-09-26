
#include <SDL2/SDL_surface.h>
#include "../headers/EngineBuffers.h"
#include "../headers/Misc/Tools.h"
#include "../headers/Misc/cJSON.h"
#include "../headers/Render/Logging.h"

EngineBuffers *EngineBuffers::instance = 0;

EngineBuffers *EngineBuffers::getInstance() {
    if (instance == 0) {
        instance = new EngineBuffers();
    }

    return instance;
}

EngineBuffers::EngineBuffers() {
    EngineSetup *setup = EngineSetup::getInstance();

    sizeBuffers = setup->RESOLUTION;

    depthBuffer = new float[sizeBuffers];
    videoBuffer = new Uint32[sizeBuffers];

    //make sure the fire buffer is zero in the beginning
    HUDbuffer = new Uint32[sizeBuffers];
    int h = EngineSetup::getInstance()->screenHeight;
    int w = EngineSetup::getInstance()->screenWidth;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int index = y * w + x;
            HUDbuffer[index] = 0;
        }
    }
    soundPackage = new SoundPackage();

    // buffer for fire shader
    int firePixelsBufferSize = setup->FIRE_WIDTH * setup->FIRE_HEIGHT;
    firePixelsBuffer = new int[firePixelsBufferSize];

    // 37 colores * 3 (rgb channels)
    this->makeFireColors();
    this->fireShaderSetup();

    goreDecalTemplates = new Sprite3D();
    goreDecalTemplates->setAutoRemoveAfterAnimation(true);
    goreDecalTemplates->addAnimation(setup->SPRITES_FOLDER + "gore/gore1", 1, 25);
    goreDecalTemplates->addAnimation(setup->SPRITES_FOLDER + "gore/gore2", 1, 25);
    goreDecalTemplates->addAnimation(setup->SPRITES_FOLDER + "gore/gore3", 1, 25);
    goreDecalTemplates->addAnimation(setup->SPRITES_FOLDER + "gore/gore4", 1, 25);
    goreDecalTemplates->addAnimation(setup->SPRITES_FOLDER + "gore/gore5", 1, 25);
    goreDecalTemplates->addAnimation(setup->SPRITES_FOLDER + "gore/gore6", 1, 25);
    goreDecalTemplates->addAnimation(setup->SPRITES_FOLDER + "gore/gore7", 1, 25);
    goreDecalTemplates->addAnimation(setup->SPRITES_FOLDER + "gore/gore8", 1, 25);
    goreDecalTemplates->addAnimation(setup->SPRITES_FOLDER + "gore/gore9", 1, 25);
    goreDecalTemplates->addAnimation(setup->SPRITES_FOLDER + "gore/gore10", 1, 25);
    goreDecalTemplates->addAnimation(setup->SPRITES_FOLDER + "gore/gore11", 1, 25);

    bloodTemplates = new Sprite3D();
    bloodTemplates->addAnimation(setup->SPRITES_FOLDER + "blood1/blood", 19, 25);
    bloodTemplates->addAnimation(setup->SPRITES_FOLDER + "blood2/blood", 18, 25);
    bloodTemplates->addAnimation(setup->SPRITES_FOLDER + "blood2/blood", 13, 25);

    gibsTemplate = new Sprite3D();
    gibsTemplate->addAnimation(setup->SPRITES_FOLDER + "gibs/gibs1", 7, 25);
    gibsTemplate->addAnimation(setup->SPRITES_FOLDER + "gibs/gibs2", 7, 25);
    gibsTemplate->addAnimation(setup->SPRITES_FOLDER + "gibs/gibs3", 8, 25);
}

void EngineBuffers::clearDepthBuffer() {
    std::fill(depthBuffer, depthBuffer + sizeBuffers, 10000);
}

float EngineBuffers::getDepthBuffer(int x, int y) {
    return depthBuffer[y * this->widthVideoBuffer + x];
}

float EngineBuffers::getDepthBuffer(int i) {
    return depthBuffer[i];
}

void EngineBuffers::setDepthBuffer(int x, int y, float value) {
    depthBuffer[y * this->widthVideoBuffer + x] = value;
}

void EngineBuffers::setDepthBuffer(const int i, const float value) {
    depthBuffer[i] = value;
}

void EngineBuffers::setVideoBuffer(const int x, const int y, Uint32 value) {
    videoBuffer[y * this->widthVideoBuffer + x] = value;
}

void EngineBuffers::setVideoBuffer(const int i, Uint32 value) {
    videoBuffer[i] = value;
}

float EngineBuffers::getVideoBuffer(int x, int y) {
    return videoBuffer[y * this->widthVideoBuffer + x];
}

void EngineBuffers::clearVideoBuffer() {
    if (EngineSetup::getInstance()->ENABLE_FOG) {
        std::fill(videoBuffer, videoBuffer + sizeBuffers, EngineSetup::getInstance()->FOG_COLOR);
        return;
    }

    std::fill(videoBuffer, videoBuffer + sizeBuffers, NULL);
}

void EngineBuffers::flipVideoBufferToSurface(SDL_Surface *surface) {
    // buffer -> surface
    memcpy(&surface->pixels, &videoBuffer, sizeof(surface->pixels));
}

void EngineBuffers::makeFireColors() {
    // Populate pallete
    for (int i = 0; i < 111 / 3; i++) {
        fireColors[i] = Tools::createRGB(
                rgbs[i * 3 + 0],
                rgbs[i * 3 + 1],
                rgbs[i * 3 + 2]
        );

        videoBuffer[100 * 320 + i] = fireColors[i];
    }
}

void EngineBuffers::fireShaderSetup() {
    // Set whole screen to 0 (color: 0x07,0x07,0x07)
    int FIRE_WIDTH = EngineSetup::getInstance()->FIRE_WIDTH;
    int FIRE_HEIGHT = EngineSetup::getInstance()->FIRE_HEIGHT;

    int firePixelsBufferSize = FIRE_HEIGHT * FIRE_WIDTH;
    for (int i = 0; i < firePixelsBufferSize; i++) {
        this->firePixelsBuffer[i] = 0;
    }

    // Set bottom line to 37 (color white: 0xFF,0xFF,0xFF)
    for (int i = 0; i < FIRE_WIDTH; i++) {
        this->firePixelsBuffer[(FIRE_HEIGHT - 1) * FIRE_WIDTH + i] = 36;
    }
}

NPCEnemyBody *EngineBuffers::getEnemyTemplateForClassname(std::string classname) {
    for (NPCEnemyBody *e : this->enemyTemplates) {
        if (e->getClassname() == classname) {
            return e;
        }
    }

    return nullptr;
}