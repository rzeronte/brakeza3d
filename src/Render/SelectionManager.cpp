#include <algorithm>
#include "imgui.h"
#include <SDL2/SDL.h>
#include "../../include/Render/SelectionManager.h"
#include "../../include/3D/Object3D.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/Logging.h"

bool SelectionManager::isObjectAlive(Object3D* o) const
{
    if (o == nullptr) return false;
    for (auto* obj : Brakeza::get()->getSceneObjects()) {
        if (obj == o) return true;
    }
    return false;
}

Object3D* SelectionManager::getLastRightClickedObject() const
{
    return isObjectAlive(lastRightClickedObject) ? lastRightClickedObject : nullptr;
}

Object3D* SelectionManager::getLastLeftClickedObject() const
{
    return isObjectAlive(lastLeftClickedObject) ? lastLeftClickedObject : nullptr;
}

void SelectionManager::setSelectedObject(Object3D *o)
{
    selectedObjects.clear();
    if (o != nullptr) selectedObjects.push_back(o);
}

void SelectionManager::addToSelection(Object3D *o)
{
    if (o == nullptr) return;
    for (auto* existing : selectedObjects) {
        if (existing == o) {
            removeFromSelection(o);
            return;
        }
    }
    selectedObjects.push_back(o);
}

void SelectionManager::removeFromSelection(const Object3D *o)
{
    selectedObjects.erase(
        std::remove_if(selectedObjects.begin(), selectedObjects.end(),
            [o](const Object3D* obj) { return obj == o; }),
        selectedObjects.end()
    );
}

void SelectionManager::clearSelection()
{
    selectedObjects.clear();
}

bool SelectionManager::isObjectInSelection(const Object3D *o) const
{
    for (const auto* obj : selectedObjects) {
        if (obj == o) return true;
    }
    return false;
}

void SelectionManager::DrawSelectionRectFill() const
{
    if (!isRectSelecting) return;
    ImDrawList *dl = ImGui::GetBackgroundDrawList();
    const float x1 = (float)std::min(rectSelectStartX, rectSelectCurrentX);
    const float y1 = (float)std::min(rectSelectStartY, rectSelectCurrentY);
    const float x2 = (float)std::max(rectSelectStartX, rectSelectCurrentX);
    const float y2 = (float)std::max(rectSelectStartY, rectSelectCurrentY);
    dl->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(100, 200, 255, 70));
    dl->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(150, 220, 255, 230), 0.0f, 0, 2.0f);
}

void SelectionManager::DrawSelectionBox() const
{
    if (!isRectSelecting) return;

    auto *render = Components::get()->Render();

    const int x1 = std::min(rectSelectStartX,   rectSelectCurrentX);
    const int y1 = std::min(rectSelectStartY,   rectSelectCurrentY);
    const int x2 = std::max(rectSelectStartX,   rectSelectCurrentX);
    const int y2 = std::max(rectSelectStartY,   rectSelectCurrentY);
    const int w  = x2 - x1;
    const int h  = y2 - y1;

    const Color fill(0.39f, 0.78f, 1.0f, 0.27f);
    const Color border(0.59f, 0.86f, 1.0f, 0.90f);
    const int   t = 2;

    render->DrawFilledRect(x1, y1, w, h, fill);
    render->DrawFilledRect(x1,     y1,     w, t, border);
    render->DrawFilledRect(x1,     y2 - t, w, t, border);
    render->DrawFilledRect(x1,     y1,     t, h, border);
    render->DrawFilledRect(x2 - t, y1,     t, h, border);
}

