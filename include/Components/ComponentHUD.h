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
#include "../../darkheaz/src/shaders/ShaderHorizontalBar.h"
#include "../../darkheaz/src/shaders/ShaderLasers.h"

class ComponentHUD : public Component {
    std::vector<Button*> buttons;
    ShaderLasers *shaderLasers;
public:

    ComponentHUD();

    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    TexturePackage *HUDTextures;
    TextWriter *textWriter;
    void loadImages();

    void drawHUD();

    void addButton(Button *button);

    [[nodiscard]] const std::vector<Button *> &getButtons() const;

    void loadButtons();

    void drawIconWeaponAndLevelName();

    int getButtonsOffsetY();

    void drawShaderLasers();

    TextWriter *getTextWriter() const;

};


#endif //BRAKEDA3D_COMPONENTHUD_H
