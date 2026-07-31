#include <filesystem>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../../../include/GUI/Objects/UIManagerGUI.h"
#include "../../../include/Config.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUI.h"
#include "../../../include/GUI/AddOns/GUIAddonFilePicker.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Misc/cJSON.h"
#include "imgui.h"

static GUIFilePicker s_pickerElImg;
static GUIFilePicker s_pickerOptImg;
static int           s_pickerOptIdx = -1;

int UIManagerGUI::selectedWidget  = -1;
int UIManagerGUI::selectedElement = -1;
GLuint UIManagerGUI::previewFBO      = 0;
GLuint UIManagerGUI::previewColorTex = 0;
GLuint UIManagerGUI::previewDepthRB  = 0;
UIWidgetRenderData UIManagerGUI::previewData;
float  UIManagerGUI::previewZoom         = 1.0f;
ImVec2 UIManagerGUI::previewPan          = {0.0f, 0.0f};

// ─── Reusable field helpers ───────────────────────────────────────────────
// Extracted from the ~15 ColorEdit4 / ~20 InputText / 2 image-picker blocks
// that were previously duplicated inline across DrawElementEditor / SaveWidget helpers.

bool UIManagerGUI::Color4Field(const char* label, Color& c, bool alphaBar)
{
    ImGuiColorEditFlags flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop;
    if (alphaBar) flags |= ImGuiColorEditFlags_AlphaBar;
    float v[4] = { c.r, c.g, c.b, c.a };
    if (ImGui::ColorEdit4(label, v, flags)) {
        c = Color(v[0], v[1], v[2], v[3]);
        return true;
    }
    return false;
}

bool UIManagerGUI::StringField(const char* label, std::string& str, float width, int bufSize)
{
    // Stack buffer capped at 1024 — every existing caller used 64/128/256/512.
    if (bufSize > 1024) bufSize = 1024;
    char buf[1024];
    const int n = std::min((int)str.size(), bufSize - 1);
    std::memcpy(buf, str.data(), n);
    buf[n] = '\0';
    if (width > 0.0f) ImGui::SetNextItemWidth(width);
    if (ImGui::InputText(label, buf, (size_t)bufSize)) {
        str = buf;
        return true;
    }
    return false;
}

bool UIManagerGUI::ImagePickerField(
    const char* label,
    std::string& path,
    GUIFilePicker& picker,
    const char* pickerTitle,
    float inputWidth)
{
    bool changed = false;
    if (StringField(label, path, inputWidth)) changed = true;
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
            path = std::string((const char*)p->Data);
            changed = true;
        }
        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine();
    // Trigger + picker draw. Caller owns the picker (may share state across options).
    std::string triggerId = std::string("##browse_") + label;
    if (picker.drawTrigger(triggerId.c_str()))
        picker.open(path, Config::get()->IMAGES_FOLDER, "../assets/images/", {".png", ".jpg", ".jpeg", ".bmp", ".tga"});
    if (picker.draw(pickerTitle)) {
        path = picker.result;
        changed = true;
    }
    return changed;
}

void UIManagerGUI::BorderEditorSection(UIElement& el)
{
    ImGui::SetNextItemWidth(80);
    ImGui::DragFloat("Border width px##elbdrw", &el.borderWidth, 0.5f, 0.0f, 20.0f, "%.0f");
    Color4Field("Normal##elbdrcol",  el.borderColor);        ImGui::SameLine();
    Color4Field("Hover##elbdrcolhv", el.borderColorHover);   ImGui::SameLine();
    Color4Field("Press##elbdrcolpr", el.borderColorPressed);
}

void UIManagerGUI::setSelectedWidget(const std::string& name)
{
    auto* render = Components::get()->Render();
    auto* ui = render->getUIManager();
    if (!ui) return;

    auto& widgets = ui->getWidgets();
    std::vector<std::string> names;
    names.reserve(widgets.size());
    for (auto& [k, _] : widgets) names.push_back(k);
    std::sort(names.begin(), names.end());

    auto it = std::find(names.begin(), names.end(), name);
    selectedWidget  = (it != names.end()) ? (int)(it - names.begin()) : -1;
    selectedElement = -1;
}

