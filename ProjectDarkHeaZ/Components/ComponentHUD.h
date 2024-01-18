//
// Created by darkhead on 12/1/20.
//

#ifndef BRAKEDA3D_COMPONENTHUD_H
#define BRAKEDA3D_COMPONENTHUD_H

#include <SDL2/SDL_events.h>
#include "../../include/Misc/TexturePackage.h"
#include "../../include/Components/Component.h"
#include "../../include/Components/ComponentWindow.h"
#include "../../include/2D/TextWriter.h"
#include "../../include/2D/Button.h"
#include "../../include/Objects/Mesh3D.h"
#include "../FX/FXLaser.h"

class ComponentHUD : public Component {
private:
    std::vector<Button*> buttons;
    FXLaser *shaderLasers;
    TexturePackage *HUDTextures;
public:
    ComponentHUD();

    ~ComponentHUD() override;

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    void loadImages();

    void drawHUD();

    void addButton(Button *button);

    [[nodiscard]] const std::vector<Button *> &getButtons() const;

    void loadButtons();

    void drawIconWeaponsAndLevelName() const;

    int getButtonsOffsetY();

    void drawShaderLasers();

    [[nodiscard]] TexturePackage *getHudTextures() const;

    [[nodiscard]] FXLaser *getShaderLasers() const;

    void drawGhateringResources();

    void drawEnemyIconAndName();

    Point2D globalOffset;
    Point2D offsetIcons;

    Point2D weaponIconOffset;
    Point2D ammoAmountOffset;
    Point2D bombIconOffset;
    Point2D ammoBombOffset;
    Point2D shieldIconOffset;
    Point2D ammoShieldOffset;
    Point2D levelNumberOffset;
    Point2D reflectionIconOffset;
    Point2D ammoReflectionOffset;

    Point2D iconHealthOffset;
    Point2D iconEnergyOffset;
    Point2D resourcesOffset;

    Point2D staminaOffset;
    Point2D energyOffset;
    Point2D staminaEnemyOffset;

    Point2D avatarEnemyOffset;

    float strokeBars = 9.0f;
    float intensityBars = 0.001f;
};


#endif //BRAKEDA3D_COMPONENTHUD_H
