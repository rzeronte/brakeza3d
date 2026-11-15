/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by Eduardo on 06/12/2025.
//

#include "../../../include/GUI/AddOns/GUIAddonObject3DProperties.h"

#include "../../../include/Brakeza.h"
#include "../../../include/Components/Components.h"
#include "../../../include/GUI/Windows/FileSystemGUI.h"
#include "../../../include/Render/Drawable.h"
#include "../../../include/Render/UndoManager.h"

void GUIAddonObject3DProperties::DrawWinObjectProps(GUIManager *gui)
{
    auto windowStatus = gui->getWindowStatus(GUIType::OBJECT_PROPS);
    if (!windowStatus->isOpen) return;

    auto render = Components::get()->Render();

    if (render->hasMultipleSelected()) {
        auto count = render->getSelectedObjects().size();
        Drawable::WarningMessage(("Multiple objects selected (" + std::to_string(count) + ")").c_str());
        return;
    }

    auto o = render->getSelectedObject();

    if (o == nullptr) {
        Drawable::WarningMessage("No object selected");
        return;
    }

    if (o->isRemoved()) return;

    UndoManager::get().observe(o, UndoManager::SRC_FIELD, UndoManager::get().fieldEditing());
    o->DrawPropertiesGUI();

    ImGui::Separator();

    GUI::ImageButtonNormalConfirm(IconGUI::OBJECT_REMOVE_SCENE, "Remove", "Removing scene object...", "Are you sure to remove object from scene?", [&] () {
        UndoManager::get().removeObjects({o});
    });
}