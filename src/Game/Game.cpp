#include <vector>
#include "../../headers/Game/Game.h"
#include "../../headers/Render/Maths.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "../../headers/Physics/Sprite3DBody.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndBSPMap.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndNPCEnemy.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncDoor.h"
#include "../../headers/Collisions/CollisionResolverBetweenCamera3DAndFuncButton.h"
#include "../../headers/Collisions/CollisionResolverBetweenEnemyPartAndBSPMap.h"
#include "../../headers/Collisions/CollisionResolverBetweenProjectileAndPlayer.h"
#include "../../headers/Render/Drawable.h"
#include <thread>

Game* Game::instance = 0;

Game::Game()
{
    player      = new Player();
    controller  = new GameInputController( player );
    HUDTextures = new TexturePackage();

    loadHUDImages();

    Brakeza3D::get()->setController( controller );
}

Game* Game::get()
{
    if (instance == 0) {
        instance = new Game();
    }

    return instance;
}

void Game::start()
{
    onStart();
    mainLoop();
    onEnd();
}

void Game::onStart()
{
    Engine::onStart();

    // Load JSON Config
    Brakeza3D::get()->getMenuManager()->getOptionsJSON();
    this->getWeaponsJSON();
    this->getMapsJSON();
    this->getEnemiesJSON();

    Mix_PlayMusic( EngineBuffers::getInstance()->soundPackage->getMusicByLabel("musicMainMenu"), -1 );
}

void Game::mainLoop()
{
    ImGuiIO& io = ImGui::GetIO();

    while(!finish) {

        this->preUpdate();

        this->onUpdateInputController();

        if (!finish) {

            // game level update
            this->onUpdate();

            // Update window
            Engine::updateWindow();
        }
    }
}

void Game::onUpdate()
{
    // Core onUpdate
    Engine::onUpdate();

    if (player->state != PlayerState::GAMEOVER) {
        if (Brakeza3D::get()->getBSP()->isCurrentLeafLiquid()) {
            player->setAir( player->getAir()-0.5);
        } else {
            player->setAir( 100 );
        }

        if (Brakeza3D::get()->getBSP()->isCurrentLeafLiquid() && Brakeza3D::get()->getBSP()->currentLeaf->type == -5) {
            player->takeDamage( 5 );
        }

        if (EngineSetup::getInstance()->ENABLE_IA) {
            onUpdateIA();
        }

        if (EngineSetup::getInstance()->DRAW_WEAPON) {
            Brakeza3D::get()->getWeaponsManager()->onUpdate(Brakeza3D::get()->getCamera(), Brakeza3D::get()->screenSurface);
        }

        if (EngineSetup::getInstance()->DRAW_HUD) {
            drawHUD();
        }

        if (EngineSetup::getInstance()->DRAW_CROSSHAIR) {
            Drawable::drawCrossHair();
        }

        if (player->isDead()) {
            redScreen();
        }

        if (player->tookDamage) {
            redScreen();
        }

    }

    if (EngineSetup::getInstance()->MENU_ACTIVE) {
        drawMenuScreen();
        Drawable::drawFireShader();
    }

    if (EngineSetup::getInstance()->LOADING) {
        SDL_BlitSurface(this->HUDTextures->getTextureByLabel("loading")->getSurface(1), NULL, Brakeza3D::get()->screenSurface, NULL);
        Tools::writeTextCenterHorizontal(Brakeza3D::get()->renderer, Brakeza3D::get()->fontDefault, Color::red(), "Loading...", 100);
        Drawable::drawFireShader();
    }

    if (EngineSetup::getInstance()->SPLASHING) {
        SDL_BlitSurface(this->HUDTextures->getTextureByLabel("splash")->getSurface(1), NULL, Brakeza3D::get()->screenSurface, NULL);
        Drawable::drawFireShader();

        if (Brakeza3D::get()->splashCounter->isFinished()) {
            Brakeza3D::get()->splashCounter->setEnabled(true);

            EngineSetup::getInstance()->SPLASHING = false;
            EngineSetup::getInstance()->MENU_ACTIVE = true;
            EngineSetup::getInstance()->FADEIN = true;
            EngineSetup::getInstance()->FADEOUT = false;
        }
        if (Brakeza3D::get()->splashCounter->getAcumulatedTime() > Brakeza3D::get()->splashCounter->getStep() * 0.75) {
            EngineSetup::getInstance()->FADEOUT = true;
            EngineSetup::getInstance()->FADEIN = false;
        }
    }

    if ( EngineSetup::getInstance()->FADEOUT ) {
        Drawable::drawFadeOut();
    }

    if ( EngineSetup::getInstance()->FADEIN ) {
        Drawable::drawFadeIn();
    }
}