// ---------------------------------------------------------------------------
void UIManagerGUI::DrawWinUIManager()
{
    auto* render = Components::get()->Render();
    auto* ui     = render->getUIManager();
    if (!ui) { ImGui::TextDisabled("UIManager not initialised"); return; }

    auto& widgets = const_cast<std::unordered_map<std::string, UIWidget>&>(ui->getWidgets());

    // Build sorted name list for stable display order
    std::vector<std::string> names;
    names.reserve(widgets.size());
    for (auto& [k, _] : widgets) names.push_back(k);
    std::sort(names.begin(), names.end());

    // ── Col 1 (split: top = list, bottom = widget setup) ─────────────────
    float colH = ImGui::GetContentRegionAvail().y;
    ImGui::BeginChild("##col1", ImVec2(360.0f, colH), false);

    // ── helper: relative label from file path ─────────────────────────────
    auto makeLabel = [&](int i) -> std::string {
        const std::string& fp = widgets[names[i]].filePath;
        std::string label = names[i];
        for (auto [prefix, skip] : std::initializer_list<std::pair<const char*,size_t>>{
                {"assets/ui\\",10},{"assets\\ui\\",10},{"assets/ui/",10},{"assets\\ui\\",10},
                {"assets/",7},{"assets\\",7}}) {
            auto pos = fp.find(prefix);
            if (pos != std::string::npos) {
                label = fp.substr(pos + skip);
                if (label.size() > 5) label.resize(label.size() - 5);
                for (char& c : label) if (c == '\\') c = '/';
                break;
            }
        }
        return label;
    };

    // ── top sub-panel: list ───────────────────────────────────────────────
    const float SETUP_H = 215.0f;
    float listPanelH = colH - SETUP_H - ImGui::GetStyle().ItemSpacing.y * 3.0f - ImGui::GetStyle().SeparatorTextBorderSize;
    ImGui::BeginChild("##col1_list", ImVec2(-1.0f, listPanelH), false);

    ImGui::SeparatorText("Loaded widgets");

    static char widgetFilter[128] = "";
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##wfilter", "Filter...", widgetFilter, sizeof(widgetFilter));

    std::string filterLow = widgetFilter;
    for (char& c : filterLow) c = (char)tolower((unsigned char)c);

    std::vector<int> filtered;
    filtered.reserve(names.size());
    for (int i = 0; i < (int)names.size(); i++) {
        if (filterLow.empty()) { filtered.push_back(i); continue; }
        std::string lbl = makeLabel(i);
        for (char& c : lbl) c = (char)tolower((unsigned char)c);
        if (lbl.find(filterLow) != std::string::npos) filtered.push_back(i);
    }

    float listH = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing() - 4.0f;
    ImGui::BeginChild("##wlist", ImVec2(-1.0f, listH), true);
    static int prevSelectedWidget = -1;
    for (int fi = 0; fi < (int)filtered.size(); fi++) {
        int i   = filtered[fi];
        bool sel = (i == selectedWidget);
        char entry[512];
        snprintf(entry, sizeof(entry), "%2d  %s##wsel%d", i + 1, makeLabel(i).c_str(), i);
        if (ImGui::Selectable(entry, sel)) {
            selectedWidget  = i;
            selectedElement = -1;
        }
    }
    if (selectedWidget != prevSelectedWidget) {
        prevSelectedWidget = selectedWidget;
        previewData.clear();
        if (selectedWidget >= 0 && selectedWidget < (int)names.size())
            previewData = widgets[names[selectedWidget]].defaultData;
    }
    ImGui::EndChild();

    GUI::ImageButtonSmall(IconGUI::CREATE_FILE, "New", [&]{
        std::string newName = "newWidget";
        int n = 0;
        while (widgets.count(newName + (n ? std::to_string(n) : ""))) n++;
        if (n) newName += std::to_string(n);
        std::string path = ui->getWidgetsDir() + newName + ".json";
        cJSON* root = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "elements", cJSON_CreateArray());
        char* txt = cJSON_Print(root);
        FILE* f = fopen(path.c_str(), "w");
        if (f) { fputs(txt, f); fclose(f); }
        free(txt); cJSON_Delete(root);
        ui->reloadWidgets();
        selectedWidget = -1; selectedElement = -1;
    });
    if (selectedWidget >= 0 && selectedWidget < (int)names.size()) {
        ImGui::SameLine();
        GUI::ImageButtonSmall(IconGUI::SCENE_UNLOAD, "Unload", [&]{
            ui->unloadWidget(names[selectedWidget]);
            selectedWidget = -1; selectedElement = -1;
        });
        ImGui::SameLine();
        GUI::ImageButtonSmallConfirm(IconGUI::REMOVE, "Delete", "Borrar widget", "Eliminar el archivo JSON del disco?", [&]{
            std::filesystem::remove(ui->getWidgetsDir() + names[selectedWidget] + ".json");
            ui->reloadWidgets();
            selectedWidget = -1; selectedElement = -1;
        });
    }
    ImGui::SameLine();
    GUI::ImageButtonSmallConfirm(IconGUI::CLEAR_SCENE, "Clear all", "Limpiar widgets", "Descargar todos los widgets de memoria?", [&]{
        ui->clearWidgets(); selectedWidget = -1; selectedElement = -1;
    });

    ImGui::EndChild(); // ##col1_list

    // ── bottom sub-panel: widget general setup ────────────────────────────
    ImGui::Separator();
    ImGui::BeginChild("##col1_setup", ImVec2(-1.0f, -1.0f), false);

    bool savedWidget = false;
    static bool openPreviewPopup = false;

    if (selectedWidget >= 0 && selectedWidget < (int)names.size()) {
        const std::string& wName = names[selectedWidget];
        UIWidget& w = widgets[wName];

        ImGui::SeparatorText(wName.c_str());

        if (GUI::ImageButtonSmall(IconGUI::SAVE, "Save widget", []{})) {
            SaveWidget(ui, wName, w);
            savedWidget = true;
        }
        ImGui::SameLine();
        if (GUI::ImageButtonSmall(IconGUI::SOLO_WINDOW_ON, "Preview", []{}))
            openPreviewPopup = true;
        ImGui::SameLine();
        {
            const bool active = ui->getDebugHighlightName() == wName;
            if (active) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.90f, 0.55f, 0.05f, 0.85f));
            if (GUI::ImageButtonSmall(IconGUI::SEARCH, active ? "Debug: ON" : "Debug", []{}))
                ui->setDebugHighlightName(active ? std::string() : wName);
            if (active) ImGui::PopStyleColor();
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Highlight this widget wherever it renders in the scene\n(includes nested renders via widgetRef / array).");
        }

        if (!savedWidget) {
            ImGui::SetNextItemWidth(70.0f);
            if (ImGui::DragFloat("Scale##wscale", &w.scale, 0.01f, 0.1f, 5.0f, "%.2f"))
                w.scale = std::max(0.1f, std::min(5.0f, w.scale));

            ImGui::SetNextItemWidth(90.0f);
            ImGui::DragFloat("X%%##wposx", &w.posX, 0.001f, 0.0f, 1.0f, "%.3f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(90.0f);
            ImGui::DragFloat("Y%%##wposy", &w.posY, 0.001f, 0.0f, 1.0f, "%.3f");

            ImGui::SetNextItemWidth(90.0f);
            ImGui::DragFloat("offX px##woffx", &w.offsetX, 0.5f, -4000.0f, 4000.0f, "%.0f");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Pixel offset added to the computed X position");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(90.0f);
            ImGui::DragFloat("offY px##woffy", &w.offsetY, 0.5f, -4000.0f, 4000.0f, "%.0f");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Pixel offset added to the computed Y position");

            ImGui::SetNextItemWidth(80.0f);
            ImGui::DragFloat("W%%##wrefW", &w.width,  0.001f, 0.0f, 1.0f, "%.3f");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Widget width as fraction of window width (0..1)");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80.0f);
            ImGui::DragFloat("H%%##wrefH", &w.height, 0.001f, 0.0f, 1.0f, "%.3f");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Widget height as fraction of window height (0..1)");

            Color4Field("BG##wbg",     w.bgColor,     true);
            Color4Field("Border##wbc", w.borderColor, true);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(60.0f);
            ImGui::DragFloat("px##wbw", &w.borderWidth, 0.5f, 0.0f, 20.0f, "%.0f");

            if (!w.cacheable) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.6f, 0.1f, 1.0f));
            ImGui::Checkbox("Cacheable##wcacheable", &w.cacheable);
            if (!w.cacheable) { ImGui::PopStyleColor(); ImGui::SameLine(); ImGui::TextDisabled("(interactive — no FBO cache)"); }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("ON: widget rendered once per data-change, blitted from FBO.\nOFF: re-rendered every frame (use for buttons with hover/press state).");
            if (ImGui::IsItemEdited()) w.cacheDirty = true;
        }
    } else {
        ImGui::TextDisabled("No widget selected");
    }

    ImGui::EndChild(); // ##col1_setup
    ImGui::EndChild(); // ##col1

    if (savedWidget || names.empty() || selectedWidget < 0 || selectedWidget >= (int)names.size())
        return;

    // ── Col 2: elements + editor ──────────────────────────────────────────
    ImGui::SameLine();

    const std::string& wName = names[selectedWidget];
    UIWidget& w = widgets[wName];
    float col2W = ImGui::GetContentRegionAvail().x - 10.0f;

    ImGui::BeginChild("##col2", ImVec2(col2W, colH), false);
    ImGui::SeparatorText("Elements");
    DrawElementList(w);
    ImGui::Spacing();

    if (selectedElement >= 0 && selectedElement < (int)w.elements.size()) {
        UIElement& el = w.elements[selectedElement];
        DrawElementEditor(el, w, ui, wName);
        if (el.type != "text" && el.type != "rect" && el.type != "button" && el.type != "image" && el.type != "array"
            && ImGui::CollapsingHeader("Preview Data", ImGuiTreeNodeFlags_None))
            DrawElementPreviewFields(el);
    }

    ImGui::EndChild(); // ##col2

    // ── Preview popup ─────────────────────────────────────────────────────
    if (openPreviewPopup) {
        ImGui::OpenPopup("##widget_preview_popup");
        openPreviewPopup = false;
    }
    ImGui::SetNextWindowSize(ImVec2(440.0f, 800.0f), ImGuiCond_FirstUseEver);
    if (ImGui::BeginPopup("##widget_preview_popup")) {
        DrawWidgetPreview(ui, wName);
        ImGui::EndPopup();
    }
}