void SelectionManager::processSDLEvent(SDL_Event *event)
{
    auto input = Components::get()->Input();
    if (Config::get()->ENABLE_IMGUI && ImGui::GetIO().WantCaptureMouse) return;

    auto window = Components::get()->Window();
    const bool ctrlHeld = input->getKeyboard()[SDL_SCANCODE_LCTRL] || input->getKeyboard()[SDL_SCANCODE_RCTRL];

    auto toNormalized = [&](int mx, int my, int &nx, int &ny) {
        nx = (int)((float)mx / window->getWidth()  * window->getWidthRender());
        ny = (int)((float)my / window->getHeight() * window->getHeightRender());
    };

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        rectSelectStartX = event->button.x;
        rectSelectStartY = event->button.y;
        isRectSelecting = false;
    }

    if (rectSelectEnabled && event->type == SDL_MOUSEMOTION && (event->motion.state & SDL_BUTTON_LMASK)) {
        rectSelectCurrentX = event->motion.x;
        rectSelectCurrentY = event->motion.y;

        const int dx = event->motion.x - rectSelectStartX;
        const int dy = event->motion.y - rectSelectStartY;

        if (std::abs(dx) > 5 || std::abs(dy) > 5) {
            isRectSelecting = true;
        }

        if (isRectSelecting) {
            int nx1, ny1, nx2, ny2;
            toNormalized(rectSelectStartX, rectSelectStartY, nx1, ny1);
            toNormalized(event->motion.x, event->motion.y, nx2, ny2);
            window->beginAsyncPickingRect(nx1, ny1, nx2, ny2);
        }
    }

    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_RIGHT) {
        if (!input->isRightDrag()) {
            int nx, ny;
            toNormalized(event->button.x, event->button.y, nx, ny);
            const unsigned int pickedId = window->getObjectIDByPickingColorFramebuffer(nx, ny);
            lastRightClickedObject = Brakeza::get()->getObjectById(pickedId);
            lastRightClickedSubmeshName.clear();
            if (lastRightClickedObject == nullptr) {
                auto entry = Components::get()->Render()->getSubmeshEntry(pickedId);
                if (entry.first != nullptr) {
                    lastRightClickedObject = entry.first;
                    lastRightClickedSubmeshName = entry.second;
                    LOG_MESSAGE("[Selection] RClick submesh: '%s' (parent: %s)", entry.second.c_str(), entry.first->getName().c_str());
                }
            }
            if (lastRightClickedObject == nullptr) {
                lastRightClickedObject = Components::get()->Render()->hitTestAvatar(event->button.x, event->button.y);
            }
        }
    }

    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
        if (!isRectSelecting) {
            // Calcular objeto bajo cursor y exponerlo a Lua (que corre antes que Render)
            int nx, ny;
            toNormalized(rectSelectStartX, rectSelectStartY, nx, ny);
            const auto id = window->getObjectIDByPickingColorFramebuffer(nx, ny);
            Object3D *clicked = Brakeza::get()->getObjectById(id);
            std::string clickedSubmesh;
            if (clicked == nullptr) {
                auto entry = Components::get()->Render()->getSubmeshEntry(id);
                if (entry.first != nullptr) {
                    clicked        = entry.first;
                    clickedSubmesh = entry.second;
                }
            }
            if (clicked == nullptr) {
                clicked = Components::get()->Render()->hitTestAvatar(event->button.x, event->button.y);
            }
            lastLeftClickedObject      = clicked;
            lastLeftClickedSubmeshName = clickedSubmesh;
            pendingClickObject         = clicked;
        }
        pendingLeftClick = true;
        pendingClickCtrl = ctrlHeld;
    }
}

void SelectionManager::update()
{
    if (!pendingLeftClick) return;
    pendingLeftClick = false;

    auto *input = Components::get()->Input();

    if (isRectSelecting) {
        isRectSelecting = false;
        auto window = Components::get()->Window();
        const auto ids = window->readAsyncPickingRect();
        if (!pendingClickCtrl) selectedObjects.clear();
        for (unsigned int id : ids) {
            Object3D *o = Brakeza::get()->getObjectById(id);
            if (o == nullptr) {
                auto entry = Components::get()->Render()->getSubmeshEntry(id);
                o = entry.first;
            }
            if (o != nullptr && o->isEnabled() && o->isSelectable() && !isObjectInSelection(o))
                selectedObjects.push_back(o);
        }
        LOG_MESSAGE("[Selection] Rect selection: %zu objects", selectedObjects.size());
        return;
    }

    // Click simple — respetar consumo de UI (Lua puede haberlo consumido en su onUpdate)
    if (input->isLeftClickConsumed()) {
        clearLeftClickedObject();
        return;
    }

    Object3D *clicked = pendingClickObject;
    pendingClickObject = nullptr;
    if (pendingClickCtrl) {
        if (clicked != nullptr) addToSelection(clicked);
    } else {
        selectedObjects.clear();
        if (clicked != nullptr && clicked->isEnabled() && clicked->isSelectable())
            selectedObjects.push_back(clicked);
    }
}
