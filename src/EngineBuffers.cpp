
#include <SDL_surface.h>
#include "../headers/EngineBuffers.h"
#include "../headers/Misc/Tools.h"
#include "../headers/Misc/cJSON.h"
#include "../headers/Render/Logging.h"

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
    EngineSetup *setup = EngineSetup::getInstance();

    sizeBuffers = setup->RESOLUTION;

    depthBuffer = new float[sizeBuffers];
    videoBuffer = new Uint32[sizeBuffers];

    soundPackage = new SoundPackage();

    // buffer for fire shader
    int firePixelsBufferSize = setup->FIRE_WIDTH * setup->FIRE_HEIGHT;
    firePixelsBuffer = new int[firePixelsBufferSize];

    OCLTrianglesBuffer = new OCLTriangle[EngineSetup::getInstance()->ENGINE_MAX_OCLTRIANGLES];

    // Load WAVs
    this->getSoundsJSON();

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
    depthBuffer[ ( y * EngineSetup::getInstance()->screenWidth ) + x ] = value;
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
    if (EngineSetup::getInstance()->ENABLE_FOG) {
        std::fill(videoBuffer, videoBuffer + sizeBuffers, EngineSetup::getInstance()->FOG_COLOR);
        return;
    }

    std::fill(videoBuffer, videoBuffer + sizeBuffers, NULL);
}

void EngineBuffers::flipVideoBufferToSurface(SDL_Surface *surface)
{
    // buffer -> surface
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

NPCEnemyBody* EngineBuffers::getEnemyTemplateForClassname(std::string classname)
{
    for ( NPCEnemyBody *e : this->enemyTemplates) {
        if (e->getClassname() == classname) {
            return e;
        }
    }

    return nullptr;
}

void EngineBuffers::getSoundsJSON()
{
    std::string sndPath = EngineSetup::getInstance()->SOUNDS_FOLDER;
    size_t file_size;
    std::string filePath = EngineSetup::getInstance()->CONFIG_FOLDER + EngineSetup::getInstance()->CFG_SOUNDS;
    const char* mapsFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);

    if (myDataJSON == NULL) {
        Logging::getInstance()->Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    // Sounds
    cJSON *soundsJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "sounds" );
    int sizeSoundsList = cJSON_GetArraySize(soundsJSONList);

    cJSON *currentSound;
    cJSON_ArrayForEach(currentSound, soundsJSONList) {
        cJSON *file = cJSON_GetObjectItemCaseSensitive(currentSound, "file");
        cJSON *label = cJSON_GetObjectItemCaseSensitive(currentSound, "label");
        cJSON *type = cJSON_GetObjectItemCaseSensitive(currentSound, "type");

        SoundPackageItemType selectedType;
        if (strcmp(type->valuestring, "music") == 0) selectedType = SoundPackageItemType::MUSIC;
        if (strcmp(type->valuestring, "sound") == 0) selectedType = SoundPackageItemType::SOUND;

        soundPackage->addItem(sndPath + file->valuestring, label->valuestring, selectedType);
    }
}
