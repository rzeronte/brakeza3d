#ifndef BRAKEZA3D_GUIWIDGETOBJECTS3D_H
#define BRAKEZA3D_GUIWIDGETOBJECTS3D_H

#include "../GUIManager.h"
#include "../../SceneObjectTypes.h"

class GUIManager;
class Object3D;

class GUIAddonObjects3D
{
public:
    explicit GUIAddonObjects3D();
    static void DrawSceneObjects(GUIManager *gui);
    static void DrawAllowedObjectsToShow(GUIManager *gui);
    static bool isAllowedObjectInConfig(GUIManager *gui, TypeObject typeObject);
};

#endif //BRAKEZA3D_GUIWIDGETOBJECTS3D_H
