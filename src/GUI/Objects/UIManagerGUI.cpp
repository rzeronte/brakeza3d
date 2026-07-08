#include <filesystem>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include "../../../include/GUI/Objects/UIManagerGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUI.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Misc/cJSON.h"
#include "imgui.h"

int UIManagerGUI::selectedWidget  = -1;
int UIManagerGUI::selectedElement = -1;
GLuint UIManagerGUI::previewFBO      = 0;
GLuint UIManagerGUI::previewColorTex = 0;
GLuint UIManagerGUI::previewDepthRB  = 0;
UIWidgetRenderData UIManagerGUI::previewData;
float  UIManagerGUI::previewZoom         = 1.0f;
ImVec2 UIManagerGUI::previewPan          = {0.0f, 0.0f};

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

    // ── Col 1: widget list ────────────────────────────────────────────────
    float colH = ImGui::GetContentRegionAvail().y;
    ImGui::BeginChild("##col1", ImVec2(310.0f, colH), false);
    ImGui::SeparatorText("Loaded widgets");

    static char widgetFilter[128] = "";
    ImGui::SetNextItemWidth(-1.0f);
    ImGui::InputTextWithHint("##wfilter", "Filter...", widgetFilter, sizeof(widgetFilter));

    // Build filtered index list (case-insensitive substring)
    std::string filterLow = widgetFilter;
    for (char& c : filterLow) c = (char)tolower((unsigned char)c);

    std::vector<int> filtered; // indices into names[]
    filtered.reserve(names.size());
    for (int i = 0; i < (int)names.size(); i++) {
        if (filterLow.empty()) { filtered.push_back(i); continue; }
        // Build the same relative label used for display
        const std::string& fp = widgets[names[i]].filePath;
        std::string label = names[i];
        auto pos = fp.find("assets/ui/");  size_t skip = 10;
        if (pos == std::string::npos) { pos = fp.find("assets\\ui\\"); skip = 10; }
        if (pos == std::string::npos) { pos = fp.find("assets/");    skip = 7;  }
        if (pos == std::string::npos) { pos = fp.find("assets\\");   skip = 7;  }
        if (pos != std::string::npos) {
            label = fp.substr(pos + skip);
            if (label.size() > 5) label.resize(label.size() - 5);
            for (char& c : label) if (c == '\\') c = '/';
        }
        std::string labelLow = label;
        for (char& c : labelLow) c = (char)tolower((unsigned char)c);
        if (labelLow.find(filterLow) != std::string::npos) filtered.push_back(i);
    }

    float listH = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing() - 4.0f;
    ImGui::BeginChild("##wlist", ImVec2(-1.0f, listH), true);
    static int prevSelectedWidget = -1;
    for (int fi = 0; fi < (int)filtered.size(); fi++) {
        int i   = filtered[fi];
        bool sel = (i == selectedWidget);
        // Build label: relative path from "assets/ui/" prefix, e.g. rts/hud/clockWidget
        const std::string& fp = widgets[names[i]].filePath;
        std::string label = names[i];
        auto pos = fp.find("assets/ui/");
        size_t skip = 10;
        if (pos == std::string::npos) { pos = fp.find("assets\\ui\\"); skip = 10; }
        if (pos == std::string::npos) { pos = fp.find("assets/");  skip = 7; }
        if (pos == std::string::npos) { pos = fp.find("assets\\"); skip = 7; }
        if (pos != std::string::npos) {
            std::string rel = fp.substr(pos + skip);
            if (rel.size() > 5) rel.resize(rel.size() - 5);  // remove ".json"
            for (char& c : rel) if (c == '\\') c = '/';
            label = rel;
        }
        char entry[512];
        snprintf(entry, sizeof(entry), "%2d  %s##wsel%d", i + 1, label.c_str(), i);
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
        free(txt);
        cJSON_Delete(root);
        ui->reloadWidgets();
        selectedWidget  = -1;
        selectedElement = -1;
    });

    if (selectedWidget >= 0 && selectedWidget < (int)names.size()) {
        ImGui::SameLine();
        GUI::ImageButtonSmall(IconGUI::SCENE_UNLOAD, "Unload", [&]{
            ui->unloadWidget(names[selectedWidget]);
            selectedWidget  = -1;
            selectedElement = -1;
        });

        ImGui::SameLine();
        GUI::ImageButtonSmallConfirm(IconGUI::REMOVE, "Delete", "Borrar widget", "Eliminar el archivo JSON del disco?", [&]{
            std::string path = ui->getWidgetsDir() + names[selectedWidget] + ".json";
            std::filesystem::remove(path);
            ui->reloadWidgets();
            selectedWidget  = -1;
            selectedElement = -1;
        });
    }

    ImGui::SameLine();
    GUI::ImageButtonSmallConfirm(IconGUI::CLEAR_SCENE, "Clear all", "Limpiar widgets", "Descargar todos los widgets de memoria?", [&]{
        ui->clearWidgets();
        selectedWidget  = -1;
        selectedElement = -1;
    });

    ImGui::EndChild(); // ##col1

    if (names.empty() || selectedWidget < 0 || selectedWidget >= (int)names.size())
        return;

    ImGui::SameLine();

    const std::string& wName = names[selectedWidget];
    UIWidget& w = widgets[wName];

    float col2W = ImGui::GetContentRegionAvail().x - 10.0f;

    // ── Col 2: elements + editor ──────────────────────────────────────────
    ImGui::BeginChild("##col2", ImVec2(col2W, colH), false);

    ImGui::SeparatorText(wName.c_str());

    static bool openPreviewPopup = false;
    if (GUI::ImageButtonSmall(IconGUI::SAVE, "Save widget", []{})) {
        SaveWidget(ui, wName, w);
        ImGui::EndChild();
        return;
    }
    ImGui::SameLine();
    if (GUI::ImageButtonSmall(IconGUI::SOLO_WINDOW_ON, "Preview", []{}))
        openPreviewPopup = true;

    ImGui::SetNextItemWidth(70.0f);
    if (ImGui::DragFloat("Global scale##wscale", &w.scale, 0.01f, 0.1f, 5.0f, "%.2f"))
        w.scale = std::max(0.1f, std::min(5.0f, w.scale));
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90.0f);
    ImGui::DragFloat("Fixed X%%##wposx", &w.posX, 0.001f, 0.0f, 1.0f, "%.3f");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90.0f);
    ImGui::DragFloat("Fixed Y%%##wposy", &w.posY, 0.001f, 0.0f, 1.0f, "%.3f");

    ImGui::Spacing();
    ImGui::SetNextItemWidth(80.0f);
    ImGui::DragFloat("W (win%%)##wrefW", &w.width,  0.001f, 0.0f, 1.0f, "%.3f");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Widget width as fraction of window width (0..1)");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80.0f);
    ImGui::DragFloat("H (win%%)##wrefH", &w.height, 0.001f, 0.0f, 1.0f, "%.3f");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Widget height as fraction of window height (0..1)");

    // Widget-level background and border
    ImGui::Spacing();
    float wbg[4] = { w.bgColor.r, w.bgColor.g, w.bgColor.b, w.bgColor.a };
    if (ImGui::ColorEdit4("BG##wbg", wbg, ImGuiColorEditFlags_AlphaBar))
        w.bgColor = { wbg[0], wbg[1], wbg[2], wbg[3] };
    float wbc[4] = { w.borderColor.r, w.borderColor.g, w.borderColor.b, w.borderColor.a };
    if (ImGui::ColorEdit4("Border##wbc", wbc, ImGuiColorEditFlags_AlphaBar))
        w.borderColor = { wbc[0], wbc[1], wbc[2], wbc[3] };
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60.0f);
    ImGui::DragFloat("Width##wbw", &w.borderWidth, 0.5f, 0.0f, 20.0f, "%.0f");

    ImGui::Spacing();
    ImGui::SeparatorText("Elements");
    DrawElementList(w);
    ImGui::Spacing();

    if (selectedElement >= 0 && selectedElement < (int)w.elements.size()) {
        UIElement& el = w.elements[selectedElement];

        if (ImGui::CollapsingHeader("Element Properties", ImGuiTreeNodeFlags_DefaultOpen))
            DrawElementEditor(el, w, ui, wName);

        if (el.type != "text" && el.type != "rect" && el.type != "button" && el.type != "image" && el.type != "array" && ImGui::CollapsingHeader("Preview Data", ImGuiTreeNodeFlags_DefaultOpen))
            DrawElementPreviewFields(el);
    }

    ImGui::EndChild();

    // ── Preview popup ─────────────────────────────────────────────────────
    if (openPreviewPopup) {
        ImGui::OpenPopup("##widget_preview_popup");
        openPreviewPopup = false;
    }
    ImGui::SetNextWindowSize(ImVec2(440.0f, 540.0f), ImGuiCond_FirstUseEver);
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
        if (ImGui::Selectable(label.c_str(), sel))
            selectedElement = i;
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

    ImGui::SeparatorText("Element Properties");

    char idBuf[128]; strncpy(idBuf, el.id.c_str(), sizeof(idBuf)-1); idBuf[sizeof(idBuf)-1]='\0';
    ImGui::SetNextItemWidth(160);
    if (ImGui::InputText("id##elid", idBuf, sizeof(idBuf))) el.id = idBuf;

    int typeIdx = 0;
    for (int i = 0; i < 8; i++) if (el.type == types[i]) { typeIdx = i; break; }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(110);
    if (ImGui::Combo("type##eltype", &typeIdx, types, 8)) el.type = types[typeIdx];

    ImGui::SeparatorText("Layout");
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

    // Helper: InputFloat + [–] [+] buttons. step=1 for px fields, small for scale fields.
    auto stepField = [&](const char* label, float& v, float step) {
        constexpr float BTN = 18.0f;
        constexpr float GAP = 2.0f;
        float fieldW = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f
                       - BTN * 2.0f - GAP * 2.0f;
        ImGui::SetNextItemWidth(std::max(fieldW, 40.0f));
        ImGui::InputFloat(label, &v, 0, 0, step >= 1.0f ? "%.0f" : "%.3f");
        ImGui::SameLine(0, GAP);
        if (ImGui::SmallButton((std::string("-##m") + label).c_str())) v -= step;
        ImGui::SameLine(0, GAP);
        if (ImGui::SmallButton((std::string("+##p") + label).c_str())) v += step;
    };
    auto stepFieldRow = [&](const char* la, float& va, const char* lb, float& vb, float step) {
        stepField(la, va, step);
        ImGui::SameLine();
        stepField(lb, vb, step);
    };

    constexpr float xyStep    = 0.001f;
    constexpr float whStep    = 0.001f;
    constexpr const char* xyFmt     = "%.3f";
    constexpr const char* whFmt     = "%.3f";
    constexpr float spatialStep = 0.001f;
    constexpr const char* spatialFmt  = "%.3f";

    if (!el.yAuto) {
        ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("x %%##elx", &el.x, xyStep, 0.0f, 0.0f, xyFmt);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("y %%##ely", &el.y, xyStep, 0.0f, 0.0f, xyFmt);
    }
    if (el.type != "text" && el.type != "progressbar") {
        ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("w %%##elw", &el.w, whStep, 0.0f, 0.0f, whFmt);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("h %%##elh", &el.h, whStep, 0.0f, 0.0f, whFmt);
    }
    ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("padL %%##elpL", &el.paddingLeft,   0.001f, 0.0f, 0.0f, "%.3f");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("PaddingLeft — shifts element right (left side guaranteed).");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("padT %%##elpT", &el.paddingTop,    0.001f, 0.0f, 0.0f, "%.3f");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("PaddingTop — pre-element space (yAuto) / adds to y (fixed).");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("padB %%##elpB", &el.paddingBottom, 0.001f, 0.0f, 0.0f, "%.3f");
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("PaddingBottom — post-element space (yAuto) / adds to effective height (fixed).");

    // Helper: border editor block (image, button, array)
    auto borderEditor = [&]() {
        constexpr auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop;
        float v[4] = { el.borderColor.r, el.borderColor.g, el.borderColor.b, el.borderColor.a };
        if (ImGui::ColorEdit4("Border color##elbdrcol", v, flags))
            el.borderColor = Color(v[0], v[1], v[2], v[3]);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        ImGui::DragFloat("Border width %%##elbdrw", &el.borderWidth, 0.001f, 0.0f, 0.1f, "%.3f");
    };

    // Helper: small reset button to the left of static string fields
    auto resetStrBtn = [&](const char* uid, std::string& str) {
        ImGui::PushID(uid);
        if (ImGui::ImageButton(FileSystemGUI::Icon(IconGUI::REMOVE), ImVec2(12, 12))) str = "";
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Reset to default");
        ImGui::PopID();
        ImGui::SameLine();
    };

    // Type-specific fields
    if (el.type == "text") {
        ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("fontScale##elfs", &el.fontScale, 0.01f, 0.0f, 10.0f, "%.2f");
        constexpr auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop;
        float v[4] = {el.staticTextColor.r, el.staticTextColor.g, el.staticTextColor.b, el.staticTextColor.a};
        if (ImGui::ColorEdit4("Color##eltxtcolor", v, flags))
            el.staticTextColor = Color(v[0], v[1], v[2], v[3]);
        {
            char tbuf[256]; strncpy(tbuf, el.staticText.c_str(), sizeof(tbuf)-1); tbuf[sizeof(tbuf)-1]='\0';
            resetStrBtn("rsttxt", el.staticText);
            ImGui::SetNextItemWidth(260);
            if (ImGui::InputText("text##eltxtst", tbuf, sizeof(tbuf))) el.staticText = tbuf;
            ImGui::SameLine(); ImGui::TextDisabled("(static, overridden by Lua)");
        }
    }
    else if (el.type == "image") {
        ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("imageScale##elis", &el.imageScale, 0.05f, 0.0f, 10.0f, "%.2f");
        {
            char ibuf[256]; strncpy(ibuf, el.staticImagePath.c_str(), sizeof(ibuf)-1); ibuf[sizeof(ibuf)-1]='\0';
            resetStrBtn("rstimgpath", el.staticImagePath);
            ImGui::Image(FileSystemGUI::Icon(IconGUI::DRAGGABLE_UI_ITEM), ImVec2(12, 12)); ImGui::SameLine();
            ImGui::SetNextItemWidth(260);
            if (ImGui::InputText("imagePath##elimgpath", ibuf, sizeof(ibuf))) el.staticImagePath = ibuf;
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM))
                    el.staticImagePath = std::string((const char*)p->Data);
                ImGui::EndDragDropTarget();
            }
            if (!el.staticImagePath.empty()) {
                GLuint tid = Components::get()->Render()->getImageGLTexture(el.staticImagePath);
                if (tid) { ImGui::SameLine(); ImGui::Image((ImTextureID)(intptr_t)tid, ImVec2(32, 32)); }
            }
        }
        {
            char ttbuf[256]; strncpy(ttbuf, el.tooltip.c_str(), sizeof(ttbuf)-1); ttbuf[sizeof(ttbuf)-1]='\0';
            ImGui::SetNextItemWidth(360);
            if (ImGui::InputText("tooltip##elimgtt", ttbuf, sizeof(ttbuf))) el.tooltip = ttbuf;
            ImGui::SameLine(); ImGui::TextDisabled("(shown on hover)");
        }
        borderEditor();
    }
    else if (el.type == "rect") {
        constexpr auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop;
        float v[4] = {el.rectColor.r, el.rectColor.g, el.rectColor.b, el.rectColor.a};
        if (ImGui::ColorEdit4("Color##elrcolor", v, flags))
            el.rectColor = Color(v[0], v[1], v[2], v[3]);

        ImGui::SetNextItemWidth(80.0f);
        if (ImGui::DragFloat("alpha##elralpha", &el.alpha, 0.01f, 0.0f, 1.0f, "%.2f"))
            el.alpha = std::max(0.0f, std::min(1.0f, el.alpha));
        borderEditor();
    }
    else if (el.type == "progressbar") {
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("barW %%##elbw", &el.barW, spatialStep, 0.0f, 1.0f, spatialFmt);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("barH %%##elbh", &el.barH, spatialStep, 0.0f, 1.0f, spatialFmt);
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("txtOffX %%##eltox", &el.textOffsetX, spatialStep, -1.0f, 1.0f, spatialFmt);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(60.0f); ImGui::DragFloat("txtOffY %%##eltoy", &el.textOffsetY, spatialStep, -1.0f, 1.0f, spatialFmt);
        ImGui::SetNextItemWidth(70.0f); ImGui::DragFloat("barFontScale##elbfs", &el.barFontScale, 0.01f, 0.0f, 10.0f, "%.2f");
        {
            constexpr auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop;
            float v[4] = {el.staticTextColor.r, el.staticTextColor.g, el.staticTextColor.b, el.staticTextColor.a};
            if (ImGui::ColorEdit4("Text color##elpbcolor", v, flags))
                el.staticTextColor = Color(v[0], v[1], v[2], v[3]);
        }
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
            char ttbuf[256]; strncpy(ttbuf, el.tooltip.c_str(), sizeof(ttbuf)-1); ttbuf[sizeof(ttbuf)-1]='\0';
            ImGui::SetNextItemWidth(360);
            if (ImGui::InputText("tooltip##elbtntt", ttbuf, sizeof(ttbuf))) el.tooltip = ttbuf;
            ImGui::SameLine(); ImGui::TextDisabled("(shown on hover)");
        }
        {
            constexpr auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop;
            float v[4] = {el.staticTextColor.r, el.staticTextColor.g, el.staticTextColor.b, el.staticTextColor.a};
            if (ImGui::ColorEdit4("Text color##elbtncolor", v, flags))
                el.staticTextColor = Color(v[0], v[1], v[2], v[3]);
        }

        ImGui::SeparatorText("Static Content");
        {
            char tbuf[128]; strncpy(tbuf, el.staticText.c_str(), sizeof(tbuf)-1); tbuf[sizeof(tbuf)-1]='\0';
            resetStrBtn("rstbtntxt", el.staticText);
            ImGui::SetNextItemWidth(200);
            if (ImGui::InputText("text##elbtntxt", tbuf, sizeof(tbuf))) el.staticText = tbuf;
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
            ImGui::SetNextItemWidth(80.0f);
            char kbuf[64]; strncpy(kbuf, opt.key.c_str(), sizeof(kbuf)-1); kbuf[sizeof(kbuf)-1]='\0';
            if (ImGui::InputText("key##optk", kbuf, sizeof(kbuf))) opt.key = kbuf;
            ImGui::SameLine();
            ImGui::SetNextItemWidth(130.0f);
            char tbuf2[256]; strncpy(tbuf2, opt.text.c_str(), sizeof(tbuf2)-1); tbuf2[sizeof(tbuf2)-1]='\0';
            if (ImGui::InputText("text##optt", tbuf2, sizeof(tbuf2))) opt.text = tbuf2;
            ImGui::SameLine();
            {
                constexpr auto cflags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaBar;
                float cv[4] = {opt.color.r, opt.color.g, opt.color.b, opt.color.a};
                if (ImGui::ColorEdit4("##optcol", cv, cflags)) { opt.color = Color(cv[0],cv[1],cv[2],cv[3]); opt.colorSet = true; }
            }
            ImGui::SameLine();
            GUI::DrawButton("Delete option", IconGUI::REMOVE, ImVec2(12, 12), false, [&]{ optToDelete = oi; });

            // Image path row
            {
                GLuint tid = opt.image.empty() ? 0 : Components::get()->Render()->getImageGLTexture(opt.image);
                if (tid) { ImGui::Image((ImTextureID)(intptr_t)tid, ImVec2(24, 24)); ImGui::SameLine(); }
                else { ImGui::Image(FileSystemGUI::Icon(IconGUI::DRAGGABLE_UI_ITEM), ImVec2(12, 12)); ImGui::SameLine(); }
                ImGui::TextDisabled("image"); ImGui::SameLine();
                char ibuf[512]; strncpy(ibuf, opt.image.c_str(), sizeof(ibuf)-1); ibuf[sizeof(ibuf)-1]='\0';
                ImGui::SetNextItemWidth(220.0f);
                if (ImGui::InputText("##optimg", ibuf, sizeof(ibuf))) opt.image = ibuf;
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM))
                        opt.image = std::string((const char*)p->Data);
                    ImGui::EndDragDropTarget();
                }
            }

            // Tooltip rows
            {
                ImGui::TextDisabled("tooltip"); ImGui::SameLine();
                char ttbuf[256]; strncpy(ttbuf, opt.tooltip.c_str(), sizeof(ttbuf)-1); ttbuf[sizeof(ttbuf)-1]='\0';
                ImGui::SetNextItemWidth(220.0f);
                if (ImGui::InputText("##opttt", ttbuf, sizeof(ttbuf))) opt.tooltip = ttbuf;
            }
            {
                ImGui::TextDisabled("ttWidget"); ImGui::SameLine();
                char twbuf[128]; strncpy(twbuf, opt.tooltipWidget.c_str(), sizeof(twbuf)-1); twbuf[sizeof(twbuf)-1]='\0';
                ImGui::SetNextItemWidth(220.0f);
                if (ImGui::InputText("##opttw", twbuf, sizeof(twbuf))) opt.tooltipWidget = twbuf;
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
        constexpr auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop;
        auto ce = [&](const char* label, Color& c) {
            float v[4] = {c.r, c.g, c.b, c.a};
            if (ImGui::ColorEdit4(label, v, flags)) c = Color(v[0], v[1], v[2], v[3]);
        };
        ce("Normal##cbtn",  el.btnBg);      ImGui::SameLine();
        ce("Hover##cbtnhv", el.btnHover);   ImGui::SameLine();
        ce("Press##cbtnpr", el.btnPressed);
        borderEditor();
    }
    else if (el.type == "widget") {
        char refBuf[128]; strncpy(refBuf, el.widgetRef.c_str(), sizeof(refBuf)-1); refBuf[sizeof(refBuf)-1]='\0';
        ImGui::SetNextItemWidth(200);
        if (ImGui::InputText("widgetRef##elwref", refBuf, sizeof(refBuf))) el.widgetRef = refBuf;
        ImGui::SameLine(); ImGui::TextDisabled("(widget name)");
    }
    else if (el.type == "array") {
        ImGui::SeparatorText("Array");

        char refBuf[128]; strncpy(refBuf, el.widgetRef.c_str(), sizeof(refBuf)-1); refBuf[sizeof(refBuf)-1]='\0';
        ImGui::SetNextItemWidth(200);
        if (ImGui::InputText("widgetRef##elaref", refBuf, sizeof(refBuf))) el.widgetRef = refBuf;
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

        borderEditor();

        ImGui::SeparatorText("Background");
        float abg[4] = { el.bgColor.r, el.bgColor.g, el.bgColor.b, el.bgColor.a };
        if (ImGui::ColorEdit4("bg color##elabg", abg, ImGuiColorEditFlags_AlphaBar))
            el.bgColor = Color(abg[0], abg[1], abg[2], abg[3]);
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
    constexpr auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop;

    auto colorEdit = [&](const char* label, Color& c) {
        float v[4] = { c.r, c.g, c.b, c.a };
        if (ImGui::ColorEdit4(label, v, flags)) c = Color(v[0], v[1], v[2], v[3]);
    };

    colorEdit("BG##cbg",       el.barBg);  ImGui::SameLine();
    colorEdit(">66%%##cok",    el.barOk);  ImGui::SameLine();
    colorEdit("33-66%%##cm",   el.barMid); ImGui::SameLine();
    colorEdit("<33%%##cl",     el.barLow);
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

    cJSON* arr = cJSON_CreateArray();

    for (auto& el : w.elements) {
        cJSON* item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "id",   el.id.c_str());
        cJSON_AddStringToObject(item, "type", el.type.c_str());
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
            if (el.borderWidth > 0.0f) {
                cJSON_AddNumberToObject(item, "borderWidth", el.borderWidth);
                addCol("borderColor", el.borderColor);
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

