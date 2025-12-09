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
