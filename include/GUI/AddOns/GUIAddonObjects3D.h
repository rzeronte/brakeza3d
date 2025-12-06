#ifndef BRAKEZA3D_GUIWIDGETOBJECTS3D_H
#define BRAKEZA3D_GUIWIDGETOBJECTS3D_H

#include <string>
#include "../GUI.h"
#include "../GUIManager.h"
#include "../../SceneObjectTypes.h"
#include "../../3D/Object3D.h"

struct AddonAllowedObjects
{
    TypeObject type;
    bool visible;
    GUIType::Sheet iconCoords;

    void Toggle() { visible = !visible; }
    [[nodiscard]] std::string id() const { return std::string("AddonAllowedObjects3D" + std::to_string((int)type)).c_str(); }
};

class GUIManager;

class GUIAddonObjects3D
{
    std::vector<Object3D *> &gameObjects;
    std::vector<AddonAllowedObjects> allowedObjectsToShow;
public:
    explicit GUIAddonObjects3D(std::vector<Object3D *> &gameObjects);
    void drawAllowedObjectsToShow();
    [[nodiscard]] bool isAllowedObjectInConfig(TypeObject typeObject) const;
    void Draw(GUIManager *gui);
};

#endif //BRAKEZA3D_GUIWIDGETOBJECTS3D_H
