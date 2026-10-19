//
// Created by Eduardo on 06/12/2025.
//

#include "../../../include/GUI/AddOns/GUIAddonObjects3D.h"
#include <algorithm>
#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Windows/FileSystemGUI.h"
#include "../../../include/GUI/AddOns/CustomTreeNode.h"
#include "../../../include/GUI/Windows/ScriptLuaGUI.h"
#include "../../../include/GUI/Widgets/GUIHelpers.h"
#include "../../../include/GUI/Widgets/GUIFields.h"
#include "../../../include/Render/Drawable.h"
#include "../../../include/Components/Components.h"

bool GUIAddonObjects3D::hideEmptyGroups = true;
int GUIAddonObjects3D::maxListedObjects = 25;

// ── Modo carpetas: estado ─────────────────────────────────────────────────────
namespace {
    struct FolderDragPayload { Scene *scene; int id; };

    // Ventana de nombre (nueva carpeta / renombrar); se abre al final de DrawWinSceneObjects
    struct FolderModalState {
        bool request = false;
        Scene *scene = nullptr;
        int folderId = 0;       // > 0 = renombrar esa; 0 = crear
        int parent = 0;         // al crear
        char name[128] = "";
        bool duplicate = false; // el último intento chocó con otra carpeta del mismo nivel
    } folderModal;

    // Datos del frame del modo carpetas (los rellena DrawObjectsFolders)
    std::unordered_map<int, std::vector<Object3D *>> folderObjects;   // carpeta -> objetos que pasan el filtro
    std::unordered_map<int, int> folderMatchCount;                     // carpeta -> nº en ella + subcarpetas
    int folderRowIndex = 0;

    // Los menús contextuales se abren dentro del estilo compacto de la lista (FramePadding/ItemSpacing
    // mínimos), así que salían apretados: se sobrescribe solo mientras el popup (y sus submenús) se dibuja.
    // Debe construirse ANTES de BeginPopup* (WindowPadding se lee al abrir la ventana).
    struct ContextMenuStyle {
        ContextMenuStyle()
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 8.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,  ImVec2(8.0f, 5.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,   ImVec2(8.0f, 6.0f));
        }
        ~ContextMenuStyle() { ImGui::PopStyleVar(3); }
        ContextMenuStyle(const ContextMenuStyle &) = delete;
        ContextMenuStyle &operator=(const ContextMenuStyle &) = delete;
    };

    void requestFolderModal(Scene *scene, int folderId, int parent, const std::string &current)
    {
        folderModal.request  = true;
        folderModal.duplicate = false;
        folderModal.scene    = scene;
        folderModal.folderId = folderId;
        folderModal.parent   = parent;
        snprintf(folderModal.name, sizeof(folderModal.name), "%s", current.c_str());
    }

    // Objetos de una carpeta y de todas sus subcarpetas (sin filtro)
    void collectFolderObjects(Scene *scene, int folderId, std::vector<Object3D *> &out)
    {
        for (auto *o : scene->getObjects())
            if (!o->isRemoved() && scene->isFolderInside(o->getFolderId(), folderId)) out.push_back(o);
    }

    // Objetos a mover al soltar: la selección entera si el arrastrado está seleccionado
    std::vector<Object3D *> dragTargets(Object3D *dragged)
    {
        auto *render = Components::get()->Render();
        if (dragged != nullptr && render->isObjectInSelection(dragged)) return render->getSelectedObjects();
        return dragged ? std::vector<Object3D *>{ dragged } : std::vector<Object3D *>{};
    }
}

// Línea final de una lista recortada por maxListedObjects
void GUIAddonObjects3D::DrawMoreObjects(int hidden)
{
    if (hidden <= 0) return;
    ImGui::Indent();
    ImGui::TextDisabled("... %d more object%s", hidden, hidden == 1 ? "" : "s");
    ImGui::Unindent();
}

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
    ImGui::PushID(o);   // la zona de arrastre del nodo no lleva ID propio: uno por fila

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

    // Arrastrar el objeto (a una carpeta del modo carpetas) y su menú contextual: sobre el botón
    // principal de la fila (ver CustomTreeNodeConfig::onNodeItem)
    config.onNodeItem = [o]() {
        if (ImGui::BeginDragDropSource()) {
            unsigned int id = o->getId();
            ImGui::SetDragDropPayload(GUIType::DragDropTarget::OBJECT_ITEM, &id, sizeof(id));
            auto n = dragTargets(o).size();
            if (n > 1) ImGui::Text("%s (+%d selected)", o->getName().c_str(), (int) n - 1);
            else       ImGui::Text("%s", o->getName().c_str());
            ImGui::EndDragDropSource();
        }
        DrawObjectContextMenu(o);
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

    ImGui::PopID();
}

