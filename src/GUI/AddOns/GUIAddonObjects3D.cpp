//
// Created by Eduardo on 06/12/2025.
//

#include "../../../include/GUI/AddOns/GUIAddonObjects3D.h"

#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../../include/Render/Drawable.h"
#include "../include/Components/Components.h"

bool GUIAddonObjects3D::exist(const std::string &pattern1, const std::string &pattern2)
{
    // Búsqueda case-insensitive
    std::string nameUpper = pattern1;
    std::string filterUpper = pattern2;
    std::transform(nameUpper.begin(), nameUpper.end(), nameUpper.begin(), ::toupper);
    std::transform(filterUpper.begin(), filterUpper.end(), filterUpper.begin(), ::toupper);

    if (nameUpper.find(filterUpper) != std::string::npos) {
        return true;
    }

    return false;
}

void GUIAddonObjects3D::DrawObjectsTree(GUIManager *gui, const std::vector<Object3D *> &objects, std::string filter)
{
    int globalCont = 0;
    for (auto& type : gui->visibleTypeObjects) {
        if (!type.visible) continue;

        int count = 0;
        for (const auto &o : objects) {
            if (o->getTypeObject() == type.type) count++;
        }

        ImGui::Image(FileSystemGUI::Icon(type.icon), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
        ImGui::SameLine(0, 5.0f);

        std::string label = type.label + " (" + std::to_string(count) + ")";

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
        if (count == 0) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.4f, 1.0f)); // Gris oscuro
        }
        bool isOpen = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_FramePadding);
        ImGui::PopStyleVar(2);
        if (count == 0) {
            ImGui::PopStyleColor();
        }
        if (isOpen) {
            int localCont = 0;
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 4.0f)); // x, y padding
            if (ImGui::BeginTable("objectsTreeTable", 5, ImGuiTableFlags_None | ImGuiTableFlags_RowBg)) {
                ImGui::TableSetupColumn("Checkbox", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Scripts", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Shaders", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Colliders", ImGuiTableColumnFlags_WidthFixed);
                for (auto &o : objects) {
                    if (o->getTypeObject() != type.type) continue;
                    if (!filter.empty() && !exist(o->getName(), filter)) continue;
                    DrawItem(globalCont, o, false);
                    globalCont++;
                    localCont++;
                }
                ImGui::EndTable();
                ImGui::PopStyleVar();
            }
            if (localCont == 0) {
                Drawable::WarningMessage("There are not objects of this type");
            }
            ImGui::TreePop();
        }
    }
}

void GUIAddonObjects3D::DrawObjectTypes(GUIManager *gui)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));

    for (auto& o : gui->visibleTypeObjects) {
        GUI::DrawButton(o.label, o.icon, GUIType::Sizes::ICONS_OBJECTS_ALLOWED, o.visible, [&]{ o.Toggle(); });
        ImGui::SameLine();
    }
    ImGui::PopStyleVar(2);
}

bool GUIAddonObjects3D::isObjectTypeVisible(GUIManager *gui, ObjectType typeObject)
{
    for (auto& o : gui->visibleTypeObjects) {
        if (o.visible && o.type == typeObject ) {
            return true;
        }
    }

    return false;
}

void GUIAddonObjects3D::DrawObjectList(GUIManager *gui, std::vector<Object3D *> &objects, std::string filter)
{
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.0f, 4.0f)); // x, y padding
    if (ImGui::BeginTable("ScriptFileButtons", 5, ImGuiTableFlags_None | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Checkbox", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Scripts", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Shaders", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Colliders", ImGuiTableColumnFlags_WidthFixed);
        for (unsigned int i = 0; i < (unsigned int) objects.size(); i++) {
            auto &o = objects[i];

            if (o->isRemoved()) continue;
            if (!isObjectTypeVisible(gui, o->getTypeObject())) continue;
            if (!filter.empty() && !exist(o->getName(), filter)) continue;
            DrawItem(i, o, true);
        }
        ImGui::PopStyleVar();
        ImGui::EndTable();
    }
}