// ---------------------------------------------------------------------------
void UIManagerGUI::DrawWidgetList(UIManager*) {}  // unused, kept for symmetry

void UIManagerGUI::DrawElementList(UIWidget& w)
{
    ImGui::BeginChild("##ellist", ImVec2(0, 200), true);
    for (int i = 0; i < (int)w.elements.size(); i++) {
        auto& el = w.elements[i];
        std::string label = "[" + el.type + "] " + el.id + "##el" + std::to_string(i);
        bool sel = (i == selectedElement);
        if (!el.enabled) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f, 0.2f, 0.2f, 1.0f));
        if (ImGui::Selectable(label.c_str(), sel))
            selectedElement = i;
        if (!el.enabled) ImGui::PopStyleColor();
    }
    ImGui::EndChild();

    const float addBtnW = GUIType::Sizes::ICON_LOCKS.x + 6.0f + ImGui::CalcTextSize("Add").x + ImGui::GetStyle().FramePadding.x * 2.0f;
    const float rowMaxX = ImGui::GetContentRegionAvail().x - addBtnW;

    bool hasSelection = selectedElement >= 0 && selectedElement < (int)w.elements.size();
    if (hasSelection) {
        int i = selectedElement;

        ImGui::BeginDisabled(i == 0);
        GUI::DrawButton("Move up", IconGUI::ARROW_UP, ImVec2(12, 12), false, [&]{
            std::swap(w.elements[i], w.elements[i-1]); selectedElement = i-1;
        });
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled(i >= (int)w.elements.size()-1);
        GUI::DrawButton("Move down", IconGUI::ARROW_DOWN, ImVec2(12, 12), false, [&]{
            std::swap(w.elements[i], w.elements[i+1]); selectedElement = i+1;
        });
        ImGui::EndDisabled();

        ImGui::SameLine();
        GUI::DrawButton("Delete element", IconGUI::REMOVE, ImVec2(12, 12), false, [&]{
            w.elements.erase(w.elements.begin() + i);
            selectedElement = -1;
        });

        ImGui::SameLine(rowMaxX);
    } else {
        ImGui::SetCursorPosX(rowMaxX);
    }

    GUI::ImageButtonSmall(IconGUI::CREATE_FILE, "Add", [&]{
        UIElement el;
        el.id   = "element" + std::to_string(w.elements.size());
        el.type = "text";
        w.elements.push_back(el);
        selectedElement = (int)w.elements.size() - 1;
    });
}

