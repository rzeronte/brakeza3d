#include <filesystem>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include "../../../include/GUI/Objects/UIManagerGUI.h"
#include "../../../include/GUI/GUIManager.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Misc/cJSON.h"
#include "imgui.h"

int UIManagerGUI::selectedWidget  = -1;
int UIManagerGUI::selectedElement = -1;
GLuint UIManagerGUI::previewFBO      = 0;
GLuint UIManagerGUI::previewColorTex = 0;
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

    // ── Left panel: widget list ───────────────────────────────────────────
    ImGui::BeginGroup();
    ImGui::SeparatorText("Widgets");

    float listH = ImGui::GetContentRegionAvail().y - 30.0f;
    ImGui::BeginChild("##wlist", ImVec2(160, listH), true);
    for (int i = 0; i < (int)names.size(); i++) {
        bool sel = (i == selectedWidget);
        if (ImGui::Selectable(names[i].c_str(), sel)) {
            selectedWidget  = i;
            selectedElement = -1;
        }
    }
    ImGui::EndChild();

    if (ImGui::Button("+ New##w")) {
        // Create a blank widget file and reload
        std::string newName = "newWidget";
        int n = 0;
        while (widgets.count(newName + (n ? std::to_string(n) : ""))) n++;
        if (n) newName += std::to_string(n);

        std::string path = ui->getWidgetsDir() + newName + ".json";
        cJSON* root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, "gap", 4);
        cJSON_AddItemToObject(root, "elements", cJSON_CreateArray());
        char* txt = cJSON_Print(root);
        FILE* f = fopen(path.c_str(), "w");
        if (f) { fputs(txt, f); fclose(f); }
        free(txt);
        cJSON_Delete(root);
        ui->reloadWidgets();
        selectedWidget  = -1;
        selectedElement = -1;
    }

    if (selectedWidget >= 0 && selectedWidget < (int)names.size()) {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1));
        if (ImGui::Button("Delete##w")) {
            std::string path = ui->getWidgetsDir() + names[selectedWidget] + ".json";
            std::filesystem::remove(path);
            ui->reloadWidgets();
            selectedWidget  = -1;
            selectedElement = -1;
        }
        ImGui::PopStyleColor();
    }
    ImGui::EndGroup();

    if (names.empty() || selectedWidget < 0 || selectedWidget >= (int)names.size())
        return;

    ImGui::SameLine();

    const std::string& wName = names[selectedWidget];
    UIWidget& w = widgets[wName];

    float col2W = 480.0f;
    float colH  = ImGui::GetContentRegionAvail().y;

    // ── Col 2: elements + editor ──────────────────────────────────────────
    ImGui::BeginChild("##col2", ImVec2(col2W, colH), false);

    ImGui::SeparatorText(wName.c_str());

    if (ImGui::InputFloat("Gap##wgap", &w.gap, 1.0f, 4.0f, "%.0f"))
        w.gap = std::max(0.0f, w.gap);
    ImGui::SetNextItemWidth(120.0f);
    if (ImGui::DragFloat("Scale##wscale", &w.scale, 0.01f, 0.1f, 5.0f, "%.2f"))
        w.scale = std::max(0.1f, std::min(5.0f, w.scale));

    ImGui::Spacing();
    ImGui::SeparatorText("Elements");
    DrawElementList(w);
    ImGui::Spacing();

    if (ImGui::Button("+ Add Element##el")) {
        UIElement el;
        el.id   = "element" + std::to_string(w.elements.size());
        el.type = "text";
        w.elements.push_back(el);
        selectedElement = (int)w.elements.size() - 1;
    }

    ImGui::Spacing();
    if (ImGui::Button("Save##wsave"))
        SaveWidget(ui, wName, w);
    ImGui::SameLine();
    ImGui::TextDisabled("(overwrites JSON + reloads)");

    ImGui::Spacing();

    if (selectedElement >= 0 && selectedElement < (int)w.elements.size()) {
        UIElement& el = w.elements[selectedElement];

        if (ImGui::CollapsingHeader("Element Properties", ImGuiTreeNodeFlags_DefaultOpen))
            DrawElementEditor(el, ui, wName);

        if (ImGui::CollapsingHeader("Preview Data", ImGuiTreeNodeFlags_DefaultOpen))
            DrawElementPreviewFields(el);
    }

    ImGui::EndChild();

    ImGui::SameLine();

    // ── Col 3: preview — ocupa todo el espacio restante ───────────────────
    float col3W = ImGui::GetContentRegionAvail().x;
    ImGui::BeginChild("##col3", ImVec2(col3W, colH), false);
    DrawWidgetPreview(ui, wName);
    ImGui::EndChild();
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

    // Up / Down / Delete buttons outside the scrollable list so they are always reachable
    if (selectedElement >= 0 && selectedElement < (int)w.elements.size()) {
        int i = selectedElement;
        if (i > 0 && ImGui::SmallButton("^##up")) { std::swap(w.elements[i], w.elements[i-1]); selectedElement = i-1; }
        ImGui::SameLine();
        if (i < (int)w.elements.size()-1 && ImGui::SmallButton("v##dn")) { std::swap(w.elements[i], w.elements[i+1]); selectedElement = i+1; }
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f,0.1f,0.1f,1));
        if (ImGui::SmallButton("X##elrm")) {
            w.elements.erase(w.elements.begin() + i);
            selectedElement = -1;
        }
        ImGui::PopStyleColor();
    }
}