// ============================================================================
// Modo carpetas
// ============================================================================

// Escena donde se crean carpetas desde la cabecera: la única cargada, o la primera
Scene *GUIAddonObjects3D::FolderTargetScene()
{
    auto &scenes = Components::get()->Scripting()->getLoadedScenes();
    return scenes.empty() ? nullptr : scenes.front();
}

void GUIAddonObjects3D::DrawMoveToFolderMenu(Scene *scene, int parent, const std::vector<Object3D *> &targets, int depth)
{
    std::vector<const SceneFolder *> children;
    for (const auto &f : scene->getFolders()) if (f.parent == parent) children.push_back(&f);
    std::sort(children.begin(), children.end(), [](const SceneFolder *a, const SceneFolder *b) { return a->name < b->name; });
    for (const auto *f : children) {
        std::string label = std::string(depth * 3, ' ') + f->name + "##mv" + std::to_string(f->id);
        if (ImGui::MenuItem(label.c_str())) {
            for (auto *t : targets) scene->moveObjectToFolder(t, f->id);
        }
        DrawMoveToFolderMenu(scene, f->id, targets, depth + 1);
    }
}

void GUIAddonObjects3D::DrawObjectContextMenu(Object3D *o)
{
    ContextMenuStyle menuStyle;
    if (!ImGui::BeginPopupContextItem("##objctx")) return;
    auto targets = dragTargets(o);
    ImGui::TextDisabled("%s%s", o->getName().c_str(), targets.size() > 1 ? " (+ selection)" : "");
    ImGui::Separator();
    auto &scenes = Components::get()->Scripting()->getLoadedScenes();
    if (scenes.empty()) {
        ImGui::TextDisabled("Load a scene to use folders");
    } else if (ImGui::BeginMenu("Move to folder")) {
        for (auto *scene : scenes) {
            // Un objeto con escena solo puede ir a carpetas de su escena; uno sin escena, a cualquiera
            if (o->getScene() != nullptr && o->getScene() != scene) continue;
            if (scenes.size() > 1) ImGui::SeparatorText(scene->getName().c_str());
            if (ImGui::MenuItem(("(no folder)##root" + scene->getName()).c_str())) {
                for (auto *t : targets) scene->moveObjectToFolder(t, 0);
            }
            DrawMoveToFolderMenu(scene, 0, targets, 0);
            if (scene->getFolders().empty()) ImGui::TextDisabled("No folders yet");
        }
        ImGui::EndMenu();
    }
    ImGui::EndPopup();
}

void GUIAddonObjects3D::DrawFolderObjects(Scene *scene, int folderId, const std::string &filter)
{
    auto it = folderObjects.find(folderId);
    if (it == folderObjects.end()) return;
    auto list = it->second;
    std::sort(list.begin(), list.end(), [](Object3D *a, Object3D *b) { return a->getName() < b->getName(); });
    int shown = 0;
    for (auto *o : list) {
        if (maxListedObjects > 0 && shown >= maxListedObjects) break;
        DrawObjectWithCustomNode(o, folderRowIndex++);
        shown++;
    }
    DrawMoreObjects((int) list.size() - shown);
}

