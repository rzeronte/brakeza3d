#ifndef BRAKEDA3D_COMPONENTSMANAGER_H
#define BRAKEDA3D_COMPONENTSMANAGER_H

#include "Component.h"
#include "ComponentCamera.h"
#include "ComponentCollisions.h"
#include "ComponentRender.h"
#include "ComponentInput.h"
#include "ComponentSound.h"
#include "ComponentWindow.h"
#include "ComponentScripting.h"

enum ComponentID {
    COMPONENT_WINDOW = 0,
    COMPONENT_SCRIPTING = 1,
    COMPONENT_CAMERA = 2,
    COMPONENT_COLLISIONS = 3,
    COMPONENT_INPUT = 4,
    COMPONENT_SOUND = 5,
    COMPONENT_RENDER = 6
};

class Components
{
    std::vector<Component *> components;
    static Components *instance;
public:

    static Components *get();
    virtual ~Components();

    void RegisterComponent(Component *component, const std::string& label);

    ComponentCamera     *Camera()           { return dynamic_cast<ComponentCamera *>     (components[COMPONENT_CAMERA]); }
    ComponentCollisions *Collisions()       { return dynamic_cast<ComponentCollisions *> (components[COMPONENT_COLLISIONS]); }
    ComponentWindow     *Window()           { return dynamic_cast<ComponentWindow *>     (components[COMPONENT_WINDOW]); }
    ComponentRender     *Render()           { return dynamic_cast<ComponentRender *>     (components[COMPONENT_RENDER]); }
    ComponentInput      *Input()            { return dynamic_cast<ComponentInput *>      (components[COMPONENT_INPUT]); }
    ComponentSound      *Sound()            { return dynamic_cast<ComponentSound *>      (components[COMPONENT_SOUND]); }
    ComponentScripting  *Scripting()        { return dynamic_cast<ComponentScripting *>  (components[COMPONENT_SCRIPTING]); }

    std::vector<Component *> getComponents() const { return components; }
};

#endif //BRAKEDA3D_COMPONENTSMANAGER_H
