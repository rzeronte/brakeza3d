//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENT_H
#define BRAKEDA3D_COMPONENT_H

#include <SDL.h>
#include <vector>
#include "../Objects/Object3D.h"

class Component {
private:
    std::vector<Object3D*>   *sceneObjects;

public:
    virtual void onStart() = 0;
    virtual void preUpdate() = 0;
    virtual void onUpdate() = 0;
    virtual void postUpdate() = 0;
    virtual void onEnd() = 0;
    virtual void onSDLPollEvent(SDL_Event *event, bool &finish) = 0;

    std::vector<Component *> *getComponents() const;
    void setComponents(std::vector<Component *> *components);
    Component *getComponentId(int id);

    std::vector<Object3D *> *getSceneObjects() const;
    void setSceneObjects(std::vector<Object3D *> *sceneObjects);

    std::vector<Component *> *components;
};


#endif //BRAKEDA3D_COMPONENT_H
