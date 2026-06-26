#ifndef BRAKEZA3D_SELECTIONMANAGER_H
#define BRAKEZA3D_SELECTIONMANAGER_H

#include <vector>
#include <string>
#include <SDL2/SDL_events.h>

class Object3D;

class SelectionManager {
    std::vector<Object3D*> selectedObjects;
    Object3D* lastRightClickedObject = nullptr;
    std::string lastRightClickedSubmeshName;
    Object3D* lastLeftClickedObject = nullptr;
    std::string lastLeftClickedSubmeshName;

    bool isRectSelecting   = false;
    int rectSelectStartX   = 0;
    int rectSelectStartY   = 0;
    int rectSelectCurrentX = 0;
    int rectSelectCurrentY = 0;


    bool pendingLeftClick        = false;
    bool pendingClickCtrl        = false;
    Object3D* pendingClickObject = nullptr;  // copia interna, no afectada por clearLeftClickedObject

public:
    void setSelectedObject(Object3D *o);
    void addToSelection(Object3D *o);
    void removeFromSelection(const Object3D *o);
    void clearSelection();

    [[nodiscard]] bool isObjectInSelection(const Object3D *o) const;
    [[nodiscard]] bool hasMultipleSelected() const                      { return selectedObjects.size() > 1; }
    [[nodiscard]] Object3D* getSelectedObject() const                   { return selectedObjects.size() == 1 ? selectedObjects[0] : nullptr; }
    [[nodiscard]] const std::vector<Object3D*>& getSelectedObjects() const { return selectedObjects; }
    [[nodiscard]] Object3D* getLastRightClickedObject() const;
    [[nodiscard]] std::string getLastRightClickedSubmeshName() const    { return lastRightClickedSubmeshName; }
    [[nodiscard]] Object3D* getLastLeftClickedObject() const;
    [[nodiscard]] std::string getLastLeftClickedSubmeshName() const     { return lastLeftClickedSubmeshName; }
    [[nodiscard]] bool isObjectAlive(Object3D* o) const;
    void clearRightClickedObject()                                      { lastRightClickedObject = nullptr; }
    void clearRightClickedSubmeshName()                                 { lastRightClickedSubmeshName.clear(); }
    void clearLeftClickedObject()                                       { lastLeftClickedObject = nullptr; lastLeftClickedSubmeshName.clear(); }

    void processSDLEvent(SDL_Event *event);
    void update();
    void DrawSelectionBox() const;
    void DrawSelectionRectFill() const;
};

#endif //BRAKEZA3D_SELECTIONMANAGER_H