void GUIAddonObjects3D::DrawWinSceneObjects(GUIManager *gui)
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::SCENE_OBJECTS);
    if (!windowStatus->isOpen) return;

    auto &gameObjects = Brakeza::get()->getSceneObjects();

    auto type = gui->getObjectsViewerMode();
    GUI::DrawButton("List mode", IconGUI::OBJECTS_VIEWER_LIST, GUIType::Sizes::ICON_SIZE_MENUS, type == GUIType::ViewerObjectsMode::LIST, [&] {
        gui->setObjectsViewerMode(GUIType::ViewerObjectsMode::LIST);
    });
    ImGui::SameLine();
    GUI::DrawButton("Tree mode", IconGUI::OBJECTS_VIEWER_TREE, GUIType::Sizes::ICON_SIZE_MENUS, type == GUIType::ViewerObjectsMode::TREE, [&] {
        gui->setObjectsViewerMode(GUIType::ViewerObjectsMode::TREE);
    });
    ImGui::SameLine();
    DrawObjectTypes(gui);

    ImGui::NewLine();
    ImGui::Separator();

    static char filterGUI[128] = "";
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("Filter", filterGUI, IM_ARRAYSIZE(filterGUI));
    ImGui::SameLine();
    GUI::DrawButton("Clear filter", IconGUI::OBJECTS_VIEWER_CLEAR_FILTER, GUIType::Sizes::ICON_SIZE_MENUS, type == GUIType::ViewerObjectsMode::TREE, [&] {
        filterGUI[0] = '\0';
    });

    ImGui::Separator();

    switch (type) {
        case GUIType::ViewerObjectsMode::TREE:
            DrawObjectsTree(gui, gameObjects, filterGUI);
            break;
        case GUIType::ViewerObjectsMode::LIST: {
            DrawObjectList(gui, gameObjects, filterGUI);
            break;
        }
        default: {
            DrawObjectList(gui, gameObjects, filterGUI);
        }
    }
}

void GUIAddonObjects3D::DrawItem(int i, Object3D* o, bool icon)
{
    ImGui::TableNextRow();

    ImGui::Spacing();
    // ----------------- Checkbox + icon
    ImGui::TableNextColumn();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
    ImGui::Checkbox(std::string("##"+ std::to_string(o->getId())).c_str(), &o->isEnabled());
    ImGui::PopStyleVar(2);
    ImGui::SameLine();

    if (icon) {
        ImGui::Image(FileSystemGUI::Icon(o->getIcon()), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    }

    // ----------------- Name
    ImGui::TableNextColumn();
    auto selectecObject = Components::get()->Render()->getSelectedObject();
    bool isSelected = selectecObject != nullptr ? selectecObject->getId() == o->getId() : false;

    auto labelSelectable = std::to_string(i + 1) + ") " + o->getName();
    if (ImGui::Selectable(labelSelectable.c_str(), isSelected, ImGuiSelectableFlags_AllowDoubleClick)) {
        Components::get()->Render()->setSelectedObject(o);
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (!Brakeza::get()->GUI()->isWindowOpen(GUIType::OBJECT_PROPS)) {
                Brakeza::get()->GUI()->getWindowStatus(GUIType::OBJECT_PROPS)->isOpen = true;
            }
        }
    }
    if (ImGui::BeginDragDropTarget()) {
        auto mesh = dynamic_cast<Mesh3D*> (o);
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::SCRIPT_ITEM)) {
            LOG_MESSAGE("[GUI] Dropping script (%s) in %s", payload->Data, o->getName().c_str());
            auto meta = ScriptLuaGUI::ExtractScriptMetainfo(std::string((char *) payload->Data));
            o->AttachScript(new ScriptLUA(meta.name, meta.codeFile, meta.typesFile));
        }
        if (mesh != nullptr) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::SHADER_ITEM)) {
                auto* receivedData = (Config::DragDropCustomShaderData*)payload->Data;
                auto fullPath = std::string(receivedData->folder) + receivedData->file;
                LOG_MESSAGE("[GUI] Dropping shader file '%s' into Mesh3D...", fullPath.c_str());
                mesh->LoadShader(fullPath);
            }
        }
        ImGui::EndDragDropTarget();
    }

    // ----------------- Scripts attached
    ImGui::TableNextColumn();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine(0, 5.0f);
    ImGui::Text(std::to_string((int) o->getScripts().size()).c_str());

    // ----------------- Shaders attached
    ImGui::TableNextColumn();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::SHADER_FILE), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine(0, 5.0f);

    auto mesh = dynamic_cast<Mesh3D*>(o);
    auto numShaders = mesh != nullptr ? (unsigned int) mesh->getCustomShaders().size() : 0;
    ImGui::Text(std::to_string(numShaders).c_str());

    // ----------------- Collisions in object
    ImGui::TableNextColumn();
    if (o->isCollisionsEnabled()) {
        if (o->getCollisionMode() == GHOST) {
            ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDER_GHOST), GUIType::Sizes::ICON_SIZE_MENUS);
        } else if (o->getCollisionMode() == BODY) {
            ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDER_BODY), GUIType::Sizes::ICON_SIZE_MENUS);
        } else if (o->getCollisionMode() == KINEMATIC) {
            ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDER_KINEMATIC), GUIType::Sizes::ICON_SIZE_MENUS);
        }
    } else {
        ImGui::Image(FileSystemGUI::Icon(IconGUI::EMPTY), GUIType::Sizes::ICON_SIZE_MENUS);
    }
}
