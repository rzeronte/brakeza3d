//
// Created by Eduardo on 06/12/2025.
//

#include "../../../include/GUI/AddOns/GUIAddonObjects3D.h"
#include <algorithm>
#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/AddOns/CustomTreeNode.h"
#include "../../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../../include/Render/Drawable.h"
#include "../../../include/Components/Components.h"

bool GUIAddonObjects3D::hideEmptyGroups = true;

bool GUIAddonObjects3D::exist(const std::string &pattern1, const std::string &pattern2)
{
    std::string nameUpper = pattern1;
    std::string filterUpper = pattern2;
    std::transform(nameUpper.begin(), nameUpper.end(), nameUpper.begin(), ::toupper);
    std::transform(filterUpper.begin(), filterUpper.end(), filterUpper.begin(), ::toupper);
    return nameUpper.find(filterUpper) != std::string::npos;
}


bool GUIAddonObjects3D::isObjectTypeVisible(GUIManager *gui, ObjectType typeObject)
{
    return true;
}

void GUIAddonObjects3D::DrawObjectWithCustomNode(Object3D* o, int index)
{
    if (!o || o->isRemoved()) return;

    // Configurar el TreeNode
    std::string objectLabel = std::to_string(index + 1) + ") " + o->getName();
    CustomImGui::CustomTreeNodeConfig config(objectLabel.c_str());

    // Icono del objeto
    config.leftIcon = FileSystemGUI::Icon(o->getIcon());
    config.iconSize = GUIType::Sizes::ICON_SIZE_MENUS;

    // Checkbox para enable/disable
    bool isEnabled = o->isEnabled();
    bool wasEnabled = isEnabled;
    config.showCheckbox = true;
    config.p_checked = &isEnabled;
    config.isLeaf = true;
    config.itemPadding = 0.8f;
    config.itemMargin = 4.0f;

    // ===== ACCIONES A LA DERECHA =====

    // Scripts
    int numScripts = (int)o->getScripts().size();
    if (!o->getScripts().empty()) {
        config.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SCRIPT_FILE),
            "Scripts: " + std::to_string(numScripts),
            [o]() {
                Components::get()->Render()->setSelectedObject(o);
                Brakeza::get()->GUI()->getWindowStatus(GUIType::OBJECT_SCRIPTS)->isOpen = true;
        });
    }

    // Shaders
    auto mesh = dynamic_cast<Mesh3D*>(o);
    if (mesh != nullptr && !mesh->getCustomShaders().empty()) {
        int numShaders = (int) mesh->getCustomShaders().size();
        config.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SHADER_FILE),
            "Shaders: " + std::to_string(numShaders),
            [o]() {
                Components::get()->Render()->setSelectedObject(o);
                Brakeza::get()->GUI()->getWindowStatus(GUIType::OBJECT_SHADERS)->isOpen = true;            }
        );
    }

    // Collider
    if (o->isCollisionsEnabled()) {
        CollisionMode mode = o->getCollisionMode();
        GUIType::Sheet collIcon = (mode == GHOST) ? IconGUI::COLLIDER_GHOST
                                : (mode == BODY)  ? IconGUI::COLLIDER_BODY
                                                  : IconGUI::COLLIDER_KINEMATIC;
        std::string collLabel = (mode == GHOST) ? "Ghost" : (mode == BODY) ? "Body" : "Kinematic";
        config.actionItems.emplace_back(
            FileSystemGUI::Icon(collIcon),
            collLabel,
            [o]() {
                Components::get()->Render()->setSelectedObject(o);
                Brakeza::get()->GUI()->getWindowStatus(GUIType::OBJECT_PROPS)->isOpen = true;
            }
        );
    }

    // Drag & Drop
    CustomImGui::TreeDragDropConfig dragDropScript;
    dragDropScript.acceptsDrop = true;
    dragDropScript.dragDropType = GUIType::DragDropTarget::SCRIPT_ITEM;
    dragDropScript.onDropCallback = [o](void* payloadData) {
        auto meta = ScriptLuaGUI::ExtractScriptMetainfo(std::string((char*)payloadData));
        o->AttachScript(new ScriptLUA(meta.name, meta.codeFile, meta.typesFile));
    };
    config.dragDrop = dragDropScript;
    config.onDoubleClick = [o]() {
        Components::get()->Render()->setSelectedObject(o);
        if (!Brakeza::get()->GUI()->isWindowOpen(GUIType::OBJECT_PROPS)) {
            Brakeza::get()->GUI()->getWindowStatus(GUIType::OBJECT_PROPS)->isOpen = true;
        }
    };

    // ===== SELECCIÓN =====
    bool isSelected = Components::get()->Render()->isObjectInSelection(o);

    bool wasSelected = isSelected;

    // Dibujar
    CustomImGui::CustomTreeNode(config, &isSelected);

    // Detectar cambio
    if (isSelected && !wasSelected) {
        Components::get()->Render()->setSelectedObject(o);
        LOG_MESSAGE("[Objects] Selected: %s", o->getName().c_str());
    }

    // Actualizar estado enabled solo cuando cambió
    if (isEnabled != wasEnabled) {
        o->setEnabled(isEnabled);
    }

    // Drag & Drop shaders (Mesh3D)
    if (mesh != nullptr) {
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::SHADER_ITEM)) {
                auto* receivedData = (Config::DragDropCustomShaderData*)payload->Data;
                auto fullPath = std::string(receivedData->folder) + receivedData->file;
                mesh->LoadShader(fullPath);
            }
            ImGui::EndDragDropTarget();
        }
    }
}

