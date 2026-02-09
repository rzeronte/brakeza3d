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

    [[nodiscard]] ComponentCamera       *Camera() const             { return (ComponentCamera *)     components[COMPONENT_CAMERA]; }
    [[nodiscard]] ComponentCollisions   *Collisions() const         { return (ComponentCollisions *) components[COMPONENT_COLLISIONS]; }
    [[nodiscard]] ComponentWindow       *Window() const             { return (ComponentWindow *)     components[COMPONENT_WINDOW]; }
    [[nodiscard]] ComponentRender       *Render() const             { return (ComponentRender *)     components[COMPONENT_RENDER]; }
    [[nodiscard]] ComponentInput        *Input() const              { return (ComponentInput *)      components[COMPONENT_INPUT]; }
    [[nodiscard]] ComponentSound        *Sound() const              { return (ComponentSound *)      components[COMPONENT_SOUND]; }
    [[nodiscard]] ComponentScripting    *Scripting() const          { return (ComponentScripting *)  components[COMPONENT_SCRIPTING]; }
    [[nodiscard]] std::vector<Component *> getComponents() const    { return components; }
};

#endif //BRAKEDA3D_COMPONENTSMANAGER_H