void Game::preUpdate()
{
    player->evalStatusMachine();

    Engine::preUpdate();
}

void Game::onEnd()
{
    Engine::onEnd();
    Close();
}

void Game::onUpdateInputController()
{
    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL2_ProcessEvent(&e);

        Brakeza3D::get()->getController()->updateKeyboardMapping();
        Brakeza3D::get()->getController()->updateMouseStates(&this->e);

        if (EngineSetup::getInstance()->CAMERA_MOUSE_ROTATION) {
            Brakeza3D::get()->getController()->handleMouse(&this->e);
        }
        Brakeza3D::get()->getController()->handleKeyboard(&this->e, this->finish);
    }

    // Check array Uint8 *keyboard
    Brakeza3D::get()->getController()->handleKeyboardContinuous(&this->e, this->finish);
}

void Game::drawHUD()
{
    SDL_Rect r;
    float iconsY = 215;
    float textY  = 212;
    // HUD Base
    r.x = 0; r.y = EngineSetup::getInstance()->screenHeight - this->HUDTextures->getTextureByLabel("hud")->getSurface(1)->h;
    SDL_BlitSurface(this->HUDTextures->getTextureByLabel("hud")->getSurface(1), NULL, Brakeza3D::get()->screenSurface, &r);

    // Ammo
    r.x = 7; r.y = iconsY;
    SDL_BlitSurface(this->HUDTextures->getTextureByLabel("ammo")->getSurface(1), NULL, Brakeza3D::get()->screenSurface, &r);
    Tools::writeText(Brakeza3D::get()->renderer, Brakeza3D::get()->fontDefault, 25, textY, Color::gray(), std::to_string(Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->ammo));

    // Stamina
    r.x = 57; r.y = iconsY;
    SDL_BlitSurface(this->HUDTextures->getTextureByLabel("health")->getSurface(1), NULL, Brakeza3D::get()->screenSurface, &r);
    Tools::writeText(Brakeza3D::get()->renderer, Brakeza3D::get()->fontDefault, 78, textY, Color::gray(), std::to_string(this->player->getStamina()));

    // Weapon Icon
    r.x = 183   ; r.y = iconsY-2;
    SDL_BlitSurface(Brakeza3D::get()->getWeaponsManager()->getCurrentWeaponType()->iconHUD, NULL, Brakeza3D::get()->screenSurface, &r);

}

void Game::onUpdateIA()
{
    if (player->isDead()) return;

    std::vector<Object3D *>::iterator itObject3D;
    for ( itObject3D = Brakeza3D::get()->getSceneObjects().begin(); itObject3D != Brakeza3D::get()->getSceneObjects().end(); itObject3D++) {
        Object3D *object = *(itObject3D);

        auto *enemy = dynamic_cast<NPCEnemyBody*> (object);

        if (enemy != NULL) {
            enemy->updateCounters();

            if (!Brakeza3D::get()->getCamera()->frustum->isPointInFrustum(*object->getPosition())) {
                continue;
            }

            if (enemy->isDead()) {
                continue;
            }

            Vertex3D A = *object->getPosition();
            Vertex3D B = *Brakeza3D::get()->getCamera()->getPosition();

            Vector3D ray = Vector3D(A, B);


            bool rayCastResult = Brakeza3D::get()->getBSP()->recastWrapper->rayCasting(A, B);

            if (enemy->counterIA->isFinished()) {
                Brakeza3D::get()->getBSP()->recastWrapper->getPathBetween(A, B, enemy->points);
            }

            enemy->evalStatusMachine(
                    rayCastResult,
                    ray.getComponent().getModule(),
                    Brakeza3D::get()->getCamera(),
                    Brakeza3D::get()->getCollisionManager()->getDynamicsWorld(),
                    Brakeza3D::get()->getSceneObjects()
            );
        }
    }
}