// ============================================================================
// DrawObjectsTree - Solo categorías con bullets
// ============================================================================
void GUIAddonObjects3D::DrawObjectsTree(GUIManager *gui, const std::vector<Object3D *> &objects, const std::string &filter)
{
    int globalIndex = 0;

    for (auto& type : gui->visibleTypeObjects) {
        // Contar objetos
        int count = 0;
        for (const auto &o : objects) {
            if (o->isRemoved()) continue;
            if (o->getTypeObject() == type.type) {
                if (filter.empty() || exist(o->getName(), filter)) {
                    count++;
                }
            }
        }

        if (count == 0 && hideEmptyGroups) continue;

        // ============================================================
        // CATEGORÍA con CustomTreeNode - CON BULLETS
        // ============================================================
        CustomImGui::CustomTreeNodeConfig categoryConfig(type.label.c_str());

        categoryConfig.leftIcon = FileSystemGUI::Icon(type.icon);
        categoryConfig.iconSize = GUIType::Sizes::ICONS_OBJECTS_ALLOWED;

        // 🎯 BULLETS solo en categorías
        categoryConfig.bulletOpen = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
        categoryConfig.bulletClosed = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);

        categoryConfig.showChildCount = true;
        categoryConfig.childCount = count;

        categoryConfig.showCheckbox = false;
        categoryConfig.itemPadding = 1.0f;
        categoryConfig.indentSpacing = 20.0f;
        categoryConfig.defaultOpen = false;

        if (count == 0) {
            categoryConfig.iconTint = ImVec4(0.5f, 0.5f, 0.5f, 0.6f);  // Gris más apagado
            categoryConfig.hoveredColor = ImVec4(0.3f, 0.3f, 0.3f, 0.3f);
            categoryConfig.textColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f); // Texto gris también
        }

        bool isCategoryOpen = CustomImGui::CustomTreeNode(categoryConfig, nullptr);

        if (isCategoryOpen) {
            int localCount = 0;

            std::vector<Object3D*> sorted;
            for (auto &o : objects) {
                if (o->isRemoved()) continue;
                if (o->getTypeObject() != type.type) continue;
                if (!filter.empty() && !exist(o->getName(), filter)) continue;
                sorted.push_back(o);
            }
            std::sort(sorted.begin(), sorted.end(), [](Object3D* a, Object3D* b) {
                return a->getName() < b->getName();
            });

            for (auto &o : sorted) {
                DrawObjectWithCustomNode(o, globalIndex);

                globalIndex++;
                localCount++;
            }

            if (localCount == 0) {
                ImGui::Indent();
                ImGui::Spacing();
                Drawable::WarningMessage("Empty list");
                ImGui::Unindent();
            }

            CustomImGui::CustomTreePop(categoryConfig.indentSpacing);
        }
    }
}

void GUIAddonObjects3D::DrawObjectList(GUIManager *gui, std::vector<Object3D *> &objects, const std::string &filter)
{
    if (objects.empty()) {
        Drawable::WarningMessage("Empty list");
    }

    for (unsigned int i = 0; i < (unsigned int) objects.size(); i++) {
        auto &o = objects[i];

        if (o->isRemoved()) continue;
        if (!isObjectTypeVisible(gui, o->getTypeObject())) continue;
        if (!filter.empty() && !exist(o->getName(), filter)) continue;

        DrawObjectWithCustomNode(o, i);
    }
}

void GUIAddonObjects3D::DrawObjectTypes(GUIManager *gui)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));

    for (auto& o : gui->visibleTypeObjects) {
        bool active = o.avatarFlag && *o.avatarFlag;
        GUI::DrawButton(o.label, o.icon, GUIType::Sizes::ICONS_OBJECTS_ALLOWED, active, [&]{
            if (o.avatarFlag) *o.avatarFlag = !*o.avatarFlag;
        });
        ImGui::SameLine();
    }

    ImGui::PopStyleVar(2);
    ImGui::NewLine();
}

