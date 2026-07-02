#include <filesystem>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <string>

#include "../../../include/Render/UI/UIManager.h"
#include "../../../include/Components/ComponentRender.h"
#include "../../../include/Components/ComponentInput.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Misc/cJSON.h"
#include "../../../include/Misc/ToolsJSON.h"
#include "../../../include/Render/TextWriter.h"

void UIManager::init(ComponentRender* r, const std::string& dir)
{
    render = r;
    input  = Components::get()->Input();
    tw     = r->getTextWriter();
    widgetsDir = dir;
    loadWidgets();
}

void UIManager::loadWidgets()
{
    widgets.clear();

    if (!std::filesystem::exists(widgetsDir)) {
        printf("[UIManager] WARN: widgets directory not found: %s\n", widgetsDir.c_str());
        return;
    }

    for (auto& entry : std::filesystem::directory_iterator(widgetsDir)) {
        if (entry.path().extension() == ".json") {
            loadWidget(entry.path().string());
        }
    }

    printf("[UIManager] loaded %zu widget(s) from %s\n", widgets.size(), widgetsDir.c_str());
}

void UIManager::reloadWidgets()
{
    loadWidgets();
    printf("[UIManager] reloaded\n");
}

void UIManager::clearWidgets()
{
    widgets.clear();
}

