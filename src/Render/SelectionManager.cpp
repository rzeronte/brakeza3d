#include <algorithm>
#include "imgui.h"
#include <SDL2/SDL.h>
#include "../../include/Render/SelectionManager.h"
#include "../../include/3D/Object3D.h"
#include "../../include/OpenGL/Quad/ShaderOGLLine.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/Logging.h"

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

void SelectionManager::DrawSelectionRect(ShaderOGLLine *lineShader) const
{
    if (!isRectSelecting) return;

    auto *window = Components::get()->Window();

    auto toRender = [&](int wx, int wy) -> Point2D {
        const int rx = (int)((float)wx / (float)window->getWidth()  * (float)Config::get()->screenWidth);
        const int ry = (int)((float)wy / (float)window->getHeight() * (float)Config::get()->screenHeight);
        return Point2D(rx, ry);
    };

    const Point2D a = toRender(rectSelectStartX,   rectSelectStartY);
    const Point2D b = toRender(rectSelectCurrentX, rectSelectCurrentY);

    const Point2D tl(a.x, a.y);
    const Point2D tr(b.x, a.y);
    const Point2D br(b.x, b.y);
    const Point2D bl(a.x, b.y);

    const Color  lineColor(0.39f, 0.78f, 1.0f, 1.0f);
    const float  weight = 0.00025f;
    const GLuint fbo    = window->getUIFramebuffer();

    lineShader->render(tl, tr, lineColor, weight, fbo);
    lineShader->render(tr, br, lineColor, weight, fbo);
    lineShader->render(br, bl, lineColor, weight, fbo);
    lineShader->render(bl, tl, lineColor, weight, fbo);
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

    if (event->type == SDL_MOUSEMOTION && (event->motion.state & SDL_BUTTON_LMASK)) {
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

    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
        if (isRectSelecting) {
            isRectSelecting = false;

            const auto ids = window->readAsyncPickingRect();
            if (!ctrlHeld) selectedObjects.clear();
            for (unsigned int id : ids) {
                Object3D *o = Brakeza::get()->getObjectById(id);
                if (o != nullptr && !isObjectInSelection(o)) selectedObjects.push_back(o);
            }
            LOG_MESSAGE("[Selection] Rect selection: %zu objects", selectedObjects.size());

        } else {
            int nx, ny;
            toNormalized(rectSelectStartX, rectSelectStartY, nx, ny);

            const auto id = window->getObjectIDByPickingColorFramebuffer(nx, ny);
            Object3D *clicked = Brakeza::get()->getObjectById(id);

            if (ctrlHeld) {
                if (clicked != nullptr) {
                    addToSelection(clicked);
                    LOG_MESSAGE("[Selection] Toggle: %s (%zu selected)", clicked->getName().c_str(), selectedObjects.size());
                }
            } else {
                selectedObjects.clear();
                if (clicked != nullptr) {
                    selectedObjects.push_back(clicked);
                    LOG_MESSAGE("[Selection] Selected: %s", clicked->getName().c_str());
                }
            }
        }
    }
}
