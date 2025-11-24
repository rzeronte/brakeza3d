#ifndef BRAKEDA3D_COMPONENT_H
#define BRAKEDA3D_COMPONENT_H

#include <SDL2/SDL.h>
#include "../Objects/Object3D.h"
#include "../BrakezaSetup.h"

class ComponentsManager;

class Component {
    int id = -1;
    std::string label;
    bool enabled = false;
protected:
    BrakezaSetup *SETUP;
public:
    Component();
    void setId(int newId);
    [[nodiscard]] const std::string &getLabel() const;
    void setLabel(const std::string &newLabel);
    virtual void onStart() = 0;
    virtual void preUpdate() = 0;
    virtual void onUpdate() = 0;
    virtual void postUpdate() = 0;
    virtual void onEnd() = 0;
    virtual void onSDLPollEvent(SDL_Event *event, bool &finish) = 0;
    [[nodiscard]] bool isEnabled() const;
    virtual void setEnabled(bool value);
    virtual ~Component();
};


#endif //BRAKEDA3D_COMPONENT_H
