//
// Created by Eduardo on 06/12/2025.
//

#include "../../../include/GUI/AddOns/GUIAddonObject3DProperties.h"

#include "../../../include/Brakeza.h"
#include "../../../include/Components/Components.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/Render/Drawable.h"

void GUIAddonObject3DProperties::DrawWinObjectProps(GUIManager *gui)
{
    auto windowStatus = gui->getWindowStatus(GUIType::OBJECT_PROPS);
    if (!windowStatus->isOpen) return;

    auto o = Components::get()->Render()->getSelectedObject();

    if (o == nullptr) {
        Drawable::WarningMessage("No object selected");
        return;
    }

    if (o->isRemoved()) return;

    o->DrawPropertiesGUI();

    ImGui::Separator();

    GUI::DrawButtonConfirm("Remove object from scene", "Are you sure to remove object from scene?", IconGUI::OBJECT_REMOVE_SCENE, GUIType::Sizes::ICONS_BROWSERS, [&] () {
        o->setRemoved(true);
    });
}