// ---------------------------------------------------------------------------
void UIManagerGUI::DrawElementEditor(UIElement& el, UIWidget& w, UIManager* ui, const std::string& widgetName)
{
    static const char* types[] = { "text", "image", "rect", "progressbar", "icons", "button", "widget", "array" };

    constexpr float spatialStep = 0.001f;
    constexpr const char* spatialFmt = "%.3f";

    // Helper: small reset button to the left of static string fields
    auto resetStrBtn = [&](const char* uid, std::string& str) {
        ImGui::PushID(uid);
        if (ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::REMOVE), ImVec2(12, 12))) str = "";
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset to default");
        ImGui::PopID();
        ImGui::SameLine();
    };

    // ── Section 1: Identity & Type ────────────────────────────────────────
    if (ImGui::CollapsingHeader("Identity & Type", ImGuiTreeNodeFlags_None)) {
        ImGui::Checkbox("enabled##elenabled", &el.enabled);
        ImGui::SameLine();
        StringField("id##elid", el.id, 160.0f, 128);

        int typeIdx = 0;
        for (int i = 0; i < 8; i++) if (el.type == types[i]) { typeIdx = i; break; }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(110);
        if (ImGui::Combo("type##eltype", &typeIdx, types, 8)) el.type = types[typeIdx];
    }

    // ── Section 2: Layout ─────────────────────────────────────────────────
    if (ImGui::CollapsingHeader("Layout", ImGuiTreeNodeFlags_None)) {
        ImGui::Checkbox("yAuto##elyauto", &el.yAuto);
        ImGui::SameLine();
        if (el.type == "array")
            ImGui::TextDisabled("(auto Y in parent — places this array block below the previous element)");
        else
            ImGui::TextDisabled("(auto Y in parent — places this element below the previous one)");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip(
            "Controls where THIS element sits inside its parent widget.\n"
            "This is NOT about the array's own internal alignment.\n\n"
            "ON  -> Y is computed automatically: element starts right\n"
            "       below the previous element in the parent widget.\n"
            "       The x/y fields are ignored.\n\n"
            "OFF -> Y is taken from the 'y' field (fixed pixel offset\n"
            "       from the top of the parent widget).");

        if (!el.yAuto) {
            ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("x %%##elx", &el.x, 0.001f, 0.0f, 0.0f, "%.3f");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("y %%##ely", &el.y, 0.001f, 0.0f, 0.0f, "%.3f");
        }
        if (el.type != "progressbar") {
            ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("w %%##elw", &el.w, 0.001f, 0.0f, 0.0f, "%.3f");
            if (el.type != "text") {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("h %%##elh", &el.h, 0.001f, 0.0f, 0.0f, "%.3f");
            }
        }
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("padL %%##elpL", &el.paddingLeft,   0.001f, 0.0f, 0.0f, "%.3f");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("PaddingLeft — shifts element right (left side guaranteed).");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("padT %%##elpT", &el.paddingTop,    0.001f, 0.0f, 0.0f, "%.3f");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("PaddingTop — pre-element space (yAuto) / adds to y (fixed).");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("padB %%##elpB", &el.paddingBottom, 0.001f, 0.0f, 0.0f, "%.3f");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("PaddingBottom — post-element space (yAuto) / adds to effective height (fixed).");
    }

    // ── Section 3: Type-specific properties ───────────────────────────────
    // Trailing "##typespec" keeps the ID stable across type changes, so the
    // header's open/closed state persists when the user swaps the type combo.
    std::string typeHdr = (el.type.empty() ? std::string("Type") : el.type) + " properties##typespec";
    if (!ImGui::CollapsingHeader(typeHdr.c_str(), ImGuiTreeNodeFlags_None)) return;

    if (el.type == "text") {
        ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("fontScale##elfs", &el.fontScale, 0.01f, 0.0f, 10.0f, "%.2f");
        Color4Field("Color##eltxtcolor", el.staticTextColor);
        {
            const char* alignItems[] = { "left", "center", "right" };
            int alignIdx = (el.textAlign == "center") ? 1 : (el.textAlign == "right") ? 2 : 0;
            ImGui::SetNextItemWidth(90.0f);
            if (ImGui::Combo("textAlign##eltxtal", &alignIdx, alignItems, IM_ARRAYSIZE(alignItems)))
                el.textAlign = alignItems[alignIdx];
            if (el.w <= 0.0f && alignIdx != 0) {
                ImGui::SameLine(); ImGui::TextDisabled("(needs w > 0)");
            }
        }
        {
            resetStrBtn("rsttxt", el.staticText);
            StringField("text##eltxtst", el.staticText, 260.0f, 256);
            ImGui::SameLine(); ImGui::TextDisabled("(static, overridden by Lua)");
        }
    }
    else if (el.type == "image") {
        ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("imageScale##elis", &el.imageScale, 0.05f, 0.0f, 10.0f, "%.2f");
        {
            resetStrBtn("rstimgpath", el.staticImagePath);
            ImGui::Image(FileSystemGUI::Icon(IconGUI::DRAGGABLE_UI_ITEM), ImVec2(12, 12)); ImGui::SameLine();
            ImagePickerField("imagePath##elimgpath", el.staticImagePath, s_pickerElImg, "UIEl_Image_FilePicker", 260.0f);
            if (!el.staticImagePath.empty()) {
                GLuint tid = Components::get()->Render()->getImageGLTexture(el.staticImagePath);
                if (tid) { ImGui::SameLine(); ImGui::Image((ImTextureID)(intptr_t)tid, ImVec2(32, 32)); }
            }
        }
        {
            StringField("tooltip##elimgtt", el.tooltip, 360.0f, 256);
            ImGui::SameLine(); ImGui::TextDisabled("(shown on hover)");
        }
        BorderEditorSection(el);
    }
    else if (el.type == "rect") {
        Color4Field("Color##elrcolor", el.rectColor);

        ImGui::SetNextItemWidth(80.0f);
        if (ImGui::DragFloat("alpha##elralpha", &el.alpha, 0.01f, 0.0f, 1.0f, "%.2f"))
            el.alpha = std::max(0.0f, std::min(1.0f, el.alpha));
        BorderEditorSection(el);
    }
    else if (el.type == "progressbar") {
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("barW %%##elbw", &el.barW, spatialStep, 0.0f, 1.0f, spatialFmt);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("barH %%##elbh", &el.barH, spatialStep, 0.0f, 1.0f, spatialFmt);
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("txtOffX %%##eltox", &el.textOffsetX, spatialStep, -1.0f, 1.0f, spatialFmt);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("txtOffY %%##eltoy", &el.textOffsetY, spatialStep, -1.0f, 1.0f, spatialFmt);
        ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("barFontScale##elbfs", &el.barFontScale, 0.01f, 0.0f, 10.0f, "%.2f");
        Color4Field("Text color##elpbcolor", el.staticTextColor);
        DrawProgressBarColors(el);
    }
    else if (el.type == "icons") {
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("iconSize %%##elis", &el.iconSize, spatialStep, 0.0f, 1.0f, spatialFmt);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("iconGap %%##eig",  &el.iconGap,  spatialStep, 0.0f, 1.0f, spatialFmt);
        DrawIconMappingEditor(el);
    }
    else if (el.type == "button") {
        ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("fontScale##elbtnfs", &el.fontScale, 0.01f, 0.0f, 10.0f, "%.2f");
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("iconSize %%##elbtnis", &el.iconSize, spatialStep, 0.0f, 1.0f, spatialFmt);
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("imgOffX %%##elbtniox", &el.btnImgOffsetX, spatialStep, -1.0f, 1.0f, spatialFmt);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("imgOffY %%##elbtniy",  &el.btnImgOffsetY, spatialStep, -1.0f, 1.0f, spatialFmt);
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("txtOffX %%##elbtntox", &el.btnTextOffsetX, spatialStep, -1.0f, 1.0f, spatialFmt);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("txtOffY %%##elbtntoy", &el.btnTextOffsetY, spatialStep, -1.0f, 1.0f, spatialFmt);
        {
            StringField("tooltip##elbtntt", el.tooltip, 360.0f, 256);
            ImGui::SameLine(); ImGui::TextDisabled("(shown on hover)");
        }
        {
            StringField("sound##elbtnsd", el.sound, 200.0f, 128);
            ImGui::SameLine(); ImGui::TextDisabled("(hover sound ID)");
        }
        Color4Field("Text color##elbtncolor", el.staticTextColor);

        ImGui::SeparatorText("Static Content");
        {
            resetStrBtn("rstbtntxt", el.staticText);
            StringField("text##elbtntxt", el.staticText, 200.0f, 128);
            ImGui::SameLine(); ImGui::TextDisabled("(fallback label if no option matches)");
        }

        ImGui::SeparatorText("Options");
        ImGui::TextDisabled("Lua: { key=\"mykey\" } -> renders matching option");

        int optToDelete = -1;
        for (int oi = 0; oi < (int)el.options.size(); oi++) {
            auto& opt = el.options[oi];
            ImGui::PushID(oi);

            // Header line: index label + key + text + color + delete
            ImGui::TextDisabled("#%d", oi);
            ImGui::SameLine();
            StringField("key##optk", opt.key, 80.0f, 64);
            ImGui::SameLine();
            StringField("text##optt", opt.text, 130.0f, 256);
            ImGui::SameLine();
            if (Color4Field("##optcol", opt.color, /*alphaBar*/ true)) opt.colorSet = true;
            ImGui::SameLine();
            GUI::DrawButton("Delete option", IconGUI::REMOVE, ImVec2(12, 12), false, [&]{ optToDelete = oi; });

            // Image path row
            {
                GLuint tid = opt.image.empty() ? 0 : Components::get()->Render()->getImageGLTexture(opt.image);
                if (tid) { ImGui::Image((ImTextureID)(intptr_t)tid, ImVec2(24, 24)); ImGui::SameLine(); }
                else { ImGui::Image(FileSystemGUI::Icon(IconGUI::DRAGGABLE_UI_ITEM), ImVec2(12, 12)); ImGui::SameLine(); }
                ImGui::TextDisabled("image"); ImGui::SameLine();
                StringField("##optimg", opt.image, 220.0f, 512);
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM))
                        opt.image = std::string((const char*)p->Data);
                    ImGui::EndDragDropTarget();
                }
                ImGui::SameLine();
                if (s_pickerOptImg.drawTrigger("##browse_optimg")) {
                    s_pickerOptIdx = oi;
                    s_pickerOptImg.open(opt.image, Config::get()->IMAGES_FOLDER, "../assets/images/", {".png", ".jpg", ".jpeg", ".bmp", ".tga"});
                }
            }
            if (s_pickerOptImg.draw("UIBtn_OptImg_FilePicker") && s_pickerOptIdx == oi)
                opt.image = s_pickerOptImg.result;

            // Tooltip rows
            {
                ImGui::TextDisabled("tooltip"); ImGui::SameLine();
                StringField("##opttt", opt.tooltip, 220.0f, 256);
            }
            {
                ImGui::TextDisabled("ttWidget"); ImGui::SameLine();
                StringField("##opttw", opt.tooltipWidget, 220.0f, 128);
            }

            if (oi < (int)el.options.size() - 1)
                ImGui::Separator();

            ImGui::PopID();
        }
        if (optToDelete >= 0) el.options.erase(el.options.begin() + optToDelete);
        GUI::ImageButtonSmall(IconGUI::CREATE_FILE, "Add option", [&]{
            UIElementOption newOpt;
            newOpt.key = "key" + std::to_string(el.options.size());
            el.options.push_back(newOpt);
        });

        ImGui::SeparatorText("Button Colors");
        Color4Field("Normal##cbtn",  el.btnBg);      ImGui::SameLine();
        Color4Field("Hover##cbtnhv", el.btnHover);   ImGui::SameLine();
        Color4Field("Press##cbtnpr", el.btnPressed);
        BorderEditorSection(el);
    }
    else if (el.type == "widget") {
        StringField("widgetRef##elwref", el.widgetRef, 200.0f, 128);
        ImGui::SameLine(); ImGui::TextDisabled("(widget name)");
    }
    else if (el.type == "array") {
        ImGui::SeparatorText("Array");

        StringField("widgetRef##elaref", el.widgetRef, 200.0f, 128);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Name of the child widget to repeat N times.");

        ImGui::SetNextItemWidth(80);
        ImGui::InputInt("count##elacnt", &el.arrayCount);
        if (el.arrayCount < 1) el.arrayCount = 1;
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Number of child widget copies to render.");

        static const char* aligns[] = { "vertical", "horizontal" };
        int aIdx = (el.arrayAlign == "horizontal") ? 1 : 0;
        ImGui::SameLine();
        ImGui::SetNextItemWidth(110);
        if (ImGui::Combo("align##elaalign", &aIdx, aligns, 2)) el.arrayAlign = aligns[aIdx];
        if (ImGui::IsItemHovered()) ImGui::SetTooltip(
            "vertical   -> items stacked top to bottom.\n"
            "horizontal -> items placed left to right.");

        ImGui::SeparatorText("Pagination");

        ImGui::Checkbox("paginate##elapag", &el.arrayPaginate);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip(
            "Enables pagination.\n"
            "arrayCount = items per page.\n"
            "Lua passes: count=total items, page=current page (0-based).\n"
            "Clicks return __prev / __next for nav buttons.");

        if (el.arrayPaginate) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(160);
            char pagerBuf[128] = {};
            std::strncpy(pagerBuf, el.arrayPagerWidget.c_str(), sizeof(pagerBuf) - 1);
            if (ImGui::InputText("pager widget##elapagw", pagerBuf, sizeof(pagerBuf)))
                el.arrayPagerWidget = pagerBuf;
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(
                "Widget used to render the pagination bar.\n"
                "Must expose: __prev (button), pageLabel (text), __next (button).\n"
                "Leave empty to use the default 'arrayPager'.");
        }

        ImGui::SeparatorText("Spacing");

        ImGui::Checkbox("distribute##eladist", &el.arrayDistribute);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip(
            "ON  -> spreads %d items evenly across the full\n"
            "       window width (horizontal) or height (vertical).\n"
            "       Step is computed at runtime: dimension / count.\n\n"
            "OFF -> fixed step: child natural size + offset.",
            el.arrayCount);

        if (el.arrayDistribute) {
            ImGui::SameLine();
            ImGui::TextDisabled("(window dimension / %d)", el.arrayCount);
        } else {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80);
            ImGui::DragFloat("offset %%##elaoff", &el.arrayOffset, spatialStep, -1.0f, 1.0f, "%.3f");
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(
                "Extra pixels added to the step between items.\n"
                "Positive = more spacing, negative = overlap.\n"
                "Only active when distribute is disabled.");
        }

        BorderEditorSection(el);

        ImGui::SeparatorText("Background");
        Color4Field("bg color##elabg", el.bgColor, /*alphaBar*/ true);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Filled rect drawn behind each array item.\nAlpha = 0 → no background.");

        ImGui::SeparatorText("Per-item data (from Lua)");
        ImGui::TextDisabled("Key prefix: \"<i>_<elementId>\"  (i starts at 0)");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip(
            "Each child item receives data whose keys start\n"
            "with its index followed by '_'. Example for count=3\n"
            "and a child with elements 'name' and 'avatar':\n\n"
            "  data[\"0_name\"]   = { text = \"Unit 1\" }\n"
            "  data[\"0_avatar\"] = { path = \"../assets/img/u1.png\" }\n"
            "  data[\"1_name\"]   = { text = \"Unit 2\" }\n"
            "  ...");
    }

}