void Game::resolveCollisions()
{
    CollisionsManager *cm = Brakeza3D::get()->getCollisionManager();

    std::vector<CollisionResolver *>::iterator itCollision;
    for ( itCollision = cm->getCollisions().begin(); itCollision != cm->getCollisions().end(); itCollision++) {
        CollisionResolver *collision = *(itCollision);
        int collisionType = collision->getTypeCollision();

        if (!collisionType) continue;

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_BSPMAP ) {
            auto *resolver = new CollisionResolverBetweenProjectileAndBSPMap(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getGameObjects(),
                    cm->getDynamicsWorld(),
                    cm->getWeaponManager(),
                    cm->getVisibleTriangles()
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_NPCENEMY ) {
            auto *resolver = new CollisionResolverBetweenProjectileAndNPCEnemy(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getGameObjects(),
                    cm->getDynamicsWorld(),
                    cm->getWeaponManager(),
                    cm->getVisibleTriangles()
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCDOOR ) {
            auto *resolver = new CollisionResolverBetweenCamera3DAndFuncDoor(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getGameObjects(),
                    cm->getVisibleTriangles()
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_CAMERA_AND_FUNCBUTTON ) {
            auto *resolver = new CollisionResolverBetweenCamera3DAndFuncButton(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getGameObjects(),
                    cm->getVisibleTriangles()
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_NPCENEMYPART_AND_BSPMAP ) {
            auto *resolver = new CollisionResolverBetweenEnemyPartAndBSPMap(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getGameObjects(),
                    cm->getDynamicsWorld(),
                    cm->getWeaponManager(),
                    cm->getVisibleTriangles()
            );
            resolver->dispatch();
            continue;
        }

        if ( collisionType == EngineSetup::getInstance()->CollisionResolverTypes::COLLISION_RESOLVER_PROJECTILE_AND_CAMERA ) {
            auto *resolver = new CollisionResolverBetweenProjectileAndPlayer(
                    collision->contactManifold,
                    collision->objA,
                    collision->objB,
                    cm->getBspMap(),
                    cm->getGameObjects(),
                    cm->getDynamicsWorld(),
                    cm->getWeaponManager(),
                    cm->getVisibleTriangles(),
                    player
            );
            resolver->dispatch();
            continue;
        }
    }

    cm->getCollisions().clear();
}

void Game::redScreen()
{
    float intensity_r = 1;
    float intensity_g = 0.5;
    float intensity_b = 0.5;

    for (int y = 0; y < EngineSetup::getInstance()->screenHeight; y++) {
        for (int x = 0; x < EngineSetup::getInstance()->screenWidth; x++) {
            Uint32 currentPixelColor = EngineBuffers::getInstance()->getVideoBuffer(x, y);

            int r_light = (int) (Tools::getRedValueFromColor(currentPixelColor) * intensity_r);
            int g_light = (int) (Tools::getGreenValueFromColor(currentPixelColor) * intensity_g);
            int b_light = (int) (Tools::getBlueValueFromColor(currentPixelColor) * intensity_b);

            currentPixelColor = Tools::createRGB(r_light, g_light, b_light);
            EngineBuffers::getInstance()->setVideoBuffer( x, y, currentPixelColor );
        }
    }
}

void Game::drawMenuScreen()
{
    if (EngineSetup::getInstance()->MENU_ACTIVE) {
        Brakeza3D::get()->getMenuManager()->drawOptions(Brakeza3D::get()->screenSurface);
    }
}

void Game::loadHUDImages()
{
    HUDTextures->addItem(EngineSetup::getInstance()->HUD_FOLDER + "splash.png", "splash");
    HUDTextures->addItem(EngineSetup::getInstance()->HUD_FOLDER + "hud_health.png", "health");
    HUDTextures->addItem(EngineSetup::getInstance()->HUD_FOLDER + "hud_ammo.png", "ammo");
    HUDTextures->addItem(EngineSetup::getInstance()->HUD_FOLDER + "hud.png", "hud");
    HUDTextures->addItem(EngineSetup::getInstance()->HUD_FOLDER + "loading.png", "loading");
}

void Game::getWeaponsJSON()
{
    size_t file_size;
    std::string filePath = EngineSetup::getInstance()->CONFIG_FOLDER + EngineSetup::getInstance()->CFG_WEAPONS;
    const char* mapsFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);

    if (myDataJSON == NULL) {
        Logging::getInstance()->Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    weaponsJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "weapons" );
    int sizeWeaponsList = cJSON_GetArraySize(weaponsJSONList);

    if (sizeWeaponsList > 0) {
        Logging::getInstance()->Log(filePath + " have " + std::to_string(sizeWeaponsList) + " weapons", "WEAPONS");
    } else {
        Logging::getInstance()->Log(filePath + " is empty", "ERROR");
    }

    // weapons loop
    cJSON *currentWeapon;
    cJSON_ArrayForEach(currentWeapon, weaponsJSONList) {
        cJSON *name        = cJSON_GetObjectItemCaseSensitive(currentWeapon, "name");
        cJSON *damage      = cJSON_GetObjectItemCaseSensitive(currentWeapon, "damage");
        cJSON *speed       = cJSON_GetObjectItemCaseSensitive(currentWeapon, "speed");
        cJSON *startAmmo   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "startAmmo");
        cJSON *projectileW = cJSON_GetObjectItemCaseSensitive(currentWeapon, "projectile_width");
        cJSON *projectileH = cJSON_GetObjectItemCaseSensitive(currentWeapon, "projectile_height");
        cJSON *soundFire   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "fire_sound");
        cJSON *soundMark   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "mark_sound");
        cJSON *accuracy    = cJSON_GetObjectItemCaseSensitive(currentWeapon, "accuracy");
        cJSON *dispersion  = cJSON_GetObjectItemCaseSensitive(currentWeapon, "dispersion");
        cJSON *iconHUD     = cJSON_GetObjectItemCaseSensitive(currentWeapon, "icon_hud");
        cJSON *sniper      = cJSON_GetObjectItemCaseSensitive(currentWeapon, "sniper");

        Logging::getInstance()->Log("Loading weapon " + std::string(name->valuestring), "WEAPONS");

        cJSON *keyDownHandle  = cJSON_GetObjectItemCaseSensitive(currentWeapon, "key_down_handle");
        cJSON *keyUpHandle    = cJSON_GetObjectItemCaseSensitive(currentWeapon, "key_up_handle");

        cJSON *keyDownAnimationStatus = cJSON_GetObjectItemCaseSensitive(currentWeapon, "key_down_animation_status");
        cJSON *keyUpAnimationStatus   = cJSON_GetObjectItemCaseSensitive(currentWeapon, "key_up_animation_status");

        // Weapon Type attributes
        Brakeza3D::get()->getWeaponsManager()->addWeaponType(name->valuestring);
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setAmmo( startAmmo->valueint );
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setDamage(damage->valueint);
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setSpeed((float)speed->valuedouble);
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setProjectileSize(projectileW->valuedouble, projectileH->valuedouble);
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->loadFireSound(soundFire->valuestring);
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->loadMarkSound(soundMark->valuestring);
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setAccuracy(accuracy->valuedouble);
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setDispersion(dispersion->valueint);
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->loadIconHUD(std::string(name->valuestring) + "/" + std::string(iconHUD->valuestring));

        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setKeyDownHandle(keyDownHandle->valueint );
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setKeyUpHandle(keyUpHandle->valueint );
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setKeyDownAnimationStatus( keyDownAnimationStatus->valueint );
        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setKeyUpAnimationStatus( keyUpAnimationStatus->valueint) ;

        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setSniper( sniper->valueint );

        cJSON *fireMark =   cJSON_GetObjectItemCaseSensitive(currentWeapon, "fire_mark");
        cJSON *markPath =   cJSON_GetObjectItemCaseSensitive(fireMark, "path");
        cJSON *markFrames = cJSON_GetObjectItemCaseSensitive(fireMark, "frames");
        cJSON *markFps =    cJSON_GetObjectItemCaseSensitive(fireMark, "fps");
        cJSON *markW =      cJSON_GetObjectItemCaseSensitive(fireMark, "width");
        cJSON *markH =      cJSON_GetObjectItemCaseSensitive(fireMark, "height");

        Logging::getInstance()->Log("Creating weapon mark billboard for " + std::string(name->valuestring), "WEAPONS");

        Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->setupMarkTemplate(
                markPath->valuestring,
                markFrames->valueint,
                markFps->valueint,
                (float) markW->valuedouble,
                (float) markH->valuedouble
        );

        Logging::getInstance()->Log("JSON Weapon mark details for : " + std::string(name->valuestring) +
                                    ", path: " + markPath->valuestring +
                                    ", frames: " + std::to_string(markFrames->valueint) +
                                    ", fps: " + std::to_string(markFps->valueint) +
                                    ", w: " + std::to_string(markW->valuedouble) + ", h: " + std::to_string(markH->valuedouble)
        );

        // animation's weapon loop
        cJSON *weaponAnimationsJSONList;
        weaponAnimationsJSONList = cJSON_GetObjectItemCaseSensitive(currentWeapon, "animations" );

        cJSON *currentWeaponAnimation;
        cJSON_ArrayForEach(currentWeaponAnimation, weaponAnimationsJSONList) {
            cJSON *status    = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "status");
            cJSON *subfolder = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "subfolder");
            cJSON *frames    = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "frames");
            cJSON *fps       = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "fps");
            cJSON *offsetX   = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "offsetX");
            cJSON *offsetY   = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "offsetY");
            cJSON *right     = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "right");
            cJSON *stopEnd   = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "stop_end");
            cJSON *time      = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "time");
            cJSON *sound     = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "sound");
            cJSON *looping   = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "looping");
            cJSON *next      = cJSON_GetObjectItemCaseSensitive(currentWeaponAnimation, "next");

            Logging::getInstance()->Log("Loading JSON Weapon Animation: " + std::string(subfolder->valuestring) + ", status:" + std::to_string(status->valueint) +", next: " + std::to_string(next->valueint));

            Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->addAnimation(
                    std::string(name->valuestring) + "/" + std::string(subfolder->valuestring),
                    frames->valueint,
                    fps->valueint,
                    offsetX->valueint,
                    offsetY->valueint,
                    right->valueint,
                    stopEnd->valueint,
                    next->valueint,
                    looping->valueint
            );

            Counter animationCounter = Counter();
            animationCounter.setStep( time->valuedouble );
            animationCounter.setEnabled( false );
            Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->fireCounters.push_back(animationCounter);
            Logging::getInstance()->Log("Adding fire animation timer with step: " + std::to_string(time->valuedouble));

            Mix_Chunk *animationSound = new Mix_Chunk();

            std::string pathSound = EngineSetup::getInstance()->SPRITES_FOLDER + name->valuestring + "/sounds/" + sound->valuestring;

            if (strlen(sound->valuestring) > 0) {
                Logging::getInstance()->Log("Loading Fire Phases sound: " + pathSound);
                animationSound = Mix_LoadWAV( (pathSound).c_str() );
            } else {
                Logging::getInstance()->Log("Loading Fire Phases sound: Empty");
            }

            Brakeza3D::get()->getWeaponsManager()->getWeaponTypeByLabel(name->valuestring)->fireSounds.push_back(animationSound);
        }
    }
}