// ============================================================================
// DrawWinSceneObjects - Ventana principal
// ============================================================================
void GUIAddonObjects3D::DrawWinSceneObjects(GUIManager *gui)
{
    auto windowStatus = Brakeza::get()->GUI()->getWindowStatus(GUIType::SCENE_OBJECTS);
    if (!windowStatus->isOpen) return;

    auto &gameObjects = Brakeza::get()->getSceneObjects();
    auto type = gui->getObjectsViewerMode();

    ImGui::Spacing();
    DrawObjectTypes(gui);

    static char filterGUI[128] = "";

    if (ImGui::BeginTable("ControlsTable", 2, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        GUI::DrawButton("Hide empty groups", IconGUI::OBJECTS_VIEWER_EMPTY_GROUPS, GUIType::Sizes::ICON_SIZE_MENUS,
            hideEmptyGroups, [&] {
            hideEmptyGroups = !hideEmptyGroups;
        });
        ImGui::SameLine();
        GUI::DrawButton("List mode", IconGUI::OBJECTS_VIEWER_LIST, GUIType::Sizes::ICON_SIZE_MENUS,
            type == GUIType::ViewerObjectsMode::LIST, [&] {
            gui->setObjectsViewerMode(GUIType::ViewerObjectsMode::LIST);
        });
        ImGui::SameLine();
        GUI::DrawButton("Tree mode", IconGUI::OBJECTS_VIEWER_TREE, GUIType::Sizes::ICON_SIZE_MENUS,
            type == GUIType::ViewerObjectsMode::TREE, [&] {
            gui->setObjectsViewerMode(GUIType::ViewerObjectsMode::TREE);
        });

        ImGui::TableSetColumnIndex(1);
        float inputWidth = 140;
        float buttonWidth = ImGui::CalcTextSize("Clear filter").x + GUIType::Sizes::ICON_SIZE_MENUS.x + ImGui::GetStyle().FramePadding.x * 4;
        float spacing = ImGui::GetStyle().ItemSpacing.x;
        float totalWidth = inputWidth + spacing + buttonWidth;
        float availWidth = ImGui::GetContentRegionAvail().x;
        float offsetX = availWidth - totalWidth;

        if (offsetX > 0) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
        }

        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputText("Filter", filterGUI, IM_ARRAYSIZE(filterGUI));
        ImGui::SameLine();
        GUI::DrawButton("Clear filter", IconGUI::OBJECTS_VIEWER_CLEAR_FILTER, GUIType::Sizes::ICON_SIZE_MENUS,
            false, [&] {
            filterGUI[0] = '\0';
        });

        ImGui::EndTable();
    }

    ImGui::Separator();

    auto &loadedScenes = Components::get()->Scripting()->getLoadedScenes();

    if (loadedScenes.size() > 1) {
        for (auto *scene : loadedScenes) {
            std::string sceneLabel = scene->getName();
            if (!scene->isActive()) sceneLabel += "  [inactive]";

            bool headerOpen = ImGui::CollapsingHeader(sceneLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

            if (headerOpen) {
                ImGui::PushID(scene);
                switch (type) {
                    case GUIType::ViewerObjectsMode::TREE:
                        DrawObjectsTree(gui, scene->getObjects(), filterGUI);
                        break;
                    default:
                        DrawObjectList(gui, scene->getObjects(), filterGUI);
                        break;
                }
                ImGui::PopID();
            }
        }

        // Objetos sin escena (multiScene, creados por script, etc.)
        std::vector<Object3D*> unscoped;
        for (auto *o : gameObjects) {
            if (o->getScene() == nullptr && !o->isRemoved()) unscoped.push_back(o);
        }
        if (!unscoped.empty()) {
            if (ImGui::CollapsingHeader("Global", ImGuiTreeNodeFlags_DefaultOpen)) {
                switch (type) {
                    case GUIType::ViewerObjectsMode::TREE:
                        DrawObjectsTree(gui, unscoped, filterGUI);
                        break;
                    default:
                        DrawObjectList(gui, unscoped, filterGUI);
                        break;
                }
            }
        }
    } else {
        switch (type) {
            case GUIType::ViewerObjectsMode::TREE:
                DrawObjectsTree(gui, gameObjects, filterGUI);
                break;
            case GUIType::ViewerObjectsMode::LIST:
                DrawObjectList(gui, gameObjects, filterGUI);
                break;
            default:
                DrawObjectList(gui, gameObjects, filterGUI);
        }
    }
}