void GUIAddonObjects3D::DrawFolderNode(Scene *scene, const SceneFolder &folder, const std::string &filter)
{
    const int count = folderMatchCount[folder.id];
    const bool filtering = !filter.empty();
    if (filtering && count == 0) return;   // con filtro, solo las carpetas con coincidencias

    ImGui::PushID(folder.id);
    CustomImGui::CustomTreeNodeConfig cfg(folder.name.c_str());
    cfg.leftIcon      = FileSystemGUI::Icon(IconGUI::FOLDER);
    cfg.iconSize      = GUIType::Sizes::ICON_SIZE_MENUS;
    cfg.bulletOpen    = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    cfg.bulletClosed  = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    cfg.showChildCount = true;
    cfg.childCount    = count;
    cfg.itemPadding   = 1.0f;
    cfg.indentSpacing = 18.0f;
    bool forceOpen = filtering;
    cfg.forceOpenPtr  = &forceOpen;
    // Soltar objetos encima = meterlos en esta carpeta
    cfg.dragDrop = CustomImGui::TreeDragDropConfig(GUIType::DragDropTarget::OBJECT_ITEM, [scene, &folder](void *data) {
        auto *dragged = Brakeza::get()->getObjectById(*(unsigned int *) data);
        for (auto *t : dragTargets(dragged)) scene->moveObjectToFolder(t, folder.id);
    });
    if (count == 0) cfg.textColor = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);

    // Arrastrar la carpeta y su menú contextual (sobre el botón principal de la fila)
    const int folderId = folder.id;
    const std::string folderName = folder.name;
    const int folderParent = folder.parent;
    bool deleted = false;
    cfg.onNodeItem = [&]() {
        if (ImGui::BeginDragDropSource()) {
            FolderDragPayload fp{ scene, folderId };
            ImGui::SetDragDropPayload(GUIType::DragDropTarget::FOLDER_ITEM, &fp, sizeof(fp));
            ImGui::Text("Folder: %s", folderName.c_str());
            ImGui::EndDragDropSource();
        }
        ContextMenuStyle menuStyle;
        if (ImGui::BeginPopupContextItem("##folderctx")) {
            ImGui::TextDisabled("%s", folderName.c_str());
            ImGui::Separator();
            if (ImGui::MenuItem("New subfolder"))   requestFolderModal(scene, 0, folderId, "New folder");
            if (ImGui::MenuItem("Rename"))          requestFolderModal(scene, folderId, folderParent, folderName);
            if (ImGui::MenuItem("Select contents")) {
                std::vector<Object3D *> objs;
                collectFolderObjects(scene, folderId, objs);
                auto *render = Components::get()->Render();
                render->clearSelection();
                for (auto *o : objs) render->addToSelection(o);
            }
            if (ImGui::MenuItem("Hide contents") ) {
                std::vector<Object3D *> objs;
                collectFolderObjects(scene, folderId, objs);
                for (auto *o : objs) o->setEnabled(false);
            }
            if (ImGui::MenuItem("Show contents")) {
                std::vector<Object3D *> objs;
                collectFolderObjects(scene, folderId, objs);
                for (auto *o : objs) o->setEnabled(true);
            }
            if (folderParent != 0 && ImGui::MenuItem("Move to root")) scene->moveFolder(folderId, 0);
            ImGui::Separator();
            if (ImGui::MenuItem("Delete folder (keeps its objects)")) {
                scene->deleteFolder(folderId);
                deleted = true;
            }
            ImGui::EndPopup();
        }
    };

    const bool open = CustomImGui::CustomTreeNode(cfg, nullptr);

    // Soltar una carpeta encima = moverla dentro (nunca dentro de sí misma)
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *pl = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::FOLDER_ITEM)) {
            auto *fp = (FolderDragPayload *) pl->Data;
            if (fp->scene == scene) scene->moveFolder(fp->id, folderId);
        }
        ImGui::EndDragDropTarget();
    }

    if (open) {
        if (!deleted) {
            std::vector<SceneFolder> children;
            for (const auto &f : scene->getFolders()) if (f.parent == folderId) children.push_back(f);
            std::sort(children.begin(), children.end(), [](const SceneFolder &a, const SceneFolder &b) { return a.name < b.name; });
            for (const auto &c : children) DrawFolderNode(scene, c, filter);
            DrawFolderObjects(scene, folderId, filter);
        }
        CustomImGui::CustomTreePop(cfg.indentSpacing);
    }
    ImGui::PopID();
}