void Game::getEnemiesJSON()
{
    size_t file_size;
    std::string filePath = EngineSetup::getInstance()->CONFIG_FOLDER + EngineSetup::getInstance()->CFG_ENEMIES;
    const char* enemiesFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(enemiesFile);

    if (myDataJSON == NULL) {
        Logging::getInstance()->Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    enemiesJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "enemies" );
    int sizeWeaponsList = cJSON_GetArraySize(enemiesJSONList);

    if (sizeWeaponsList > 0) {
        Logging::getInstance()->Log(filePath + " have " + std::to_string(sizeWeaponsList));
    } else {
        Logging::getInstance()->Log(filePath + " is empty", "ERROR");
    }

    // weapons loop
    cJSON *currentEnemy;
    cJSON_ArrayForEach(currentEnemy, enemiesJSONList) {
        cJSON *name             = cJSON_GetObjectItemCaseSensitive(currentEnemy, "name");
        cJSON *classname        = cJSON_GetObjectItemCaseSensitive(currentEnemy, "classname");
        cJSON *range            = cJSON_GetObjectItemCaseSensitive(currentEnemy, "range");
        cJSON *damage           = cJSON_GetObjectItemCaseSensitive(currentEnemy, "damage");
        cJSON *cadence          = cJSON_GetObjectItemCaseSensitive(currentEnemy, "cadence");
        cJSON *speed            = cJSON_GetObjectItemCaseSensitive(currentEnemy, "speed");
        cJSON *width            = cJSON_GetObjectItemCaseSensitive(currentEnemy, "width");
        cJSON *height           = cJSON_GetObjectItemCaseSensitive(currentEnemy, "height");
        cJSON *projectileW      = cJSON_GetObjectItemCaseSensitive(currentEnemy, "projectile_width");
        cJSON *projectileH      = cJSON_GetObjectItemCaseSensitive(currentEnemy, "projectile_height");
        cJSON *soundFire        = cJSON_GetObjectItemCaseSensitive(currentEnemy, "fire_sound");
        cJSON *soundMark        = cJSON_GetObjectItemCaseSensitive(currentEnemy, "mark_sound");
        cJSON *defaultAnimation = cJSON_GetObjectItemCaseSensitive(currentEnemy, "default_animation");

        NPCEnemyBody *newEnemy = new NPCEnemyBody();
        newEnemy->setDamage( (float) damage->valuedouble );
        newEnemy->setCadence( (float) cadence->valuedouble );
        newEnemy->setRange( (float) range->valuedouble );
        newEnemy->setSpeed( (float) speed->valuedouble );
        newEnemy->setPosition(Vertex3D(0, 0, 0) );
        newEnemy->setRotation(M3() );
        newEnemy->getBillboard()->setDimensions( (float) width->valuedouble, (float) height->valuedouble );
        newEnemy->setLabel( name->valuestring);
        newEnemy->setClassname( classname->valuestring );
        newEnemy->setAnimation( defaultAnimation->valueint );


        Logging::getInstance()->Log("Enemy JSON detected: name: " + std::string(name->valuestring) +
                                    ", classname: " + classname->valuestring +
                                    ", speed: " + std::to_string(speed->valuedouble) +
                                    ", w: " + std::to_string(width->valuedouble) +
                                    ", h: " + std::to_string(height->valuedouble)
        );

        // animation's enemy loop
        cJSON *enemyAnimationsJSONList = cJSON_GetObjectItemCaseSensitive(currentEnemy, "animations" );

        cJSON *currentEnemyAnimation;
        cJSON_ArrayForEach(currentEnemyAnimation, enemyAnimationsJSONList) {
            cJSON *path           = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "path");
            cJSON *frames         = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "frames");
            cJSON *fps            = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "fps");
            cJSON *zeroDirection  = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "zeroDirection");
            cJSON *maxTimes       = cJSON_GetObjectItemCaseSensitive(currentEnemyAnimation, "maxTimes");

            Logging::getInstance()->Log("Reading JSON Enemy Animation: " + std::string(path->valuestring)
                                        + ", maxTimes: " + std::to_string(maxTimes->valueint)
            );

            newEnemy->addAnimationDirectional2D(
                    path->valuestring,
                    frames->valueint,
                    fps->valueint,
                    zeroDirection->valueint,
                    maxTimes->valueint
            );
        }

        EngineBuffers::getInstance()->enemyTemplates.push_back(newEnemy);
    }
}

