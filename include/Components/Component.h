/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEDA3D_COMPONENT_H
#define BRAKEDA3D_COMPONENT_H

#include <SDL2/SDL.h>
#include "../3D/Object3D.h"
#include "../Config.h"

class Components;

class Component {
    int id = -1;
    std::string label;
    bool enabled = false;
    float deltaTime = 0;
protected:
    Config *SETUP;
public:
    Component();
    virtual ~Component();
    void setId(int newId);
    [[nodiscard]] const std::string &getLabel() const;
    void setLabel(const std::string &newLabel);
    virtual void onStart();
    virtual void preUpdate();
    virtual void onUpdate();
    virtual void postUpdate();
    virtual void onEnd() = 0;
    virtual void onSDLPollEvent(SDL_Event *event, bool &finish) = 0;
    [[nodiscard]] bool isEnabled() const;
    virtual void setEnabled(bool value);
};


#endif //BRAKEDA3D_COMPONENT_H
