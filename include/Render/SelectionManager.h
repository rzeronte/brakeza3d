#ifndef BRAKEZA3D_SELECTIONMANAGER_H
#define BRAKEZA3D_SELECTIONMANAGER_H

#include <vector>
#include <SDL2/SDL_events.h>

class Object3D;
class ShaderOGLLine;

class SelectionManager {
    std::vector<Object3D*> selectedObjects;

    bool isRectSelecting    = false;
    int rectSelectStartX    = 0;
    int rectSelectStartY    = 0;
    int rectSelectCurrentX  = 0;
    int rectSelectCurrentY  = 0;

public:
    void setSelectedObject(Object3D *o);
    void addToSelection(Object3D *o);
    void removeFromSelection(const Object3D *o);
    void clearSelection();

    [[nodiscard]] bool isObjectInSelection(const Object3D *o) const;
    [[nodiscard]] bool hasMultipleSelected() const                      { return selectedObjects.size() > 1; }
    [[nodiscard]] Object3D* getSelectedObject() const                   { return selectedObjects.size() == 1 ? selectedObjects[0] : nullptr; }
    [[nodiscard]] const std::vector<Object3D*>& getSelectedObjects() const { return selectedObjects; }

    void processSDLEvent(SDL_Event *event);
    void DrawSelectionRect(ShaderOGLLine *lineShader) const;
};

#endif //BRAKEZA3D_SELECTIONMANAGER_H