// ---------------------------------------------------------------------------
void UIManagerGUI::DrawProgressBarColors(UIElement& el)
{
    ImGui::SeparatorText("Bar Colors");
    Color4Field("BG##cbg",       el.barBg);  ImGui::SameLine();
    Color4Field(">66%%##cok",    el.barOk);  ImGui::SameLine();
    Color4Field("33-66%%##cm",   el.barMid); ImGui::SameLine();
    Color4Field("<33%%##cl",     el.barLow);
}

// ---------------------------------------------------------------------------
void UIManagerGUI::DrawIconMappingEditor(UIElement& el)
{
    ImGui::SeparatorText("Icon Mapping");

    static char newKey[64]  = "";
    static char newVal[256] = "";

    std::vector<std::string> toRemove;
    for (auto& [k, v] : el.iconMapping) {
        char vbuf[256]; strncpy(vbuf, v.c_str(), sizeof(vbuf)-1); vbuf[sizeof(vbuf)-1]='\0';
        std::string vlabel = "##mapv_" + k;
        ImGui::Text("%s", k.c_str());
        ImGui::SameLine(80);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::DRAGGABLE_UI_ITEM), ImVec2(12, 12)); ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        if (ImGui::InputText(vlabel.c_str(), vbuf, sizeof(vbuf)))
            el.iconMapping[k] = vbuf;
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM))
                el.iconMapping[k] = std::string((const char*)p->Data);
            ImGui::EndDragDropTarget();
        }
        ImGui::SameLine();
        if (ImGui::SmallButton(("X##maprm_" + k).c_str()))
            toRemove.push_back(k);
        if (!v.empty()) {
            GLuint tid = Components::get()->Render()->getImageGLTexture(v);
            if (tid) { ImGui::SameLine(); ImGui::Image((ImTextureID)(intptr_t)tid, ImVec2(32, 32)); }
        }
    }
    for (auto& k : toRemove) el.iconMapping.erase(k);

    ImGui::SetNextItemWidth(80);
    ImGui::InputText("key##mapnk",  newKey, sizeof(newKey));
    ImGui::SameLine();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::DRAGGABLE_UI_ITEM), ImVec2(12, 12)); ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    ImGui::InputText("path##mapnv", newVal, sizeof(newVal));
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM))
            strncpy(newVal, (const char*)p->Data, sizeof(newVal)-1);
        ImGui::EndDragDropTarget();
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Add##mapadd") && newKey[0] != '\0') {
        el.iconMapping[newKey] = newVal;
        newKey[0] = '\0'; newVal[0] = '\0';
    }
}

