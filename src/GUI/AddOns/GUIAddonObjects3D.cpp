//
// Created by Eduardo on 06/12/2025.
//

#include "../../../include/GUI/AddOns/GUIAddonObjects3D.h"

#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../include/Components/Components.h"

GUIAddonObjects3D::GUIAddonObjects3D()
{
}

bool GUIAddonObjects3D::exist(std::string pattern1, std::string pattern2)
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

void GUIAddonObjects3D::DrawObjectsTree(GUIManager *gui, const std::vector<Object3D *> &objects, int selectedIndex, std::string filter)
{
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

        bool isOpen = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_FramePadding);

        ImGui::PopStyleVar(2);

        if (isOpen) {
            int i = 0;
            for (auto &o : objects) {
                if (o->getTypeObject() != type.type) continue;
                if (!filter.empty() && !exist(o->getName(), filter)) continue;
                DrawItem(i, o, objects, selectedIndex, false);
                i++;
            }
            ImGui::TreePop();
        }
    }
}

void GUIAddonObjects3D::DrawObjectTypes(GUIManager *gui)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));

    for (auto& o : gui->visibleTypeObjects) {
        GUI::DrawButton(o.label, o.icon, ImVec2(28, 28), o.visible, [&](){ o.Toggle(); });
        ImGui::SameLine();
    }
    ImGui::PopStyleVar(2);
    ImGui::NewLine();
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

void GUIAddonObjects3D::DrawObjectList(GUIManager *gui, std::vector<Object3D *> &objects, int selectedObjectIndex, std::string filter)
{
    for (unsigned int i = 0; i < (unsigned int) objects.size(); i++) {
        auto o = objects[i];

        if (o->isRemoved()) continue;
        if (!isObjectTypeVisible(gui, o->getTypeObject())) continue;
        if (!filter.empty() && !exist(o->getName(), filter)) continue;
        DrawItem(i, o, objects, selectedObjectIndex, true);
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

    static char filterGUI[128] = "";
    ImGui::SetNextItemWidth(150);
    ImGui::InputText("Filter", filterGUI, IM_ARRAYSIZE(filterGUI));
    ImGui::SameLine();
    GUI::DrawButton("Clear filter", IconGUI::OBJECTS_VIEWER_CLEAR_FILTER, GUIType::Sizes::ICON_SIZE_MENUS, type == GUIType::ViewerObjectsMode::TREE, [&] {
        filterGUI[0] = '\0';
    });

    ImGui::Separator();
    DrawObjectTypes(gui);
    ImGui::Separator();

    auto title = std::string("Number objects: ") + std::to_string(gameObjects.size());
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", title.c_str());
    ImGui::Separator();

    auto selectedObjectIndex = gui->selectedObjectIndexPointer();

    switch (type) {
        case GUIType::ViewerObjectsMode::TREE:
            DrawObjectsTree(gui, gameObjects, selectedObjectIndex, filterGUI);
            break;
        case GUIType::ViewerObjectsMode::LIST: {
            DrawObjectList(gui, gameObjects, selectedObjectIndex, filterGUI);
            break;
        }
        default: {
            DrawObjectList(gui, gameObjects, selectedObjectIndex, filterGUI);
        }
    }
}

void GUIAddonObjects3D::DrawItem(int i, Object3D* o, const std::vector<Object3D *> &objects, int selectedIndex, bool icon)
{
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 2));
        ImGui::Checkbox(std::string("##"+ std::to_string(o->getId())).c_str(), &o->isEnabled());
        ImGui::PopStyleVar(2);
        ImGui::SameLine();

        if (icon) {
            ImGui::Image(FileSystemGUI::Icon(o->getIcon()), ImVec2(16, 16)); ImGui::SameLine();
        }

        ImGui::SetNextItemWidth(150.0f);
        if (ImGui::Selectable(std::string("##select"+ std::to_string(i)).c_str(), selectedIndex == i, ImGuiSelectableFlags_AllowDoubleClick)) {
            Brakeza::get()->GUI()->setSelectedObjectIndex(i);
            Components::get()->Render()->setSelectedObject(objects[i]);
            if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (!Brakeza::get()->GUI()->isWindowOpen(GUIType::OBJECT_PROPS)) {
                    Brakeza::get()->GUI()->getWindowStatus(GUIType::OBJECT_PROPS)->isOpen = true;
                }
            }
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 15.0f);
        ImGui::Text((std::to_string(i + 1) + ") " + o->getName()).c_str());
        if (ImGui::BeginDragDropTarget()) {
            auto mesh = dynamic_cast<Mesh3D*> (o);

            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_ITEM")) {
                Logging::Message("Dropping script (%s) in %s", payload->Data, o->getName().c_str());
                o->AttachScript(new ScriptLUA(
                        std::string((const char*) payload->Data),
                        ScriptLUA::dataTypesFileFor(std::string((char *)payload->Data)))
                );
            }

            if (mesh != nullptr) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CUSTOMSHADER_ITEM")) {
                    auto* receivedData = (Config::DragDropCustomShaderData*)payload->Data;
                    Logging::Message("Dropping shader into Mesh3D (Folder: %s, File: %s)", receivedData->folder, receivedData->file);
                    mesh->LoadShader(receivedData->folder, receivedData->file);
                }
            }
            ImGui::EndDragDropTarget();
        }
        ImGui::SameLine();
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));

        ImGui::Image(FileSystemGUI::Icon(IconGUI::SCRIPT_FILE), ImVec2(14, 14)); ImGui::SameLine();

        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 10.0f, ImGui::GetCursorPosY()));
        ImGui::Text(std::to_string((int)o->getScripts().size()).c_str());
        if (o->isCollisionsEnabled()) {
            ImGui::SameLine();
            ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 5.0f));

            if (o->getCollisionMode() == GHOST) {
                ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDER_GHOST), ImVec2(14, 14));
            }
            if (o->getCollisionMode() == BODY) {
                ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDER_BODY), ImVec2(14, 14));
            }
            if (o->getCollisionMode() == KINEMATIC) {
                ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDER_KINEMATIC), ImVec2(14, 14));
            }
        }
}