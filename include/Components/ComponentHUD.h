//
// Created by darkhead on 12/1/20.
//

#ifndef BRAKEDA3D_COMPONENTHUD_H
#define BRAKEDA3D_COMPONENTHUD_H

#include <SDL2/SDL_events.h>
#include "../Misc/TexturePackage.h"
#include "Component.h"
#include "ComponentWindow.h"
#include "../2D/TextWriter.h"
#include "../2D/Button.h"
#include "../Objects/Mesh3D.h"
#include "../../darkheaz/src/shaders/ShaderProjectiles.h"

class ComponentHUD : public Component {
private:
    std::vector<Button*> buttons;
    ShaderProjectiles *shaderLasers;
    TexturePackage *HUDTextures;
    int offsetY = 430;
    int playerBarSeparation = 15;
public:
    ComponentHUD();

    virtual ~ComponentHUD();

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

    void drawIconWeaponAndLevelName();

    int getButtonsOffsetY();

    void drawShaderLasers();

    [[nodiscard]] TexturePackage *getHudTextures() const;
};


#endif //BRAKEDA3D_COMPONENTHUD_H