// ---------------------------------------------------------------------------
void UIManagerGUI::DrawElementEditor(UIElement& el, UIManager* ui, const std::string& widgetName)
{
    static const char* types[] = { "text", "image", "rect", "progressbar", "icons", "button" };

    ImGui::SeparatorText("Element Properties");

    char idBuf[128]; strncpy(idBuf, el.id.c_str(), sizeof(idBuf)-1); idBuf[sizeof(idBuf)-1]='\0';
    ImGui::SetNextItemWidth(160);
    if (ImGui::InputText("id##elid", idBuf, sizeof(idBuf))) el.id = idBuf;

    int typeIdx = 0;
    for (int i = 0; i < 6; i++) if (el.type == types[i]) { typeIdx = i; break; }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(110);
    if (ImGui::Combo("type##eltype", &typeIdx, types, 6)) el.type = types[typeIdx];

    ImGui::Checkbox("yAuto##elyauto", &el.yAuto);
    ImGui::SameLine(); ImGui::TextDisabled("(stack below previous)");

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

    if (!el.yAuto) stepFieldRow("x##elx", el.x, "y##ely", el.y, 1.0f);
    if (el.type != "text") stepFieldRow("w##elw", el.w, "h##elh", el.h, 1.0f);

    // Type-specific fields
    if (el.type == "text") {
        stepField("fontScale##elfs", el.fontScale, 0.01f);
    }
    else if (el.type == "image") {
        stepField("imageScale##elis", el.imageScale, 0.05f);
    }
    else if (el.type == "rect") {
        constexpr auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop;
        float v[4] = {el.rectColor.r, el.rectColor.g, el.rectColor.b, el.rectColor.a};
        if (ImGui::ColorEdit4("Color##elrcolor", v, flags))
            el.rectColor = Color(v[0], v[1], v[2], v[3]);

        float hw = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f;
        ImGui::SetNextItemWidth(hw);
        if (ImGui::DragFloat("alpha##elralpha", &el.alpha, 0.01f, 0.0f, 1.0f, "%.2f"))
            el.alpha = std::max(0.0f, std::min(1.0f, el.alpha));

        ImGui::SetNextItemWidth(hw);
        if (ImGui::DragFloat("wPct##elwpct", &el.wPct, 0.01f, 0.0f, 1.0f, "W%% %.2f"))
            el.wPct = std::max(0.0f, std::min(1.0f, el.wPct));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(hw);
        if (ImGui::DragFloat("hPct##elhpct", &el.hPct, 0.01f, 0.0f, 1.0f, "H%% %.2f"))
            el.hPct = std::max(0.0f, std::min(1.0f, el.hPct));
        if (el.wPct > 0 || el.hPct > 0)
            ImGui::TextDisabled("(w/h ignorados cuando pct > 0)");

        static const char* hAligns[] = { "left", "center", "right" };
        static const char* vAligns[] = { "top",  "center", "bottom" };
        int hIdx = 0, vIdx = 0;
        for (int i = 0; i < 3; i++) { if (el.alignH == hAligns[i]) hIdx = i; }
        for (int i = 0; i < 3; i++) { if (el.alignV == vAligns[i]) vIdx = i; }
        ImGui::SetNextItemWidth(hw);
        if (ImGui::Combo("alignH##elah", &hIdx, hAligns, 3)) el.alignH = hAligns[hIdx];
        ImGui::SameLine();
        ImGui::SetNextItemWidth(hw);
        if (ImGui::Combo("alignV##elav", &vIdx, vAligns, 3)) el.alignV = vAligns[vIdx];
    }
    else if (el.type == "progressbar") {
        stepFieldRow("barW##elbw", el.barW, "barH##elbh", el.barH, 1.0f);
        stepFieldRow("barOffsetY##elbopy", el.barOffsetY, "textOffsetX##eltox", el.textOffsetX, 1.0f);
        stepField("barFontScale##elbfs", el.barFontScale, 0.01f);
        DrawProgressBarColors(el);
    }
    else if (el.type == "icons") {
        stepFieldRow("iconSize##elis", el.iconSize, "iconGap##eig", el.iconGap, 1.0f);
        DrawIconMappingEditor(el);
    }
    else if (el.type == "button") {
        stepField("fontScale##elbtnfs", el.fontScale, 0.01f);
        ImGui::SeparatorText("Button Colors");
        constexpr auto flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop;
        auto ce = [&](const char* label, Color& c) {
            float v[4] = {c.r, c.g, c.b, c.a};
            if (ImGui::ColorEdit4(label, v, flags)) c = Color(v[0], v[1], v[2], v[3]);
        };
        ce("Normal##cbtn",  el.btnBg);      ImGui::SameLine();
        ce("Hover##cbtnhv", el.btnHover);   ImGui::SameLine();
        ce("Press##cbtnpr", el.btnPressed);
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
        ImGui::SetNextItemWidth(200);
        if (ImGui::InputText(vlabel.c_str(), vbuf, sizeof(vbuf)))
            el.iconMapping[k] = vbuf;
        ImGui::SameLine();
        if (ImGui::SmallButton(("X##maprm_" + k).c_str()))
            toRemove.push_back(k);
    }
    for (auto& k : toRemove) el.iconMapping.erase(k);

    ImGui::SetNextItemWidth(80);
    ImGui::InputText("key##mapnk",  newKey, sizeof(newKey));
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    ImGui::InputText("path##mapnv", newVal, sizeof(newVal));
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
    cJSON_AddNumberToObject(root, "gap", w.gap);
    cJSON_AddNumberToObject(root, "scale", w.scale);

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
        if (el.type != "text" && el.w > 0) cJSON_AddNumberToObject(item, "w", el.w);
        if (el.type != "text" && el.h > 0) cJSON_AddNumberToObject(item, "h", el.h);

        if (el.type == "text") {
            cJSON_AddNumberToObject(item, "fontScale", el.fontScale);
        }
        else if (el.type == "image") {
            if (el.imageScale != 1.0f) cJSON_AddNumberToObject(item, "imageScale", el.imageScale);
        }
        else if (el.type == "rect") {
            if (el.alpha < 0.999f) cJSON_AddNumberToObject(item, "alpha", el.alpha);
            if (el.wPct > 0)       cJSON_AddNumberToObject(item, "wPct", el.wPct);
            if (el.hPct > 0)       cJSON_AddNumberToObject(item, "hPct", el.hPct);
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
        }
        else if (el.type == "progressbar") {
            cJSON_AddNumberToObject(item, "barW",        el.barW);
            cJSON_AddNumberToObject(item, "barH",        el.barH);
            cJSON_AddNumberToObject(item, "barOffsetY",  el.barOffsetY);
            cJSON_AddNumberToObject(item, "textOffsetX", el.textOffsetX);
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
        }
        else if (el.type == "button") {
            cJSON_AddNumberToObject(item, "fontScale", el.fontScale);
            auto addCol = [&](const char* key, const Color& c) {
                cJSON* col = cJSON_CreateObject();
                cJSON_AddNumberToObject(col, "r", c.r); cJSON_AddNumberToObject(col, "g", c.g);
                cJSON_AddNumberToObject(col, "b", c.b); cJSON_AddNumberToObject(col, "a", c.a);
                cJSON_AddItemToObject(item, key, col);
            };
            addCol("btnBg",      el.btnBg);
            addCol("btnHover",   el.btnHover);
            addCol("btnPressed", el.btnPressed);
        }
        else if (el.type == "icons") {
            cJSON_AddNumberToObject(item, "size", el.iconSize);
            cJSON_AddNumberToObject(item, "gap",  el.iconGap);
            cJSON* mapping = cJSON_CreateObject();
            for (auto& [k, v] : el.iconMapping)
                cJSON_AddStringToObject(mapping, k.c_str(), v.c_str());
            cJSON_AddItemToObject(item, "mapping", mapping);
        }

        cJSON_AddItemToArray(arr, item);
    }

    cJSON_AddItemToObject(root, "elements", arr);

    char* txt = cJSON_Print(root);
    std::string path = ui->getWidgetsDir() + widgetName + ".json";
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

    if (el.type == "text") {
        char buf[256];
        strncpy(buf, pd.text.c_str(), sizeof(buf)-1); buf[sizeof(buf)-1] = '\0';
        if (ImGui::InputText("text##pvtext", buf, sizeof(buf)))
            pd.text = buf;
        float col[4] = { pd.color.r, pd.color.g, pd.color.b, pd.color.a };
        if (ImGui::ColorEdit4("color##pvtcol", col))
            pd.color = Color(col[0], col[1], col[2], col[3]);
    }
    else if (el.type == "rect") {
        ImGui::TextDisabled("(color comes from element definition)");
    }
    else if (el.type == "image") {
        char buf[256];
        strncpy(buf, pd.path.c_str(), sizeof(buf)-1); buf[sizeof(buf)-1] = '\0';
        ImGui::SetNextItemWidth(200);
        if (ImGui::InputText("path##pvimg", buf, sizeof(buf)))
            pd.path = buf;
        ImGui::SameLine();
        ImGui::TextDisabled("(drag image here)");
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
                pd.path = std::string((const char*)payload->Data);
            }
            ImGui::EndDragDropTarget();
        }
    }
    else if (el.type == "progressbar") {
        ImGui::DragFloat("value##pvpbv", &pd.value,    1.0f, 0.0f, pd.maxValue);
        ImGui::DragFloat("max##pvpbm",   &pd.maxValue, 1.0f, 1.0f, 9999.0f);
    }
    else if (el.type == "icons") {
        ImGui::SeparatorText("Icon list for preview");
        for (auto& [k, _] : el.iconMapping) {
            bool active = std::find(pd.iconList.begin(), pd.iconList.end(), k) != pd.iconList.end();
            if (ImGui::Checkbox(k.c_str(), &active)) {
                if (active) pd.iconList.push_back(k);
                else pd.iconList.erase(std::remove(pd.iconList.begin(), pd.iconList.end(), k), pd.iconList.end());
            }
        }
    }
    else if (el.type == "button") {
        char buf[128];
        strncpy(buf, pd.text.c_str(), sizeof(buf)-1); buf[sizeof(buf)-1] = '\0';
        if (ImGui::InputText("label##pvbtn", buf, sizeof(buf))) pd.text = buf;
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

        GLuint depthRB;
        glGenRenderbuffers(1, &depthRB);
        glBindRenderbuffer(GL_RENDERBUFFER, depthRB);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, PW, PH);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRB);

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
    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& el : ui->getWidgets().at(wName).elements)
        previewData[el.id].provided = (el.type != "rect");

    ui->drawWidget(wName, 4, 4, previewData, "foreground");

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
    ImGui::SameLine();
    ImGui::TextDisabled("(wheel=zoom  drag=pan)");

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

    float availW = ImGui::GetContentRegionAvail().x;
    float dispH  = availW * (float)PH / (float)PW;
    ImGui::Image((ImTextureID)(intptr_t)previewColorTex, ImVec2(availW, dispH), uv0, uv1);

}