void Game::getMapsJSON()
{
    size_t file_size;
    const char* mapsFile = Tools::readFile(EngineSetup::getInstance()->CONFIG_FOLDER + EngineSetup::getInstance()->CFG_MAPS, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);
    if (myDataJSON == NULL) {
        Logging::getInstance()->Log("maps.json can't be loaded", "ERROR");
        return;
    }

    cJSON *currentMap = NULL;
    mapsJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "maps" );
    int sizeMaps = cJSON_GetArraySize(mapsJSONList);

    if (sizeMaps <= 0) {
        Logging::getInstance()->Log("maps.json is empty", "ERROR");
    }

    cJSON_ArrayForEach(currentMap, mapsJSONList) {
        cJSON *nameMap = cJSON_GetObjectItemCaseSensitive(currentMap, "name");

        if (cJSON_IsString(nameMap)) {
            Logging::getInstance()->Log("Map JSON detected: " + std::string(nameMap->valuestring));
        }
    }
}

void Game::initBSP()
{
    cJSON *firstMap = cJSON_GetArrayItem(mapsJSONList, 0);
    cJSON *nameMap  = cJSON_GetObjectItemCaseSensitive(firstMap, "name");

    EngineSetup::getInstance()->LOADING     = true;
    EngineSetup::getInstance()->MENU_ACTIVE = false;

    Brakeza3D::get()->initBSP(nameMap->valuestring, &this->frameTriangles);
}
