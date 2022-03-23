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
    std::vector<Object3D *> *sceneObjects;
    int id;
    std::string label;
    bool enabled;
public:

    Component();

    // Identification functions
    int getId() const;

    void setId(int newId);

    const std::string &getLabel() const;

    void setLabel(const std::string &newLabel);

    // Life cycle functions
    virtual void onStart() = 0;

    virtual void preUpdate() = 0;

    virtual void onUpdate() = 0;

    virtual void postUpdate() = 0;

    virtual void onEnd() = 0;

    virtual void onSDLPollEvent(SDL_Event *event, bool &finish) = 0;

    // get another components functions
    std::vector<Component *> *getComponents() const;

    Component *getComponentById(unsigned int componentId) const;

    void setComponents(std::vector<Component *> *newComponent);

    // Accesor to scene objects
    std::vector<Object3D *> *getSceneObjects() const;

    void setSceneObjects(std::vector<Object3D *> *newSceneObjects);

    std::vector<Component *> *components{};

    // Accessors to Setup and Buffers
    EngineBuffers *BUFFERS;
    EngineSetup *SETUP;

    bool isEnabled() const;

    void setEnabled(bool enabled);
};


#endif //BRAKEDA3D_COMPONENT_H