// ---------------------------------------------------------------------------
void UIManagerGUI::SaveWidget(UIManager* ui, const std::string& widgetName, UIWidget& w)
{
    cJSON* root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "scale", w.scale);
    if (w.posX != 0.0f) cJSON_AddNumberToObject(root, "posX", w.posX);
    if (w.posY != 0.0f) cJSON_AddNumberToObject(root, "posY", w.posY);
    if (w.offsetX != 0.0f) cJSON_AddNumberToObject(root, "offsetX", w.offsetX);
    if (w.offsetY != 0.0f) cJSON_AddNumberToObject(root, "offsetY", w.offsetY);
    if (w.width  > 0) cJSON_AddNumberToObject(root, "width",  w.width);
    if (w.height > 0) cJSON_AddNumberToObject(root, "height", w.height);
    if (w.bgColor.a > 0.0f) {
        cJSON* bg = cJSON_CreateObject();
        cJSON_AddNumberToObject(bg, "r", w.bgColor.r);
        cJSON_AddNumberToObject(bg, "g", w.bgColor.g);
        cJSON_AddNumberToObject(bg, "b", w.bgColor.b);
        cJSON_AddNumberToObject(bg, "a", w.bgColor.a);
        cJSON_AddItemToObject(root, "bgColor", bg);
    }
    if (w.borderWidth > 0.0f) {
        cJSON* bc = cJSON_CreateObject();
        cJSON_AddNumberToObject(bc, "r", w.borderColor.r);
        cJSON_AddNumberToObject(bc, "g", w.borderColor.g);
        cJSON_AddNumberToObject(bc, "b", w.borderColor.b);
        cJSON_AddNumberToObject(bc, "a", w.borderColor.a);
        cJSON_AddItemToObject(root, "borderColor", bc);
        cJSON_AddNumberToObject(root, "borderWidth", w.borderWidth);
    }
    if (!w.cacheable) cJSON_AddFalseToObject(root, "cacheable");

    cJSON* arr = cJSON_CreateArray();

    for (auto& el : w.elements) {
        cJSON* item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "id",   el.id.c_str());
        cJSON_AddStringToObject(item, "type", el.type.c_str());
        if (!el.enabled) cJSON_AddFalseToObject(item, "enabled");
        if (!el.yAuto) {
            cJSON_AddNumberToObject(item, "x", el.x);
            cJSON_AddNumberToObject(item, "y", el.y);
        } else {
            cJSON_AddTrueToObject(item, "yAuto");
        }
        if (el.paddingLeft   != 0.0f) cJSON_AddNumberToObject(item, "paddingLeft",   el.paddingLeft);
        if (el.paddingTop    != 0.0f) cJSON_AddNumberToObject(item, "paddingTop",    el.paddingTop);
        if (el.paddingBottom != 0.0f) cJSON_AddNumberToObject(item, "paddingBottom", el.paddingBottom);
        if (el.w != 0.0f) cJSON_AddNumberToObject(item, "w", el.w);
        if (el.h != 0.0f) cJSON_AddNumberToObject(item, "h", el.h);

        if (el.type == "text") {
            cJSON_AddNumberToObject(item, "fontScale", el.fontScale);
            if (!el.staticText.empty()) cJSON_AddStringToObject(item, "text", el.staticText.c_str());
            if (el.textAlign != "left") cJSON_AddStringToObject(item, "textAlign", el.textAlign.c_str());
            const Color& tc = el.staticTextColor;
            if (tc.r < 0.999f || tc.g < 0.999f || tc.b < 0.999f || tc.a < 0.999f) {
                cJSON* tcJ = cJSON_CreateObject();
                cJSON_AddNumberToObject(tcJ, "r", tc.r);
                cJSON_AddNumberToObject(tcJ, "g", tc.g);
                cJSON_AddNumberToObject(tcJ, "b", tc.b);
                cJSON_AddNumberToObject(tcJ, "a", tc.a);
                cJSON_AddItemToObject(item, "textColor", tcJ);
            }
        }
        else if (el.type == "image") {
            if (el.imageScale != 1.0f) cJSON_AddNumberToObject(item, "imageScale", el.imageScale);
            if (!el.staticImagePath.empty()) cJSON_AddStringToObject(item, "imagePath", el.staticImagePath.c_str());
            if (!el.tooltip.empty()) cJSON_AddStringToObject(item, "tooltip", el.tooltip.c_str());
            if (el.borderWidth > 0.0f) {
                cJSON_AddNumberToObject(item, "borderWidth", el.borderWidth);
                cJSON* bc = cJSON_CreateObject();
                cJSON_AddNumberToObject(bc, "r", el.borderColor.r); cJSON_AddNumberToObject(bc, "g", el.borderColor.g);
                cJSON_AddNumberToObject(bc, "b", el.borderColor.b); cJSON_AddNumberToObject(bc, "a", el.borderColor.a);
                cJSON_AddItemToObject(item, "borderColor", bc);
            }
        }
        else if (el.type == "rect") {
            if (el.alpha < 0.999f) cJSON_AddNumberToObject(item, "alpha", el.alpha);
            if (el.alignH != "left")  cJSON_AddStringToObject(item, "alignH", el.alignH.c_str());
            if (el.alignV != "top")   cJSON_AddStringToObject(item, "alignV", el.alignV.c_str());
            {
                cJSON* col = cJSON_CreateObject();
                cJSON_AddNumberToObject(col, "r", el.rectColor.r);
                cJSON_AddNumberToObject(col, "g", el.rectColor.g);
                cJSON_AddNumberToObject(col, "b", el.rectColor.b);
                cJSON_AddNumberToObject(col, "a", el.rectColor.a);
                cJSON_AddItemToObject(item, "color", col);
            }
            if (el.borderWidth > 0.0f) {
                cJSON_AddNumberToObject(item, "borderWidth", el.borderWidth);
                cJSON* bc = cJSON_CreateObject();
                cJSON_AddNumberToObject(bc, "r", el.borderColor.r); cJSON_AddNumberToObject(bc, "g", el.borderColor.g);
                cJSON_AddNumberToObject(bc, "b", el.borderColor.b); cJSON_AddNumberToObject(bc, "a", el.borderColor.a);
                cJSON_AddItemToObject(item, "borderColor", bc);
            }
        }
        else if (el.type == "progressbar") {
            cJSON_AddNumberToObject(item, "barW",        el.barW);
            cJSON_AddNumberToObject(item, "barH",        el.barH);
            cJSON_AddNumberToObject(item, "textOffsetX", el.textOffsetX);
            if (el.textOffsetY != 0.0f) cJSON_AddNumberToObject(item, "textOffsetY", el.textOffsetY);
            cJSON_AddNumberToObject(item, "barFontScale", el.barFontScale);

            auto addColor = [&](const char* key, const Color& c) {
                cJSON* col = cJSON_CreateObject();
                cJSON_AddNumberToObject(col, "r", c.r);
                cJSON_AddNumberToObject(col, "g", c.g);
                cJSON_AddNumberToObject(col, "b", c.b);
                cJSON_AddNumberToObject(col, "a", c.a);
                cJSON_AddItemToObject(item, key, col);
            };
            addColor("barBg",  el.barBg);
            addColor("barOk",  el.barOk);
            addColor("barMid", el.barMid);
            addColor("barLow", el.barLow);
            const Color& tc = el.staticTextColor;
            if (tc.r < 0.999f || tc.g < 0.999f || tc.b < 0.999f || tc.a < 0.999f)
                addColor("textColor", tc);
        }
        else if (el.type == "button") {
            cJSON_AddNumberToObject(item, "fontScale", el.fontScale);
            if (!el.staticText.empty())
                cJSON_AddStringToObject(item, "text", el.staticText.c_str());
            if (!el.staticImagePath.empty())
                cJSON_AddStringToObject(item, "imagePath", el.staticImagePath.c_str());
            if (!el.staticImagePathOff.empty())
                cJSON_AddStringToObject(item, "imagePathOff", el.staticImagePathOff.c_str());
            if (el.iconSize > 0)
                cJSON_AddNumberToObject(item, "size", el.iconSize);
            if (el.btnImgOffsetX != 0.0f)
                cJSON_AddNumberToObject(item, "btnImgOffsetX", el.btnImgOffsetX);
            if (el.btnImgOffsetY != 0.0f)
                cJSON_AddNumberToObject(item, "btnImgOffsetY", el.btnImgOffsetY);
            if (el.btnTextOffsetX != 0.0f)
                cJSON_AddNumberToObject(item, "btnTextOffsetX", el.btnTextOffsetX);
            if (el.btnTextOffsetY != 0.0f)
                cJSON_AddNumberToObject(item, "btnTextOffsetY", el.btnTextOffsetY);
            auto addCol = [&](const char* key, const Color& c) {
                cJSON* col = cJSON_CreateObject();
                cJSON_AddNumberToObject(col, "r", c.r); cJSON_AddNumberToObject(col, "g", c.g);
                cJSON_AddNumberToObject(col, "b", c.b); cJSON_AddNumberToObject(col, "a", c.a);
                cJSON_AddItemToObject(item, key, col);
            };
            addCol("btnBg",      el.btnBg);
            addCol("btnHover",   el.btnHover);
            addCol("btnPressed", el.btnPressed);
            const Color& tc = el.staticTextColor;
            if (tc.r < 0.999f || tc.g < 0.999f || tc.b < 0.999f || tc.a < 0.999f)
                addCol("textColor", tc);
            if (!el.tooltip.empty()) cJSON_AddStringToObject(item, "tooltip", el.tooltip.c_str());
            if (!el.sound.empty())   cJSON_AddStringToObject(item, "sound",   el.sound.c_str());
            if (el.borderWidth > 0.0f) {
                cJSON_AddNumberToObject(item, "borderWidth", el.borderWidth);
                addCol("borderColor",        el.borderColor);
                addCol("borderColorHover",   el.borderColorHover);
                addCol("borderColorPressed", el.borderColorPressed);
            }
            if (!el.options.empty()) {
                cJSON* optArr = cJSON_CreateArray();
                for (auto& opt : el.options) {
                    cJSON* o = cJSON_CreateObject();
                    cJSON_AddStringToObject(o, "key", opt.key.c_str());
                    if (!opt.text.empty())          cJSON_AddStringToObject(o, "text",          opt.text.c_str());
                    if (!opt.image.empty())         cJSON_AddStringToObject(o, "image",         opt.image.c_str());
                    if (!opt.tooltip.empty())       cJSON_AddStringToObject(o, "tooltip",       opt.tooltip.c_str());
                    if (!opt.tooltipWidget.empty()) cJSON_AddStringToObject(o, "tooltipWidget", opt.tooltipWidget.c_str());
                    if (opt.colorSet) {
                        cJSON* oc = cJSON_CreateObject();
                        cJSON_AddNumberToObject(oc, "r", opt.color.r);
                        cJSON_AddNumberToObject(oc, "g", opt.color.g);
                        cJSON_AddNumberToObject(oc, "b", opt.color.b);
                        cJSON_AddNumberToObject(oc, "a", opt.color.a);
                        cJSON_AddItemToObject(o, "color", oc);
                    }
                    cJSON_AddItemToArray(optArr, o);
                }
                cJSON_AddItemToObject(item, "options", optArr);
            }
        }
        else if (el.type == "icons") {
            cJSON_AddNumberToObject(item, "size", el.iconSize);
            cJSON_AddNumberToObject(item, "gap",  el.iconGap);
            cJSON* mapping = cJSON_CreateObject();
            for (auto& [k, v] : el.iconMapping)
                cJSON_AddStringToObject(mapping, k.c_str(), v.c_str());
            cJSON_AddItemToObject(item, "mapping", mapping);
        }
        else if (el.type == "widget") {
            if (!el.widgetRef.empty())
                cJSON_AddStringToObject(item, "widgetRef", el.widgetRef.c_str());
        }
        else if (el.type == "array") {
            if (!el.widgetRef.empty())
                cJSON_AddStringToObject(item, "widgetRef", el.widgetRef.c_str());
            cJSON_AddNumberToObject(item, "arrayCount", el.arrayCount);
            cJSON_AddStringToObject(item, "arrayAlign", el.arrayAlign.c_str());
            if (el.arrayDistribute)
                cJSON_AddTrueToObject(item, "arrayDistribute");
            if (el.arrayPaginate)
                cJSON_AddTrueToObject(item, "arrayPaginate");
            if (!el.arrayPagerWidget.empty())
                cJSON_AddStringToObject(item, "arrayPagerWidget", el.arrayPagerWidget.c_str());
            if (el.arrayOffset != 0.0f)
                cJSON_AddNumberToObject(item, "arrayOffset", el.arrayOffset);
            if (el.borderWidth > 0.0f) {
                cJSON_AddNumberToObject(item, "borderWidth", el.borderWidth);
                cJSON* bc = cJSON_CreateObject();
                cJSON_AddNumberToObject(bc, "r", el.borderColor.r); cJSON_AddNumberToObject(bc, "g", el.borderColor.g);
                cJSON_AddNumberToObject(bc, "b", el.borderColor.b); cJSON_AddNumberToObject(bc, "a", el.borderColor.a);
                cJSON_AddItemToObject(item, "borderColor", bc);
            }
            if (el.bgColor.a > 0.0f) {
                cJSON* bg = cJSON_CreateObject();
                cJSON_AddNumberToObject(bg, "r", el.bgColor.r); cJSON_AddNumberToObject(bg, "g", el.bgColor.g);
                cJSON_AddNumberToObject(bg, "b", el.bgColor.b); cJSON_AddNumberToObject(bg, "a", el.bgColor.a);
                cJSON_AddItemToObject(item, "bgColor", bg);
            }
        }

        cJSON_AddItemToArray(arr, item);
    }

    cJSON_AddItemToObject(root, "elements", arr);

    if (!previewData.empty()) {
        cJSON* dataSection = cJSON_CreateObject();
        for (auto& [id, ed] : previewData) {
            if (!ed.provided) continue;
            // find element type
            std::string elType;
            for (auto& el : w.elements) if (el.id == id) { elType = el.type; break; }
            const bool isButton = (elType == "button");
            cJSON* entry = cJSON_CreateObject();
            if (!ed.text.empty())
                cJSON_AddStringToObject(entry, "text", ed.text.c_str());
            // path/pathOff belong in element definition for buttons — never serialize to "data"
            if (!isButton && !ed.path.empty())
                cJSON_AddStringToObject(entry, "path", ed.path.c_str());
            if (!isButton && !ed.pathOff.empty())
                cJSON_AddStringToObject(entry, "pathOff", ed.pathOff.c_str());
            if (ed.colorProvided) {
                cJSON* col = cJSON_CreateObject();
                cJSON_AddNumberToObject(col, "r", ed.color.r);
                cJSON_AddNumberToObject(col, "g", ed.color.g);
                cJSON_AddNumberToObject(col, "b", ed.color.b);
                cJSON_AddNumberToObject(col, "a", ed.color.a);
                cJSON_AddItemToObject(entry, "color", col);
            }
            if (ed.value != 75.0f)    cJSON_AddNumberToObject(entry, "value", ed.value);
            if (ed.maxValue != 100.0f) cJSON_AddNumberToObject(entry, "max",   ed.maxValue);
            if (!ed.iconList.empty()) {
                cJSON* list = cJSON_CreateArray();
                for (auto& icon : ed.iconList)
                    cJSON_AddItemToArray(list, cJSON_CreateString(icon.c_str()));
                cJSON_AddItemToObject(entry, "list", list);
            }
            cJSON_AddItemToObject(dataSection, id.c_str(), entry);
        }
        cJSON_AddItemToObject(root, "data", dataSection);
    }

    char* txt = cJSON_Print(root);
    const std::string& path = w.filePath.empty()
        ? ui->getWidgetsDir() + widgetName + ".json"
        : w.filePath;
    FILE* f = fopen(path.c_str(), "w");
    if (f) { fputs(txt, f); fclose(f); }
    free(txt);
    cJSON_Delete(root);

    ui->reloadWidgets();
}