void GUIAddonObjects3D::DrawObjectsFolders(Scene *scene, const std::vector<Object3D *> &objects, const std::string &filter)
{
    if (scene == nullptr) {
        Drawable::WarningMessage("Load a scene to organize its objects in folders");
        return;
    }

    // Objetos del frame por carpeta (con el filtro aplicado) y recuento acumulado por subárbol
    folderObjects.clear();
    folderMatchCount.clear();
    folderRowIndex = 0;
    int total = 0;
    for (auto *o : objects) {
        if (o->isRemoved()) continue;
        if (o->getScene() != nullptr && o->getScene() != scene) continue;
        if (!filter.empty() && !exist(o->getName(), filter)) continue;
        int fid = o->getFolderId();
        if (fid != 0 && scene->getFolder(fid) == nullptr) fid = 0;   // carpeta inexistente -> raíz
        folderObjects[fid].push_back(o);
        total++;
        for (int f = fid, guard = 0; f != 0 && guard < 1024; guard++) {   // suma en la carpeta y sus padres
            folderMatchCount[f]++;
            const SceneFolder *sf = scene->getFolder(f);
            f = sf ? sf->parent : 0;
        }
    }
    if (!filter.empty() && total == 0) {
        Drawable::WarningMessage("No objects match this search");
        return;
    }

    std::vector<SceneFolder> roots;
    for (const auto &f : scene->getFolders()) if (f.parent == 0) roots.push_back(f);
    std::sort(roots.begin(), roots.end(), [](const SceneFolder &a, const SceneFolder &b) { return a.name < b.name; });
    for (const auto &f : roots) DrawFolderNode(scene, f, filter);

    // Raíz: objetos sin carpeta. Soltar aquí = sacar de su carpeta (objetos) / subir a la raíz (carpetas)
    ImGui::PushID("##rootfolder");
    CustomImGui::CustomTreeNodeConfig cfg("(no folder)");
    cfg.leftIcon       = FileSystemGUI::Icon(IconGUI::FOLDER);
    cfg.iconSize       = GUIType::Sizes::ICON_SIZE_MENUS;
    cfg.iconTint       = ImVec4(0.6f, 0.6f, 0.6f, 0.8f);
    cfg.bulletOpen     = FileSystemGUI::Icon(IconGUI::TREE_BULLET_ON);
    cfg.bulletClosed   = FileSystemGUI::Icon(IconGUI::TREE_BULLET_OFF);
    cfg.showChildCount = true;
    cfg.childCount     = (int) folderObjects[0].size();
    cfg.defaultOpen    = true;
    cfg.indentSpacing  = 18.0f;
    cfg.dragDrop = CustomImGui::TreeDragDropConfig(GUIType::DragDropTarget::OBJECT_ITEM, [scene](void *data) {
        auto *dragged = Brakeza::get()->getObjectById(*(unsigned int *) data);
        for (auto *t : dragTargets(dragged)) scene->moveObjectToFolder(t, 0);
    });
    const bool open = CustomImGui::CustomTreeNode(cfg, nullptr);
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *pl = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::FOLDER_ITEM)) {
            auto *fp = (FolderDragPayload *) pl->Data;
            if (fp->scene == scene) scene->moveFolder(fp->id, 0);
        }
        ImGui::EndDragDropTarget();
    }
    if (open) {
        DrawFolderObjects(scene, 0, filter);
        CustomImGui::CustomTreePop(cfg.indentSpacing);
    }
    ImGui::PopID();
}

