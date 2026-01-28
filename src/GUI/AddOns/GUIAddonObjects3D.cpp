//
// Created by Eduardo on 06/12/2025.
// SIN BULLETS en objetos (solo en categorías)
//

#include "../../../include/GUI/AddOns/GUIAddonObjects3D.h"
#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/AddOns/CustomTreeNode.h"
#include "../../../include/GUI/Objects/ScriptLuaGUI.h"
#include "../../../include/Render/Drawable.h"
#include "../../../include/Components/Components.h"

// ============================================================================
// FUNCIÓN AUXILIAR: Búsqueda case-insensitive
// ============================================================================
bool GUIAddonObjects3D::exist(const std::string &pattern1, const std::string &pattern2)
{
    std::string nameUpper = pattern1;
    std::string filterUpper = pattern2;
    std::transform(nameUpper.begin(), nameUpper.end(), nameUpper.begin(), ::toupper);
    std::transform(filterUpper.begin(), filterUpper.end(), filterUpper.begin(), ::toupper);
    return nameUpper.find(filterUpper) != std::string::npos;
}

// ============================================================================
// FUNCIÓN AUXILIAR: Verificar si un tipo de objeto está visible
// ============================================================================
bool GUIAddonObjects3D::isObjectTypeVisible(GUIManager *gui, ObjectType typeObject)
{
    for (auto& o : gui->visibleTypeObjects) {
        if (o.visible && o.type == typeObject) {
            return true;
        }
    }
    return false;
}

// ============================================================================
// FUNCIÓN PRINCIPAL: Dibujar objeto individual con CustomTreeNode
// ============================================================================
void GUIAddonObjects3D::DrawObjectWithCustomNode(Object3D* o, int index)
{
    // Configurar el TreeNode
    std::string objectLabel = std::to_string(index + 1) + ") " + o->getName();
    CustomImGui::CustomTreeNodeConfig config(objectLabel.c_str());

    // Icono del objeto
    config.leftIcon = FileSystemGUI::Icon(o->getIcon());
    config.iconSize = GUIType::Sizes::ICON_SIZE_MENUS;

    // Checkbox para enable/disable
    bool isEnabled = o->isEnabled();
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
        int numShaders = mesh ? (int)mesh->getCustomShaders().size() : 0;
        config.actionItems.emplace_back(
            FileSystemGUI::Icon(IconGUI::SHADER_FILE),
            "Shaders: " + std::to_string(numShaders),
            [o, mesh]() {
                Components::get()->Render()->setSelectedObject(o);
                Brakeza::get()->GUI()->getWindowStatus(GUIType::OBJECT_SHADERS)->isOpen = true;            }
        );
    }

    // Colliders
    GUIType::Sheet colliderIcon = IconGUI::EMPTY;
    std::string colliderTooltip = "No collider";

    if (o->isCollisionsEnabled()) {
        if (o->getCollisionMode() == GHOST) {
            colliderIcon = IconGUI::COLLIDER_GHOST;
            colliderTooltip = "Collider: GHOST";
        } else if (o->getCollisionMode() == BODY) {
            colliderIcon = IconGUI::COLLIDER_BODY;
            colliderTooltip = "Collider: BODY";
        } else if (o->getCollisionMode() == KINEMATIC) {
            colliderIcon = IconGUI::COLLIDER_KINEMATIC;
            colliderTooltip = "Collider: KINEMATIC";
        }

        config.actionItems.emplace_back(
            FileSystemGUI::Icon(colliderIcon),
            colliderTooltip,
            [o]() {
                LOG_MESSAGE("[Objects] Collider: %s", o->getName().c_str());
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
    auto selectedObject = Components::get()->Render()->getSelectedObject();
    bool isSelected = (selectedObject != nullptr && selectedObject->getId() == o->getId());

    bool wasSelected = isSelected;

    // Dibujar
    bool isOpen = CustomImGui::CustomTreeNode(config, &isSelected);

    // Detectar cambio
    if (isSelected && !wasSelected) {
        Components::get()->Render()->setSelectedObject(o);
        LOG_MESSAGE("[Objects] Selected: %s", o->getName().c_str());
    }

    // Actualizar estado enabled
    o->setEnabled(isEnabled);

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
void GUIAddonObjects3D::DrawObjectsTree(GUIManager *gui, const std::vector<Object3D *> &objects, std::string filter)
{
    int globalIndex = 0;

    for (auto& type : gui->visibleTypeObjects) {
        if (!type.visible) continue;

        // Contar objetos
        int count = 0;
        for (const auto &o : objects) {
            if (o->getTypeObject() == type.type) {
                if (filter.empty() || exist(o->getName(), filter)) {
                    count++;
                }
            }
        }

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

            for (auto &o : objects) {
                if (o->getTypeObject() != type.type) continue;
                if (!filter.empty() && !exist(o->getName(), filter)) continue;

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

// ============================================================================
// DrawObjectList - Modo lista (sin bullets tampoco)
// ============================================================================
void GUIAddonObjects3D::DrawObjectList(GUIManager *gui, std::vector<Object3D *> &objects, std::string filter)
{
    for (unsigned int i = 0; i < (unsigned int)objects.size(); i++) {
        auto &o = objects[i];

        if (o->isRemoved()) continue;
        if (!isObjectTypeVisible(gui, o->getTypeObject())) continue;
        if (!filter.empty() && !exist(o->getName(), filter)) continue;

        DrawObjectWithCustomNode(o, i);
    }
}

// ============================================================================
// DrawObjectTypes - Botones de filtro
// ============================================================================
void GUIAddonObjects3D::DrawObjectTypes(GUIManager *gui)
{
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));

    for (auto& o : gui->visibleTypeObjects) {
        GUI::DrawButton(o.label, o.icon, GUIType::Sizes::ICONS_OBJECTS_ALLOWED, o.visible, [&]{ o.Toggle(); });
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