// ---------------------------------------------------------------------------
void UIManagerGUI::DrawElementPreviewFields(UIElement& el)
{
    ImGui::SeparatorText("Preview data");
    auto& pd = previewData[el.id];

    if (el.type == "progressbar") {
        ImGui::DragFloat("value##pvpbv", &pd.value, 1.0f, 0.0f, 9999.0f);
    }
    else if (el.type == "icons") {
        for (auto& [k, _] : el.iconMapping) {
            bool active = std::find(pd.iconList.begin(), pd.iconList.end(), k) != pd.iconList.end();
            if (ImGui::Checkbox(k.c_str(), &active)) {
                if (active) pd.iconList.push_back(k);
                else pd.iconList.erase(std::remove(pd.iconList.begin(), pd.iconList.end(), k), pd.iconList.end());
            }
        }
    }
}

// ---------------------------------------------------------------------------
void UIManagerGUI::DrawWidgetPreview(UIManager* ui, const std::string& wName)
{
    if (!ui || !ui->getWidgets().count(wName)) return;

    auto* win = Components::get()->Window();
    const int PW = win->getWidthRender();
    const int PH = win->getHeightRender();

    // Lazy-init preview FBO at full render resolution
    if (!previewFBO) {
        glGenFramebuffers(1, &previewFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, previewFBO);

        glGenTextures(1, &previewColorTex);
        glBindTexture(GL_TEXTURE_2D, previewColorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, PW, PH, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, previewColorTex, 0);

        glGenRenderbuffers(1, &previewDepthRB);
        glBindRenderbuffer(GL_RENDERBUFFER, previewDepthRB);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, PW, PH);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, previewDepthRB);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Save GL state
    GLint prevFBO = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    win->setRenderFBOOverride(previewFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, previewFBO);
    glViewport(0, 0, PW, PH);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& el : ui->getWidgets().at(wName).elements)
        previewData[el.id].provided = (el.type != "rect");

    ui->drawWidget(wName, previewData, "foreground");

    win->setRenderFBOOverride(0);
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);
    glViewport(vp[0], vp[1], vp[2], vp[3]);

    // ── Interactive preview ──────────────────────────────────────────────────
    ImGui::SeparatorText("Preview");

    // Toolbar above the preview
    if (ImGui::SmallButton(" - ##pvzm")) previewZoom = std::max(1.0f, previewZoom - 0.5f);
    ImGui::SameLine();
    ImGui::TextDisabled("%.1fx", previewZoom);
    ImGui::SameLine();
    if (ImGui::SmallButton(" + ##pvzp")) previewZoom = std::min(16.0f, previewZoom + 0.5f);
    ImGui::SameLine();
    if (ImGui::SmallButton("Reset##pvr")) { previewZoom = 1.0f; previewPan = {0.0f, 0.0f}; }

    // Pan buttons
    float step = 0.05f / previewZoom;
    float inv    = 1.0f / previewZoom;
    float maxPan = std::max(0.0f, 1.0f - inv);
    ImGui::SameLine();
    if (ImGui::SmallButton(" ^ ##pvU")) previewPan.y = std::max(0.0f, previewPan.y - step);
    ImGui::SameLine();
    if (ImGui::SmallButton(" v ##pvD")) previewPan.y = std::min(maxPan, previewPan.y + step);
    ImGui::SameLine();
    if (ImGui::SmallButton(" < ##pvL")) previewPan.x = std::max(0.0f, previewPan.x - step);
    ImGui::SameLine();
    if (ImGui::SmallButton(" > ##pvR")) previewPan.x = std::min(maxPan, previewPan.x + step);

    previewPan.x = std::max(0.0f, std::min(maxPan, previewPan.x));
    previewPan.y = std::max(0.0f, std::min(maxPan, previewPan.y));

    // UV with Y-flip (OpenGL bottom-up vs ImGui top-down)
    ImVec2 uv0(previewPan.x,       1.0f - previewPan.y);
    ImVec2 uv1(previewPan.x + inv, 1.0f - previewPan.y - inv);

    float availW = ImGui::GetContentRegionAvail().x - 2.0f;
    float dispH  = availW * (float)PH / (float)PW;
    ImGui::Image((ImTextureID)(intptr_t)previewColorTex, ImVec2(availW, dispH), uv0, uv1,
                 ImVec4(1,1,1,1), ImVec4(0.4f, 0.4f, 0.4f, 0.8f));

}

