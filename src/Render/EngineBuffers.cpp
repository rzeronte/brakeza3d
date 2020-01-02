
#include <SDL_surface.h>
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Render/Tools.h"

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
    this->loadSounds();

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

void EngineBuffers::loadSounds()
{
    std::string sndPath = EngineSetup::getInstance()->SOUNDS_FOLDER;

    soundPackage->addItem(sndPath + "menuMusic.wav", "musicMainMenu", SoundPackageItemType::MUSIC);
    soundPackage->addItem(sndPath + "menuMusic.wav", "musicBaseLevel0", SoundPackageItemType::MUSIC);

    soundPackage->addItem(sndPath + "start_game.wav", "startGame", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "option-click.wav", "soundMenuClick", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "option-accept.wav", "soundMenuAccept", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "enemy_dead.wav", "soundEnemyDead", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "body_explode.wav", "bodyExplode", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "step1.wav", "playerStep1", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "step2.wav", "playerStep2", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "step3.wav", "playerStep3", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "step4.wav", "playerStep4", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "step5.wav", "playerStep5", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "step6.wav", "playerStep6", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "playerDead1.wav", "playerDead1", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerDead2.wav", "playerDead2", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerDead3.wav", "playerDead3", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerDead4.wav", "playerDead4", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerDead5.wav", "playerDead5", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerDead6.wav", "playerDead6", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "playerPain1.wav", "playerPain1", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerPain2.wav", "playerPain2", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerPain3.wav", "playerPain3", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerPain4.wav", "playerPain4", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "playerJump1.wav", "playerJump1", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerJump2.wav", "playerJump2", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerJump3.wav", "playerJump3", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerJump4.wav", "playerJump4", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "playerLand1.wav", "playerLand1", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerLand2.wav", "playerLand2", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerLand3.wav", "playerLand3", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "playerLand4.wav", "playerLand4", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "teleporting.wav", "teleporting", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "enemyRage1.wav", "enemyRage1", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "enemyRage2.wav", "enemyRage2", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "enemyRage3.wav", "enemyRage3", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "enemyRage4.wav", "enemyRage4", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "enemyRage5.wav", "enemyRage5", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "bulletWhisper.wav", "bulletWhisper", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "switch_weapon.wav", "switchWeapon", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "sniperOn.wav", "sniperOn", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "bloodhit1.wav", "bloodHit1", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "bloodhit2.wav", "bloodHit2", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "bloodhit3.wav", "bloodHit3", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "open_door.wav", "openDoor", SoundPackageItemType::SOUND);
    soundPackage->addItem(sndPath + "get_weapon.wav", "getWeapon", SoundPackageItemType::SOUND);

    soundPackage->addItem(sndPath + "first_aid.wav", "firstAid", SoundPackageItemType::SOUND);
}