// Ventana de nombre: nueva carpeta / renombrar (se pide con requestFolderModal)
void GUIAddonObjects3D::DrawFolderModal()
{
    if (folderModal.request) {
        ImGui::OpenPopup("Folder name##foldermodal");
        folderModal.request = false;
    }
    if (ImGui::BeginPopupModal("Folder name##foldermodal", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();
        bool enter = false;
        {
            GUIFields::Section folderName("FOLDER", 90.0f, 220.0f);
            if (folderName) {
                enter = GUIFields::String("Name", folderModal.name, sizeof(folderModal.name), nullptr,
                                          ImGuiInputTextFlags_EnterReturnsTrue);
            }
        }
        ImGui::Spacing();
        const bool ok = ImGui::Button(folderModal.folderId > 0 ? "Rename" : "Create") || enter;
        ImGui::SameLine();
        const bool cancel = GUIHelpers::CancelButton() || ImGui::IsKeyPressed(ImGuiKey_Escape);
        bool close = cancel;
        if (ok && folderModal.scene != nullptr && folderModal.name[0] != '\0') {
            // No se permiten dos carpetas con el mismo nombre en el mismo nivel: se avisa y la ventana sigue abierta
            const bool done = folderModal.folderId > 0
                ? folderModal.scene->renameFolder(folderModal.folderId, folderModal.name)
                : folderModal.scene->createFolder(folderModal.name, folderModal.parent) != 0;
            folderModal.duplicate = !done;
            close = close || done;
        } else if (ok) {
            close = true;   // nombre vacío: se cierra sin hacer nada, como antes
        }
        if (folderModal.duplicate) {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.3f, 1.0f), "A folder with that name already exists here");
        }
        if (close) {
            folderModal.duplicate = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

// ============================================================================
// DrawObjectsTree - Solo categorías con bullets
// ============================================================================
void GUIAddonObjects3D::DrawObjectsTree(GUIManager *gui, const std::vector<Object3D *> &objects, const std::string &filter)
{
    int globalIndex = 0;

    if (!filter.empty()) {
        bool anyMatch = false;
        for (const auto &o : objects) {
            if (!o->isRemoved() && exist(o->getName(), filter)) {
                anyMatch = true;
                break;
            }
        }
        if (!anyMatch) {
            Drawable::WarningMessage("No objects match this search");
            return;
        }
    }

    // Sin objetos (o todos ocultos por el filtro de tipos): mismo aviso que el listado
    bool anyObject = false;
    for (const auto &o : objects) {
        if (!o->isRemoved() && isObjectTypeVisible(gui, o->getTypeObject())) { anyObject = true; break; }
    }
    if (!anyObject) {
        Drawable::WarningMessage("Empty list");
        return;
    }

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
                if (maxListedObjects > 0 && localCount >= maxListedObjects) break;
                DrawObjectWithCustomNode(o, globalIndex);

                globalIndex++;
                localCount++;
            }
            DrawMoreObjects((int) sorted.size() - localCount);

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

    int shown = 0, hidden = 0;
    for (unsigned int i = 0; i < (unsigned int) objects.size(); i++) {
        auto &o = objects[i];

        if (o->isRemoved()) continue;
        if (!isObjectTypeVisible(gui, o->getTypeObject())) continue;
        if (!filter.empty() && !exist(o->getName(), filter)) continue;

        if (maxListedObjects > 0 && shown >= maxListedObjects) { hidden++; continue; }
        DrawObjectWithCustomNode(o, i);
        shown++;
    }
    if (!filter.empty() && shown == 0 && hidden == 0 && !objects.empty()) {
        Drawable::WarningMessage("No objects match this search");
    }
    DrawMoreObjects(hidden);
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

    // Línea 1: modos de vista y, a su lado, solo lo que aplica al modo activo
    GUI::DrawButton("List mode", IconGUI::OBJECTS_VIEWER_LIST, GUIType::Sizes::ICON_SIZE_MENUS,
        type == GUIType::ViewerObjectsMode::LIST, [&] {
        gui->setObjectsViewerMode(GUIType::ViewerObjectsMode::LIST);
    });
    ImGui::SameLine();
    GUI::DrawButton("Tree mode", IconGUI::OBJECTS_VIEWER_TREE, GUIType::Sizes::ICON_SIZE_MENUS,
        type == GUIType::ViewerObjectsMode::TREE, [&] {
        gui->setObjectsViewerMode(GUIType::ViewerObjectsMode::TREE);
    });
    ImGui::SameLine();
    GUI::DrawButton("Folders mode", IconGUI::FOLDER, GUIType::Sizes::ICON_SIZE_MENUS,
        type == GUIType::ViewerObjectsMode::FOLDERS, [&] {
        gui->setObjectsViewerMode(GUIType::ViewerObjectsMode::FOLDERS);
    });
    if (type == GUIType::ViewerObjectsMode::TREE) {
        // Solo el modo árbol tiene grupos (por tipo de objeto) que puedan quedar vacíos
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();
        GUI::DrawButton("Hide empty groups", IconGUI::OBJECTS_VIEWER_EMPTY_GROUPS, GUIType::Sizes::ICON_SIZE_MENUS,
            hideEmptyGroups, [&] {
            hideEmptyGroups = !hideEmptyGroups;
        });
    }
    if (type == GUIType::ViewerObjectsMode::FOLDERS) {
        ImGui::SameLine();
        ImGui::TextDisabled("|");
        ImGui::SameLine();
        GUI::DrawButton("New folder at the root of the scene", IconGUI::FOLDER_ADD, GUIType::Sizes::ICON_SIZE_MENUS, false, [&] {
            if (auto *s = FolderTargetScene()) requestFolderModal(s, 0, 0, "New folder");
        });
    }

    // Línea 2: límite de la lista a la izquierda, filtro a la derecha
    if (ImGui::BeginTable("ControlsTable", 2, ImGuiTableFlags_SizingStretchProp)) {
        ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::SetNextItemWidth(60);
        if (ImGui::InputInt("Max", &maxListedObjects, 0)) maxListedObjects = std::max(0, maxListedObjects);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Objects listed (per group). 0 = no limit");

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
                    case GUIType::ViewerObjectsMode::FOLDERS:
                        DrawObjectsFolders(scene, scene->getObjects(), filterGUI);
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
            case GUIType::ViewerObjectsMode::FOLDERS:
                DrawObjectsFolders(FolderTargetScene(), gameObjects, filterGUI);
                break;
            default:
                DrawObjectList(gui, gameObjects, filterGUI);
        }
    }
    DrawFolderModal();
}