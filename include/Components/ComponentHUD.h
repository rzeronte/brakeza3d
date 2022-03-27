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

class ComponentHUD : public Component {
    std::vector<Button*> buttons;
public:

    ComponentHUD();

    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    TexturePackage *HUDTextures;
    TexturePackage *iconsTextures;
    TextWriter *textureWriter;

    void loadImages();

    void writeText(int x, int y, const char *, bool bold) const;

    void writeTextMiddleScreen(const char *, bool bold) const;

    void writeCenterHorizontal(int y, const char *, bool bold) const;

    void drawHUD();

    void addButton(Button *button);

    const std::vector<Button *> &getButtons() const;

    void loadButtons();

    void drawPlayerStamina(int y);

    void drawPlayerEnergy(int y);

    void drawEnemyStamina(int y);

    int getButtonsOffsetY();


};


#endif //BRAKEDA3D_COMPONENTHUD_H
