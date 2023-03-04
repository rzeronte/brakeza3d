#ifndef BRAKEDA3D_COMPONENT_H
#define BRAKEDA3D_COMPONENT_H

#include <SDL2/SDL.h>
#include <vector>
#include "../Objects/Object3D.h"
#include "../EngineSetup.h"
#include "../EngineBuffers.h"

class ComponentsManager;

class Component {
private:
    int id;
    std::string label;
    bool enabled;
public:

    Component();

    void setId(int newId);

    [[nodiscard]] const std::string &getLabel() const;

    void setLabel(const std::string &newLabel);

    // Life cycle functions
    virtual void onStart() = 0;

    virtual void preUpdate() = 0;

    virtual void onUpdate() = 0;

    virtual void postUpdate() = 0;

    virtual void onEnd() = 0;

    virtual void onSDLPollEvent(SDL_Event *event, bool &finish) = 0;

    EngineBuffers *BUFFERS;
    EngineSetup *SETUP;

    [[nodiscard]] bool isEnabled() const;

    virtual void setEnabled(bool enabled);
};


#endif //BRAKEDA3D_COMPONENT_H
