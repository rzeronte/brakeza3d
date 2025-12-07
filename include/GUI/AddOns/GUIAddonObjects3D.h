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

    static bool exist(std::string pattern1, std::string pattern2);

    static void DrawObjectsTree(GUIManager * gui, const std::vector<Object3D *> & objects, int selectedIndex, std::string filter = "");
    static void DrawObjectList(GUIManager *gui, std::vector<Object3D *> &objects, int selectedObjectIndex, std::string filter);
    static void DrawSceneObjects(GUIManager *gui);
    static void DrawItem(int i, Object3D* o, const std::vector<Object3D *> &objects, int selectedIndex, bool icon);
    static void DrawObjectTypes(GUIManager *gui);
    static bool isObjectTypeVisible(GUIManager *gui, TypeObject typeObject);

};

#endif //BRAKEZA3D_GUIWIDGETOBJECTS3D_H