void UIManager::loadWidget(const std::string& filePath)
{
    FILE* f = fopen(filePath.c_str(), "rb");
    if (!f) {
        printf("[UIManager] WARN: cannot open %s\n", filePath.c_str());
        return;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    auto* buf = (char*)malloc(len + 1);
    if (!buf) { fclose(f); return; }
    fread(buf, 1, len, f);
    fclose(f);
    buf[len] = '\0';

    cJSON* root = cJSON_Parse(buf);
    free(buf);
    if (!root) {
        printf("[UIManager] WARN: JSON parse error in %s\n", filePath.c_str());
        return;
    }

    std::string widgetName = std::filesystem::path(filePath).stem().string();

    UIWidget widget;
    cJSON* gapItem = cJSON_GetObjectItem(root, "gap");
    if (gapItem) widget.gap = (float)gapItem->valuedouble;

    cJSON* scaleItem = cJSON_GetObjectItem(root, "scale");
    if (scaleItem) widget.scale = (float)scaleItem->valuedouble;

    cJSON* elementsArr = cJSON_GetObjectItem(root, "elements");
    if (!elementsArr || !cJSON_IsArray(elementsArr)) {
        printf("[UIManager] WARN: no elements array in %s\n", filePath.c_str());
        cJSON_Delete(root);
        return;
    }

    int arrSize = cJSON_GetArraySize(elementsArr);
    for (int i = 0; i < arrSize; i++) {
        cJSON* item = cJSON_GetArrayItem(elementsArr, i);
        if (!item) continue;

        UIElement el;

        cJSON* id = cJSON_GetObjectItem(item, "id");
        if (id) el.id = id->valuestring;

        cJSON* type = cJSON_GetObjectItem(item, "type");
        if (type) {
            el.type = type->valuestring;
            // backward compat: old JSON files may still use "hpbar"
            if (el.type == "hpbar") el.type = "progressbar";
        }

        cJSON* x = cJSON_GetObjectItem(item, "x");
        if (x) el.x = (float)x->valuedouble;

        cJSON* y = cJSON_GetObjectItem(item, "y");
        if (y) el.y = (float)y->valuedouble;

        cJSON* yAuto = cJSON_GetObjectItem(item, "yAuto");
        if (yAuto) el.yAuto = (yAuto->type == cJSON_True);

        cJSON* w = cJSON_GetObjectItem(item, "w");
        if (w) el.w = (float)w->valuedouble;

        cJSON* h = cJSON_GetObjectItem(item, "h");
        if (h) el.h = (float)h->valuedouble;

        cJSON* alpha = cJSON_GetObjectItem(item, "alpha");
        if (alpha) el.alpha = (float)alpha->valuedouble;

        cJSON* fontScale = cJSON_GetObjectItem(item, "fontScale");
        if (fontScale) el.fontScale = (float)fontScale->valuedouble;

        cJSON* imageScale = cJSON_GetObjectItem(item, "imageScale");
        if (imageScale) el.imageScale = (float)imageScale->valuedouble;

        // progressbar — new keys, with fallback to old "hpw"/"hph" keys
        cJSON* barW = cJSON_GetObjectItem(item, "barW");
        if (!barW) barW = cJSON_GetObjectItem(item, "hpw");
        el.barW = barW ? (float)barW->valuedouble : el.w;

        cJSON* barH = cJSON_GetObjectItem(item, "barH");
        if (!barH) barH = cJSON_GetObjectItem(item, "hph");
        el.barH = barH ? (float)barH->valuedouble : el.h;

        cJSON* textOffX = cJSON_GetObjectItem(item, "textOffsetX");
        if (textOffX) el.textOffsetX = (float)textOffX->valuedouble;

        cJSON* barOffY = cJSON_GetObjectItem(item, "barOffsetY");
        if (!barOffY) barOffY = cJSON_GetObjectItem(item, "hpOffsetY");
        if (barOffY) el.barOffsetY = (float)barOffY->valuedouble;

        cJSON* barFont = cJSON_GetObjectItem(item, "barFontScale");
        if (!barFont) barFont = cJSON_GetObjectItem(item, "hpFontScale");
        if (barFont) el.barFontScale = (float)barFont->valuedouble;

        cJSON* wPct = cJSON_GetObjectItem(item, "wPct");
        if (wPct) el.wPct = (float)wPct->valuedouble;
        cJSON* hPct = cJSON_GetObjectItem(item, "hPct");
        if (hPct) el.hPct = (float)hPct->valuedouble;

        cJSON* alignH = cJSON_GetObjectItem(item, "alignH");
        if (alignH && alignH->valuestring) el.alignH = alignH->valuestring;
        cJSON* alignV = cJSON_GetObjectItem(item, "alignV");
        if (alignV && alignV->valuestring) el.alignV = alignV->valuestring;

        cJSON* bg = cJSON_GetObjectItem(item, "barBg");
        if (bg) el.barBg = ToolsJSON::getColorByJSON(bg);

        cJSON* ok = cJSON_GetObjectItem(item, "barOk");
        if (ok) el.barOk = ToolsJSON::getColorByJSON(ok);

        cJSON* mid = cJSON_GetObjectItem(item, "barMid");
        if (mid) el.barMid = ToolsJSON::getColorByJSON(mid);

        cJSON* low = cJSON_GetObjectItem(item, "barLow");
        if (low) el.barLow = ToolsJSON::getColorByJSON(low);

        cJSON* btnBg = cJSON_GetObjectItem(item, "btnBg");
        if (btnBg) el.btnBg = ToolsJSON::getColorByJSON(btnBg);
        cJSON* btnHover = cJSON_GetObjectItem(item, "btnHover");
        if (btnHover) el.btnHover = ToolsJSON::getColorByJSON(btnHover);
        cJSON* btnPressed = cJSON_GetObjectItem(item, "btnPressed");
        if (btnPressed) el.btnPressed = ToolsJSON::getColorByJSON(btnPressed);

        cJSON* iconSize = cJSON_GetObjectItem(item, "size");
        if (iconSize) el.iconSize = (float)iconSize->valuedouble;

        cJSON* iconGap = cJSON_GetObjectItem(item, "gap");
        if (iconGap) el.iconGap = (float)iconGap->valuedouble;

        cJSON* mapping = cJSON_GetObjectItem(item, "mapping");
        if (mapping && cJSON_IsObject(mapping)) {
            cJSON* mapChild = nullptr;
            cJSON_ArrayForEach(mapChild, mapping) {
                if (mapChild->valuestring) {
                    el.iconMapping[mapChild->string] = mapChild->valuestring;
                }
            }
        }

        widget.elements.push_back(el);
    }

    cJSON_Delete(root);

    computeWidgetHeight(widget);
    widgets[widgetName] = widget;
    printf("[UIManager] loaded widget '%s' (%zu elements, height=%.0f)\n",
        widgetName.c_str(), widget.elements.size(), widget.height);
}

void UIManager::computeWidgetHeight(UIWidget& w)
{
    float maxY = 0;
    for (auto& el : w.elements) {
        float bottom = el.y + getElementHeight(el);
        if (bottom > maxY) maxY = bottom;
    }
    w.height = maxY + w.gap;
}

float UIManager::getElementHeight(const UIElement& el)
{
    if (el.type == "text")        return el.fontScale * 20.0f;
    if (el.type == "image")       return el.h * el.imageScale;
    if (el.type == "rect")        return (el.h > 0) ? el.h : 0;
    if (el.type == "progressbar") return el.barH;
    if (el.type == "icons")       return el.iconSize + el.iconGap;
    if (el.type == "button")      return el.h;
    return 0;
}

// ---------------------------------------------------------------------------
// Core renderer — takes plain C++ data, no Lua

float UIManager::drawWidget(const std::string& name, float x, float y, const UIWidgetRenderData& data)
{
    auto it = widgets.find(name);
    if (it == widgets.end()) {
        printf("[UIManager] WARN: widget '%s' not found\n", name.c_str());
        return y;
    }

    UIWidget& w = it->second;
    lastClickedId = "";
    float s = w.scale > 0 ? w.scale : 1.0f;
    static const UIElementData defaultData;
    float autoY = y;
    for (auto& el : w.elements) {
        auto dit = data.find(el.id);
        const UIElementData& ed = (dit != data.end()) ? dit->second : defaultData;

        UIElement scaled = el;
        scaled.x           *= s;
        scaled.y           *= s;
        scaled.w           *= s;
        scaled.h           *= s;
        scaled.fontScale   *= s;
        scaled.barW        *= s;
        scaled.barH        *= s;
        scaled.barOffsetY  *= s;
        scaled.barFontScale*= s;
        scaled.iconSize    *= s;
        scaled.iconGap     *= s;

        float drawY = el.yAuto ? autoY : y + scaled.y;
        renderElement(scaled, x, drawY, ed);
        if (el.yAuto) autoY += getElementHeight(scaled);
    }
    return y + w.height * s;
}

void UIManager::renderElement(const UIElement& el, float baseX, float baseY, const UIElementData& data)
{
    if (el.type == "text")        { renderText(el, baseX, baseY, data);        return; }
    if (el.type == "image")       { renderImage(el, baseX, baseY, data);       return; }
    if (el.type == "rect")        { renderRect(el, baseX, baseY, data);        return; }
    if (el.type == "progressbar") { renderProgressBar(el, baseX, baseY, data); return; }
    if (el.type == "icons")       { renderIcons(el, baseX, baseY, data);       return; }
    if (el.type == "button")      { renderButton(el, baseX, baseY, data);      return; }
}

void UIManager::renderText(const UIElement& el, float x, float y, const UIElementData& data)
{
    if (data.text.empty()) return;
    tw->writeTextAtlas((int)(x + el.x), (int)y, data.text.c_str(), data.color, el.fontScale);
}

void UIManager::renderImage(const UIElement& el, float x, float y, const UIElementData& data)
{
    if (data.path.empty()) return;
    render->DrawImage2D(data.path, (int)(x + el.x), (int)y,
        (int)(el.w * el.imageScale), (int)(el.h * el.imageScale));
}

void UIManager::renderRect(const UIElement& el, float x, float y, const UIElementData& data)
{
    auto* win = Components::get()->Window();
    float w = el.wPct > 0 ? (float)win->getWidthRender()  * el.wPct
            : data.w  > 0 ? data.w : el.w;
    float h = el.hPct > 0 ? (float)win->getHeightRender() * el.hPct
            : data.h  > 0 ? data.h : el.h;

    float rx = x + el.x;
    if      (el.alignH == "center") rx -= w * 0.5f;
    else if (el.alignH == "right")  rx -= w;

    float ry = y;
    if      (el.alignV == "center") ry -= h * 0.5f;
    else if (el.alignV == "bottom") ry -= h;

    Color c = data.color;
    c.a *= el.alpha;
    render->DrawFilledRect((int)rx, (int)ry, (int)w, (int)h, c);
}

void UIManager::renderProgressBar(const UIElement& el, float x, float y, const UIElementData& data)
{
    float maxV = data.maxValue > 0 ? data.maxValue : 1.0f;
    float ratio = data.value / maxV;
    float bw = el.barW;
    float bh = el.barH;

    float barY = y + el.barOffsetY;
    render->DrawFilledRect((int)(x + el.x), (int)barY, (int)bw, (int)bh, el.barBg);

    Color fillColor = el.barOk;
    if (ratio < 0.33f) fillColor = el.barLow;
    else if (ratio < 0.66f) fillColor = el.barMid;

    int fillW = (int)(bw * ratio);
    if (fillW > 0)
        render->DrawFilledRect((int)(x + el.x), (int)barY, fillW, (int)bh, fillColor);

    std::string label = std::to_string((int)data.value) + "/" + std::to_string((int)data.maxValue);
    tw->writeTextAtlas((int)(x + el.x + el.textOffsetX), (int)barY, label.c_str(), Color::white(), el.barFontScale);
}

void UIManager::renderIcons(const UIElement& el, float x, float y, const UIElementData& data)
{
    float cx = x + el.x;
    for (auto& key : data.iconList) {
        auto mapIt = el.iconMapping.find(key);
        if (mapIt == el.iconMapping.end()) continue;
        render->DrawImage2D(mapIt->second, (int)cx, (int)y, (int)el.iconSize, (int)el.iconSize);
        cx += el.iconSize + el.iconGap;
    }
}

void UIManager::renderButton(const UIElement& el, float x, float y, const UIElementData& data)
{
    if (!data.provided) return;

    float bx = x + el.x;
    float by = y;
    float bw = el.w > 0 ? el.w : 80.0f;
    float bh = el.h > 0 ? el.h : 20.0f;

    int mx = input ? input->getRawMouseX() : -1;
    int my = input ? input->getRawMouseY() : -1;
    bool hovered = mx >= (int)bx && mx <= (int)(bx + bw) &&
                   my >= (int)by && my <= (int)(by + bh);
    bool pressed = hovered && input && input->isLeftMouseButtonPressed();
    bool clicked = hovered && input && input->isMouseButtonDown();

    Color bg = pressed ? el.btnPressed : hovered ? el.btnHover : el.btnBg;
    render->DrawFilledRect((int)bx, (int)by, (int)bw, (int)bh, bg);

    if (!data.text.empty()) {
        float scale = el.fontScale > 0 ? el.fontScale : 0.45f;
        float textW = scale * (float)data.text.size() * 7.5f;
        float textH = scale * 20.0f;
        int tx = (int)(bx + (bw - textW) * 0.5f);
        int ty = (int)(by + (bh - textH) * 0.5f);
        Color col = data.color;
        if (col.r == 1 && col.g == 1 && col.b == 1 && col.a == 1)
            col = Color(0.92f, 0.92f, 0.92f, 1.0f);
        tw->writeTextAtlas(tx, ty, data.text.c_str(), col, scale);
    }

    if (clicked && lastClickedId.empty())
        lastClickedId = el.id;
}

// ---------------------------------------------------------------------------
// Lua wrapper — converts sol::table → UIWidgetRenderData and calls core

UIElementData UIManager::solTableToElementData(sol::table t)
{
    UIElementData d;
    d.provided = true;

    sol::object textObj = t["text"];
    if (textObj.valid() && textObj.get_type() == sol::type::string)
        d.text = textObj.as<std::string>();

    sol::object colorObj = t["color"];
    if (colorObj.valid()) d.color = colorObj.as<Color>();

    sol::object pathObj = t["path"];
    if (pathObj.valid() && pathObj.get_type() == sol::type::string)
        d.path = pathObj.as<std::string>();

    sol::object wObj = t["w"];
    if (wObj.valid()) d.w = wObj.as<float>();

    sol::object hObj = t["h"];
    if (hObj.valid()) d.h = hObj.as<float>();

    sol::object valueObj = t["value"];
    if (valueObj.valid()) d.value = valueObj.as<float>();

    sol::object maxObj = t["max"];
    if (maxObj.valid()) d.maxValue = maxObj.as<float>();

    sol::object listObj = t["list"];
    if (listObj.valid() && listObj.get_type() == sol::type::table) {
        sol::table list = listObj;
        list.for_each([&](sol::object, sol::object val) {
            if (val.get_type() == sol::type::string)
                d.iconList.push_back(val.as<std::string>());
        });
    }

    return d;
}

std::tuple<float, std::string> UIManager::drawWidgetLua(const std::string& name, float x, float y, sol::table data)
{
    auto it = widgets.find(name);
    if (it == widgets.end()) {
        printf("[UIManager] WARN: widget '%s' not found\n", name.c_str());
        return {y, ""};
    }

    UIWidgetRenderData renderData;
    for (auto& el : it->second.elements) {
        sol::object elObj = data[el.id];
        if (elObj.valid() && elObj.get_type() == sol::type::table)
            renderData[el.id] = solTableToElementData(elObj.as<sol::table>());
    }

    float nextY = drawWidget(name, x, y, renderData);
    return {nextY, lastClickedId};
}
