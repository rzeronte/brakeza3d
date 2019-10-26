
#include <SDL_surface.h>
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Render/Timer.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Engine.h"
#include "../../headers/Objects/Decal.h"

EngineBuffers* EngineBuffers::instance = 0;

EngineBuffers* EngineBuffers::getInstance()
{
    if (instance == 0) {
        instance = new EngineBuffers();
    }

    return instance;
}

EngineBuffers::EngineBuffers()
{
    sizeBuffers = EngineSetup::getInstance()->screenWidth * EngineSetup::getInstance()->screenHeight;

    depthBuffer = new float[sizeBuffers];
    videoBuffer = new Uint32[sizeBuffers];

    // buffer for fire shader
    int firePixelsBufferSize = EngineSetup::getInstance()->FIRE_WIDTH * EngineSetup::getInstance()->FIRE_HEIGHT;
    firePixelsBuffer = new int[firePixelsBufferSize];

    //fragmentsBuffer = new pixelFragment[EngineSetup::get()->ENGINE_MAX_FRAGMENTS];
    OCLTrianglesBuffer = new OCLTriangle[EngineSetup::getInstance()->ENGINE_MAX_OCLTRIANGLES];

    // 37 colores * 3 (rgb channels)
    this->makeFireColors();
    this->fireShaderSetup();

    goreTemplate = new Sprite3D();
    goreTemplate->setAutoRemoveAfterAnimation(true);
    goreTemplate->addAnimation("gore/gore1", 1, 25);
    goreTemplate->addAnimation("gore/gore2", 1, 25);
    goreTemplate->addAnimation("gore/gore3", 1, 25);
    goreTemplate->addAnimation("gore/gore4", 1, 25);
    goreTemplate->addAnimation("gore/gore5", 1, 25);
    goreTemplate->addAnimation("gore/gore6", 1, 25);
    goreTemplate->addAnimation("gore/gore7", 1, 25);
    goreTemplate->addAnimation("gore/gore8", 1, 25);
    goreTemplate->addAnimation("gore/gore9", 1, 25);
    goreTemplate->addAnimation("gore/gore10", 1, 25);
    goreTemplate->addAnimation("gore/gore11", 1, 25);

    gibsTemplate = new Sprite3D();
    gibsTemplate->addAnimation("gibs/gibs1", 7, 25);
    gibsTemplate->addAnimation("gibs/gibs2", 7, 25);
    gibsTemplate->addAnimation("gibs/gibs3", 8, 25);
}

void EngineBuffers::clearDepthBuffer()
{
    std::fill(depthBuffer, depthBuffer + sizeBuffers, 10000);
}

float EngineBuffers::getDepthBuffer(int x, int y)
{
    return depthBuffer[( y * EngineSetup::getInstance()->screenWidth ) + x];
}

float EngineBuffers::getDepthBuffer(int i)
{
    return depthBuffer[i];
}

void EngineBuffers::setDepthBuffer(int x, int y, float value)
{
    depthBuffer[( y * EngineSetup::getInstance()->screenWidth ) + x] = value;
}

void EngineBuffers::setDepthBuffer(const int i, const float value)
{
    depthBuffer[i] = value;
}

void EngineBuffers::setVideoBuffer(const int x, const int y, Uint32 value)
{
    videoBuffer[( y * EngineSetup::getInstance()->screenWidth ) + x ] = value;
}

void EngineBuffers::setVideoBuffer(const int i, Uint32 value)
{
    videoBuffer[ i ] = value;
}

void EngineBuffers::addOCLTriangle(OCLTriangle oclt)
{
    OCLTrianglesBuffer[numOCLTriangles] = oclt;
    numOCLTriangles++;
}

float EngineBuffers::getVideoBuffer(int x, int y)
{
    return videoBuffer[( y * EngineSetup::getInstance()->screenWidth ) + x ];
}

void EngineBuffers::clearVideoBuffer()
{
    std::fill(videoBuffer, videoBuffer + sizeBuffers, NULL);
}

void EngineBuffers::flipVideoBuffer(SDL_Surface *surface)
{
    memcpy (&surface->pixels, &videoBuffer, sizeof(surface->pixels));
}

void EngineBuffers::makeFireColors()
{
    // Populate pallete
    for (int i = 0; i < 111 / 3; i++) {
        fireColors[i] = Tools::createRGB(
            rgbs[i * 3 + 0],
            rgbs[i * 3 + 1],
            rgbs[i * 3 + 2]
        );

        videoBuffer[100 * 320 +i] = fireColors[i];
    }
}

void EngineBuffers::fireShaderSetup()
{
    // Set whole screen to 0 (color: 0x07,0x07,0x07)
    int FIRE_WIDTH  = EngineSetup::getInstance()->FIRE_WIDTH;
    int FIRE_HEIGHT = EngineSetup::getInstance()->FIRE_HEIGHT;

    int firePixelsBufferSize = FIRE_HEIGHT * FIRE_WIDTH;
    for(int i = 0 ; i < firePixelsBufferSize ; i++) {
        this->firePixelsBuffer[i] = 0;
    }

    // Set bottom line to 37 (color white: 0xFF,0xFF,0xFF)
    for(int i = 0; i < FIRE_WIDTH; i++) {
        this->firePixelsBuffer[ (FIRE_HEIGHT-1) * FIRE_WIDTH + i] = 36;
    }
}

void EngineBuffers::loadWAVs()
{
    this->snd_base_menu    = Mix_LoadMUS((EngineSetup::getInstance()->SOUNDS_FOLDER + EngineSetup::getInstance()->SOUND_MAINMENU).c_str() );
    this->snd_base_level_0 = Mix_LoadMUS((EngineSetup::getInstance()->SOUNDS_FOLDER + EngineSetup::getInstance()->SOUND_BASE_LEVEL_0).c_str() );

    this->soundEnemyDead = Mix_LoadWAV((EngineSetup::getInstance()->SOUNDS_FOLDER + EngineSetup::getInstance()->SOUND_ENEMY_DEAD).c_str() );
}

NPCEnemyBody* EngineBuffers::getEnemyTemplateForClassname(std::string classname)
{
    for ( NPCEnemyBody *e : this->enemyTemplates) {
        if (e->getClassname() == classname) {
            return e;
        }
    }

    return nullptr;
}