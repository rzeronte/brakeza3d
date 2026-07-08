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
    loadedDirs.clear();
    loadWidgetsFromDir(widgetsDir);
}

void UIManager::reloadWidgets()
{
    widgets.clear();
    auto dirs = loadedDirs;   // snapshot — loadWidgetsFromDir modifica loadedDirs
    loadedDirs.clear();
    for (auto& dir : dirs)
        loadWidgetsFromDir(dir);
    printf("[UIManager] reloaded\n");
}

void UIManager::clearWidgets()
{
    widgets.clear();
    loadedDirs.clear();
}

void UIManager::loadWidgetsFromDir(const std::string& dir)
{
    if (!std::filesystem::exists(dir)) {
        printf("[UIManager] WARN: directory not found: %s\n", dir.c_str());
        return;
    }

    auto addDir = [&](const std::string& d) {
        if (std::find(loadedDirs.begin(), loadedDirs.end(), d) == loadedDirs.end())
            loadedDirs.push_back(d);
    };
    addDir(dir);

    // Collect all .json paths recursively, registering subdirs along the way.
    std::vector<std::string> paths;
    for (auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
        if (entry.is_directory()) {
            addDir(entry.path().string() + "/");
        } else if (entry.path().extension() == ".json") {
            paths.push_back(entry.path().string());
        }
    }

    // Pass 1: parse every file and store in the map (no ref resolution, no height).
    size_t before = widgets.size();
    for (auto& p : paths)
        parseWidgetJSON(p);


    printf("[UIManager] loaded %zu widget(s) from %s (recursive)\n", widgets.size() - before, dir.c_str());
}

void UIManager::loadWidgetFromFile(const std::string& filePath)
{
    loadWidget(filePath);
    printf("[UIManager] loaded widget from %s\n", filePath.c_str());
}

void UIManager::unloadWidget(const std::string& name)
{
    if (widgets.erase(name))
        printf("[UIManager] unloaded widget '%s'\n", name.c_str());
    else
        printf("[UIManager] WARN: widget '%s' not found for unload\n", name.c_str());
}

// ---------------------------------------------------------------------------
// parseWidgetJSON — parse + store in map; no ref resolution, no height computation.
// Used by the two-pass bulk loader (loadWidgetsFromDir).
void UIManager::parseWidgetJSON(const std::string& filePath)
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
    widget.filePath = filePath;

    cJSON* scaleItem = cJSON_GetObjectItem(root, "scale");
    if (scaleItem) widget.scale = (float)scaleItem->valuedouble;
    cJSON* posXItem = cJSON_GetObjectItem(root, "posX");
    if (posXItem) widget.posX = (float)posXItem->valuedouble;
    cJSON* posYItem = cJSON_GetObjectItem(root, "posY");
    if (posYItem) widget.posY = (float)posYItem->valuedouble;
    cJSON* widthItem = cJSON_GetObjectItem(root, "width");
    if (widthItem) widget.width = (float)widthItem->valuedouble;
    cJSON* heightItem = cJSON_GetObjectItem(root, "height");
    if (heightItem) widget.height = (float)heightItem->valuedouble;

    auto parseColor4 = [](cJSON* node, Color& c) {
        if (!node) return;
        cJSON* r = cJSON_GetObjectItem(node, "r"); if (r) c.r = (float)r->valuedouble;
        cJSON* g = cJSON_GetObjectItem(node, "g"); if (g) c.g = (float)g->valuedouble;
        cJSON* b = cJSON_GetObjectItem(node, "b"); if (b) c.b = (float)b->valuedouble;
        cJSON* a = cJSON_GetObjectItem(node, "a"); if (a) c.a = (float)a->valuedouble;
    };
    parseColor4(cJSON_GetObjectItem(root, "bgColor"),     widget.bgColor);
    parseColor4(cJSON_GetObjectItem(root, "borderColor"), widget.borderColor);
    cJSON* bwItem = cJSON_GetObjectItem(root, "borderWidth");
    if (bwItem) widget.borderWidth = (float)bwItem->valuedouble;

    cJSON* elementsArr = cJSON_GetObjectItem(root, "elements");
    if (elementsArr && cJSON_IsArray(elementsArr)) {
        int arrSize = cJSON_GetArraySize(elementsArr);
        for (int i = 0; i < arrSize; i++) {
            cJSON* item = cJSON_GetArrayItem(elementsArr, i);
            if (!item) continue;
            UIElement el;
            cJSON* id   = cJSON_GetObjectItem(item, "id");   if (id)   el.id   = id->valuestring;
            cJSON* type = cJSON_GetObjectItem(item, "type");
            if (type) {
                el.type = type->valuestring;
                if (el.type == "hpbar") el.type = "progressbar";  // backward compat
            }
            cJSON* xItem = cJSON_GetObjectItem(item, "x"); if (xItem) el.x = (float)xItem->valuedouble;
            cJSON* yItem = cJSON_GetObjectItem(item, "y"); if (yItem) el.y = (float)yItem->valuedouble;
            cJSON* yAutoItem = cJSON_GetObjectItem(item, "yAuto");
            if (yAutoItem) el.yAuto = (yAutoItem->type == cJSON_True);
            cJSON* wItem = cJSON_GetObjectItem(item, "w"); if (wItem) el.w = (float)wItem->valuedouble;
            cJSON* hItem = cJSON_GetObjectItem(item, "h"); if (hItem) el.h = (float)hItem->valuedouble;
            cJSON* wPct = cJSON_GetObjectItem(item, "wPct"); if (wPct) el.wPct = (float)wPct->valuedouble;
            cJSON* hPct = cJSON_GetObjectItem(item, "hPct"); if (hPct) el.hPct = (float)hPct->valuedouble;
            cJSON* alignH = cJSON_GetObjectItem(item, "alignH"); if (alignH && alignH->valuestring) el.alignH = alignH->valuestring;
            cJSON* alignV = cJSON_GetObjectItem(item, "alignV"); if (alignV && alignV->valuestring) el.alignV = alignV->valuestring;
            cJSON* alpha = cJSON_GetObjectItem(item, "alpha"); if (alpha) el.alpha = (float)alpha->valuedouble;
            cJSON* rectCol = cJSON_GetObjectItem(item, "color"); if (rectCol) el.rectColor = ToolsJSON::getColorByJSON(rectCol);
            cJSON* fontScale = cJSON_GetObjectItem(item, "fontScale"); if (fontScale) el.fontScale = (float)fontScale->valuedouble;
            cJSON* textCol = cJSON_GetObjectItem(item, "textColor"); if (textCol) el.staticTextColor = ToolsJSON::getColorByJSON(textCol);
            cJSON* staticText = cJSON_GetObjectItem(item, "text"); if (staticText && staticText->valuestring) el.staticText = staticText->valuestring;
            cJSON* imageScale = cJSON_GetObjectItem(item, "imageScale"); if (imageScale) el.imageScale = (float)imageScale->valuedouble;
            cJSON* staticImg = cJSON_GetObjectItem(item, "imagePath"); if (staticImg && staticImg->valuestring) el.staticImagePath = staticImg->valuestring;
            cJSON* staticImgOff = cJSON_GetObjectItem(item, "imagePathOff"); if (staticImgOff && staticImgOff->valuestring) el.staticImagePathOff = staticImgOff->valuestring;
            cJSON* btnBg  = cJSON_GetObjectItem(item, "btnBg");      if (btnBg)  el.btnBg      = ToolsJSON::getColorByJSON(btnBg);
            cJSON* btnHov = cJSON_GetObjectItem(item, "btnHover");   if (btnHov) el.btnHover   = ToolsJSON::getColorByJSON(btnHov);
            cJSON* btnPrs = cJSON_GetObjectItem(item, "btnPressed"); if (btnPrs) el.btnPressed = ToolsJSON::getColorByJSON(btnPrs);
            cJSON* bImgOX = cJSON_GetObjectItem(item, "btnImgOffsetX");  if (bImgOX) el.btnImgOffsetX  = (float)bImgOX->valuedouble;
            cJSON* bImgOY = cJSON_GetObjectItem(item, "btnImgOffsetY");  if (bImgOY) el.btnImgOffsetY  = (float)bImgOY->valuedouble;
            cJSON* bTxtOX = cJSON_GetObjectItem(item, "btnTextOffsetX"); if (bTxtOX) el.btnTextOffsetX = (float)bTxtOX->valuedouble;
            cJSON* bTxtOY = cJSON_GetObjectItem(item, "btnTextOffsetY"); if (bTxtOY) el.btnTextOffsetY = (float)bTxtOY->valuedouble;
            cJSON* barW = cJSON_GetObjectItem(item, "barW"); if (!barW) barW = cJSON_GetObjectItem(item, "hpw");
            if (barW) el.barW = (float)barW->valuedouble;
            cJSON* barH = cJSON_GetObjectItem(item, "barH"); if (!barH) barH = cJSON_GetObjectItem(item, "hph");
            if (barH) el.barH = (float)barH->valuedouble;
            cJSON* tox = cJSON_GetObjectItem(item, "textOffsetX"); if (tox) el.textOffsetX = (float)tox->valuedouble;
            cJSON* toy = cJSON_GetObjectItem(item, "textOffsetY"); if (toy) el.textOffsetY = (float)toy->valuedouble;
            cJSON* bfs = cJSON_GetObjectItem(item, "barFontScale"); if (!bfs) bfs = cJSON_GetObjectItem(item, "hpFontScale");
            if (bfs) el.barFontScale = (float)bfs->valuedouble;
            cJSON* barBg  = cJSON_GetObjectItem(item, "barBg");  if (barBg)  el.barBg  = ToolsJSON::getColorByJSON(barBg);
            cJSON* barOk  = cJSON_GetObjectItem(item, "barOk");  if (barOk)  el.barOk  = ToolsJSON::getColorByJSON(barOk);
            cJSON* barMid = cJSON_GetObjectItem(item, "barMid"); if (barMid) el.barMid = ToolsJSON::getColorByJSON(barMid);
            cJSON* barLow = cJSON_GetObjectItem(item, "barLow"); if (barLow) el.barLow = ToolsJSON::getColorByJSON(barLow);
            cJSON* iconSz  = cJSON_GetObjectItem(item, "size"); if (iconSz)  el.iconSize = (float)iconSz->valuedouble;
            cJSON* iconGap = cJSON_GetObjectItem(item, "gap");  if (iconGap) el.iconGap  = (float)iconGap->valuedouble;
            cJSON* mapping = cJSON_GetObjectItem(item, "mapping");
            if (mapping && cJSON_IsObject(mapping)) {
                cJSON* kv = nullptr;
                cJSON_ArrayForEach(kv, mapping) if (kv->valuestring) el.iconMapping[kv->string] = kv->valuestring;
            }
            cJSON* widgetRef = cJSON_GetObjectItem(item, "widgetRef");
            if (widgetRef && widgetRef->valuestring) el.widgetRef = widgetRef->valuestring;
            cJSON* arrayCount = cJSON_GetObjectItem(item, "arrayCount");
            if (arrayCount) el.arrayCount = arrayCount->valueint;
            cJSON* arrayAlign = cJSON_GetObjectItem(item, "arrayAlign");
            if (arrayAlign && arrayAlign->valuestring) el.arrayAlign = arrayAlign->valuestring;
            cJSON* arrayDistribute = cJSON_GetObjectItem(item, "arrayDistribute");
            if (arrayDistribute) el.arrayDistribute = (arrayDistribute->type == cJSON_True);
            cJSON* arrayOffset = cJSON_GetObjectItem(item, "arrayOffset");
            if (arrayOffset) el.arrayOffset = (float)arrayOffset->valuedouble;
            cJSON* borderCol = cJSON_GetObjectItem(item, "borderColor");
            if (borderCol) el.borderColor = ToolsJSON::getColorByJSON(borderCol);
            cJSON* borderW = cJSON_GetObjectItem(item, "borderWidth");
            if (borderW) el.borderWidth = (float)borderW->valuedouble;
            cJSON* bgColItem = cJSON_GetObjectItem(item, "bgColor");
            if (bgColItem) el.bgColor = ToolsJSON::getColorByJSON(bgColItem);
            cJSON* tooltipItem = cJSON_GetObjectItem(item, "tooltip");
            if (tooltipItem && tooltipItem->valuestring) el.tooltip = tooltipItem->valuestring;
            cJSON* optArr = cJSON_GetObjectItem(item, "options");
            if (optArr && cJSON_IsArray(optArr)) {
                int optSize = cJSON_GetArraySize(optArr);
                for (int j = 0; j < optSize; j++) {
                    cJSON* opt = cJSON_GetArrayItem(optArr, j);
                    if (!opt) continue;
                    UIElementOption o;
                    cJSON* ok  = cJSON_GetObjectItem(opt, "key");           if (ok  && ok->valuestring)  o.key           = ok->valuestring;
                    cJSON* otx = cJSON_GetObjectItem(opt, "text");          if (otx && otx->valuestring) o.text          = otx->valuestring;
                    cJSON* oim = cJSON_GetObjectItem(opt, "image");         if (oim && oim->valuestring) o.image         = oim->valuestring;
                    cJSON* ott = cJSON_GetObjectItem(opt, "tooltip");       if (ott && ott->valuestring) o.tooltip       = ott->valuestring;
                    cJSON* otw = cJSON_GetObjectItem(opt, "tooltipWidget"); if (otw && otw->valuestring) o.tooltipWidget = otw->valuestring;
                    cJSON* oc  = cJSON_GetObjectItem(opt, "color");         if (oc) { o.color = ToolsJSON::getColorByJSON(oc); o.colorSet = true; }
                    el.options.push_back(o);
                }
            }
            cJSON* paddingLeft   = cJSON_GetObjectItem(item, "paddingLeft");
            if (paddingLeft)   el.paddingLeft   = (float)paddingLeft->valuedouble;
            cJSON* paddingTop    = cJSON_GetObjectItem(item, "paddingTop");
            if (paddingTop)    el.paddingTop    = (float)paddingTop->valuedouble;
            cJSON* paddingBottom = cJSON_GetObjectItem(item, "paddingBottom");
            if (paddingBottom) el.paddingBottom = (float)paddingBottom->valuedouble;
            widget.elements.push_back(el);
        }
    }

    cJSON* dataSection = cJSON_GetObjectItem(root, "data");
    if (dataSection && cJSON_IsObject(dataSection)) {
        cJSON* entry = nullptr;
        cJSON_ArrayForEach(entry, dataSection) {
            if (!cJSON_IsObject(entry)) continue;
            UIElementData ed;
            bool hasContent = false;
            cJSON* t = cJSON_GetObjectItem(entry, "text");   if (t && t->valuestring) { ed.text = t->valuestring; hasContent |= !ed.text.empty(); }
            cJSON* p = cJSON_GetObjectItem(entry, "path");   if (p && p->valuestring) { ed.path = p->valuestring; hasContent |= !ed.path.empty(); }
            cJSON* pOff = cJSON_GetObjectItem(entry, "pathOff"); if (pOff && pOff->valuestring) { ed.pathOff = pOff->valuestring; hasContent |= !ed.pathOff.empty(); }
            cJSON* enab = cJSON_GetObjectItem(entry, "enabled"); if (enab) { ed.enabled = cJSON_IsTrue(enab); hasContent = true; }
            cJSON* col  = cJSON_GetObjectItem(entry, "color"); if (col) { ed.color = ToolsJSON::getColorByJSON(col); ed.colorProvided = true; hasContent = true; }
            cJSON* val  = cJSON_GetObjectItem(entry, "value"); if (val) { ed.value = (float)val->valuedouble; hasContent = true; }
            cJSON* maxv = cJSON_GetObjectItem(entry, "max");   if (maxv) { ed.maxValue = (float)maxv->valuedouble; hasContent = true; }
            cJSON* list = cJSON_GetObjectItem(entry, "list");
            if (list && cJSON_IsArray(list)) {
                cJSON* icon = nullptr;
                cJSON_ArrayForEach(icon, list) if (icon->valuestring) ed.iconList.push_back(icon->valuestring);
                hasContent |= !ed.iconList.empty();
            }
            if (hasContent) { ed.provided = true; widget.defaultData[entry->string] = ed; }
        }
    }

    cJSON_Delete(root);
    widgets[widgetName] = widget;
}

// ---------------------------------------------------------------------------
// loadWidget — single-file load with ref resolution and height computation.
// Used by loadWidgetFromFile (editor, runtime on-demand).
void UIManager::loadWidget(const std::string& filePath)
{
    parseWidgetJSON(filePath);

    std::string widgetName = std::filesystem::path(filePath).stem().string();
    auto it = widgets.find(widgetName);
    if (it == widgets.end()) return;
    UIWidget& widget = it->second;

    // Auto-load referenced widgets not yet in memory.
    // Searches: same dir first, then all loadedDirs (includes subdirs).
    std::string parentDir = std::filesystem::path(filePath).parent_path().string() + "/";
    for (auto& el : widget.elements) {
        if ((el.type == "widget" || el.type == "array") && !el.widgetRef.empty()) {
            if (!widgets.count(el.widgetRef)) {
                std::string refPath = parentDir + el.widgetRef + ".json";
                if (!std::filesystem::exists(refPath)) {
                    for (auto& ld : loadedDirs) {
                        std::string alt = ld + el.widgetRef + ".json";
                        if (std::filesystem::exists(alt)) { refPath = alt; break; }
                    }
                }
                if (std::filesystem::exists(refPath))
                    loadWidget(refPath);
            }
        }
    }

    printf("[UIManager] loaded widget '%s' (%zu elements, w=%.3f h=%.3f)\n",
        widgetName.c_str(), widget.elements.size(), widget.width, widget.height);
}


float UIManager::getElementHeight(const UIElement& el)
{
    if (el.type == "text")        return el.fontScale * 20.0f;
    if (el.type == "image")       return el.h * el.imageScale;
    if (el.type == "rect")        return (el.h > 0) ? el.h : 0;
    if (el.type == "progressbar") return el.barH;
    if (el.type == "icons")       return el.iconSize + el.iconGap;
    if (el.type == "button")      return el.h;
    if (el.type == "widget") {
        auto it = widgets.find(el.widgetRef);
        if (it != widgets.end()) {
            float ss = it->second.scale > 0 ? it->second.scale : 1.0f;
            return it->second.height * ss;
        }
    }
    if (el.type == "array") {
        auto it = widgets.find(el.widgetRef);
        if (it != widgets.end()) {
            float ss = it->second.scale > 0 ? it->second.scale : 1.0f;
            if (el.arrayAlign == "horizontal") return it->second.height * ss;
            return it->second.height * ss * (float)el.arrayCount;
        }
    }
    return 0;
}

// ---------------------------------------------------------------------------
// Core renderer — takes plain C++ data, no Lua

float UIManager::renderWidgetAt(UIWidget& w, float x, float y, const UIWidgetRenderData& data, bool useDefaultData)
{
    if (widgetRenderDepth == 0) lastClickedId = "";
    widgetRenderDepth++;
    static const UIElementData defaultData;
    const UIWidgetRenderData* prevData = currentData;
    currentData = &data;

    float prevRenderScale = currentRenderScale;
    float ownScale = w.scale > 0 ? w.scale : 1.0f;
    float s = ownScale * prevRenderScale;   // accumulated: parent scale × own scale
    currentRenderScale = s;

    // x/y/w/h on elements are 0..1 fractions of the widget box (cw × ch in screen px)
    auto* winPtr = Components::get()->Window();
    const float cw = (float)winPtr->getWidth()  * (w.width  > 0 ? w.width  : 1.0f);
    const float ch = (float)winPtr->getHeight() * (w.height > 0 ? w.height : 1.0f);

    auto buildScaled = [s, cw, ch](const UIElement& el) {
        UIElement sc = el;
        sc.x              = (el.x * cw + el.paddingLeft * cw) * s;
        sc.y              = el.y * ch * s;
        sc.w              = el.w * cw * s;
        sc.h              = el.h * ch * s;
        sc.barW           = el.barW           * cw * s;
        sc.barH           = el.barH           * ch * s;
        sc.iconSize       = el.iconSize       * ch * s;
        sc.iconGap        = el.iconGap        * cw * s;
        sc.textOffsetX    = el.textOffsetX    * cw * s;
        sc.textOffsetY    = el.textOffsetY    * ch * s;
        sc.btnImgOffsetX  = el.btnImgOffsetX  * sc.w;
        sc.btnImgOffsetY  = el.btnImgOffsetY  * sc.h;
        sc.btnTextOffsetX = el.btnTextOffsetX * sc.w;
        sc.btnTextOffsetY = el.btnTextOffsetY * sc.h;
        sc.paddingLeft    = el.paddingLeft    * cw * s;
        sc.paddingTop     = el.paddingTop     * ch * s;
        sc.paddingBottom  = el.paddingBottom  * ch * s;
        sc.arrayOffset    = el.arrayOffset    * ch * s;
        sc.fontScale      = el.fontScale      * s;
        sc.barFontScale   = el.barFontScale   * s;
        return sc;
    };

    auto resolveData = [&](const UIElement& el) -> const UIElementData& {
        auto dit = data.find(el.id);
        if (dit != data.end()) return dit->second;
        if (useDefaultData) {
            auto dit2 = w.defaultData.find(el.id);
            if (dit2 != w.defaultData.end()) return dit2->second;
        }
        return defaultData;
    };

    // Height of one element, accounting for dynamic array count override from data.
    auto getHeightDynamic = [&](const UIElement& sc, const UIElementData& elData) -> float {
        if (sc.type == "array") {
            auto it2 = widgets.find(sc.widgetRef);
            if (it2 != widgets.end()) {
                float ss = it2->second.scale > 0 ? it2->second.scale : 1.0f;
                int cnt = (elData.count > 0) ? elData.count : sc.arrayCount;
                if (sc.arrayAlign == "horizontal") return it2->second.height * ss * s;
                float step = it2->second.height * ss * s + sc.arrayOffset;
                return step * (float)cnt;
            }
        }
        return getElementHeight(sc);
    };

    float dynamicH = ch * s;
    float dynamicW = cw * s;

    // Draw widget background before any element.
    if (w.bgColor.a > 0.0f) {
        render->DrawFilledRectToFB(
            (int)x, (int)y, (int)dynamicW, (int)dynamicH, applyGA(w.bgColor), currentFB);
    }

    // Pass 1: rects, images, icons (no text)
    textPass = false;
    float autoY = y;
    for (auto& el : w.elements) {
        UIElement scaled = buildScaled(el);
        if (el.yAuto) autoY += scaled.paddingTop;
        float drawY = el.yAuto ? autoY : y + scaled.y + scaled.paddingTop;
        renderElement(scaled, x, drawY, resolveData(el));
        if (el.yAuto) autoY += getHeightDynamic(scaled, resolveData(el)) + scaled.paddingBottom;
    }

    // Pass 2: texts only
    textPass = true;
    autoY = y;
    for (auto& el : w.elements) {
        UIElement scaled = buildScaled(el);
        if (el.yAuto) autoY += scaled.paddingTop;
        float drawY = el.yAuto ? autoY : y + scaled.y + scaled.paddingTop;
        renderElement(scaled, x, drawY, resolveData(el));
        if (el.yAuto) autoY += getHeightDynamic(scaled, resolveData(el)) + scaled.paddingBottom;
    }

    // Draw widget border after all elements.
    if (w.borderWidth > 0.0f) {
        int bw = (int)w.borderWidth;
        Color bc = applyGA(w.borderColor);
        int ix = (int)x, iy = (int)y, iw = (int)dynamicW, ih = (int)dynamicH;
        render->DrawFilledRectToFB(ix,       iy,       iw, bw, bc, currentFB);
        render->DrawFilledRectToFB(ix,       iy+ih-bw, iw, bw, bc, currentFB);
        render->DrawFilledRectToFB(ix,       iy,       bw, ih, bc, currentFB);
        render->DrawFilledRectToFB(ix+iw-bw, iy,       bw, ih, bc, currentFB);
    }

    textPass = false;
    currentData = prevData;
    currentRenderScale = prevRenderScale;
    widgetRenderDepth--;
    return y + dynamicH;
}

// Renders at the widget's stored (posX*winW, posY*winH).
float UIManager::drawWidget(const std::string& name, const UIWidgetRenderData& data, const std::string& fb, float scaleOverride)
{
    auto it = widgets.find(name);
    if (it == widgets.end()) {
        printf("[UIManager] WARN: widget '%s' not found\n", name.c_str());
        return 0.0f;
    }
    UIWidget& w = it->second;
    currentFB = fb;

    auto* win = Components::get()->Window();
    float ox = w.posX * (float)win->getWidth();
    float oy = w.posY * (float)win->getHeight();

    float prevScale = w.scale;
    if (scaleOverride > 0.0f) w.scale = w.scale * scaleOverride;
    float result = renderWidgetAt(w, ox, oy, data);
    w.scale = prevScale;
    return result;
}

// Renders at exactly (x, y) in pixels — ignores stored posX/posY.
float UIManager::drawWidgetAtPos(const std::string& name, float x, float y, const UIWidgetRenderData& data, const std::string& fb, float scaleOverride)
{
    auto it = widgets.find(name);
    if (it == widgets.end()) {
        printf("[UIManager] WARN: widget '%s' not found\n", name.c_str());
        return y;
    }
    UIWidget& w = it->second;
    currentFB = fb;

    float prevScale = w.scale;
    if (scaleOverride > 0.0f) w.scale = w.scale * scaleOverride;
    float result = renderWidgetAt(w, x, y, data);
    w.scale = prevScale;
    return result;
}

static UIWidgetRenderData filterDataByPrefix(const UIWidgetRenderData& data, const std::string& prefix)
{
    UIWidgetRenderData out;
    for (auto& [k, v] : data) {
        if (k.size() > prefix.size() && k.compare(0, prefix.size(), prefix) == 0)
            out[k.substr(prefix.size())] = v;
    }
    return out;
}

void UIManager::renderElement(const UIElement& el, float baseX, float baseY, const UIElementData& data)
{
    // Helper: compute step for array items.
    // - arrayDistribute=false (default): step = child natural size × scales (fixed, set at load time).
    // - arrayDistribute=true: step = full window dimension / arrayCount (computed at runtime).
    //   In distribute mode the child widget is still rendered at its own scale; only the spacing changes.
    int resolvedCount = (data.count > 0) ? data.count : el.arrayCount;

    auto arrayStep = [&](UIWidget& child) -> std::pair<float,float> {
        float stepX = 0.0f, stepY = 0.0f;
        if (el.arrayDistribute) {
            auto* win = Components::get()->Window();
            if (el.arrayAlign == "horizontal")
                stepX = (float)win->getWidthRender() / (float)resolvedCount;
            else
                stepY = (float)win->getHeightRender() / (float)resolvedCount;
        } else {
            auto* win = Components::get()->Window();
            float ss = child.scale > 0 ? child.scale : 1.0f;
            if (el.arrayAlign == "horizontal")
                stepX = (float)win->getWidthRender()  * child.width  * ss * currentRenderScale + el.arrayOffset;
            else
                stepY = (float)win->getHeightRender() * child.height * ss * currentRenderScale + el.arrayOffset;
        }
        return {stepX, stepY};
    };

    if (textPass) {
        if (el.type == "text")        renderText(el, baseX, baseY, data);
        if (el.type == "progressbar") renderProgressBar(el, baseX, baseY, data);
        if (el.type == "button")      renderButton(el, baseX, baseY, data);
        if (el.type == "widget" || el.type == "array") {
            if (renderDepth < 4) {
                auto it = widgets.find(el.widgetRef);
                if (it != widgets.end()) {
                    renderDepth++;
                    if (el.type == "widget") {
                        const UIWidgetRenderData& fullData = currentData ? *currentData : UIWidgetRenderData{};
                        auto sub = filterDataByPrefix(fullData, el.id + "_");
                        if (sub.empty() && !data.provided) { renderDepth--; return; }
                        renderWidgetAt(it->second, baseX + el.x, baseY + el.y, sub, true);
                    } else {
                        auto [stepX, stepY] = arrayStep(it->second);
                        const UIWidgetRenderData& fullData = currentData ? *currentData : UIWidgetRenderData{};
                        for (int i = 0; i < resolvedCount; i++) {
                            auto sub = filterDataByPrefix(fullData, std::to_string(i) + "_");
                            renderWidgetAt(it->second,
                                baseX + el.x + (float)i * stepX,
                                baseY + el.y + (float)i * stepY, sub, false);
                        }
                    }
                    renderDepth--;
                }
            }
        }
        return;
    }
    if (el.type == "image")       { renderImage(el, baseX, baseY, data);       return; }
    if (el.type == "rect")        { renderRect(el, baseX, baseY, data);        return; }
    if (el.type == "progressbar") { renderProgressBar(el, baseX, baseY, data); return; }
    if (el.type == "icons")       { renderIcons(el, baseX, baseY, data);       return; }
    if (el.type == "button")      { renderButton(el, baseX, baseY, data);      return; }
    if (el.type == "widget" || el.type == "array") {
        if (renderDepth >= 4) return;
        auto it = widgets.find(el.widgetRef);
        if (it == widgets.end()) return;
        renderDepth++;
        if (el.type == "widget") {
            const UIWidgetRenderData& fullData = currentData ? *currentData : UIWidgetRenderData{};
            auto sub = filterDataByPrefix(fullData, el.id + "_");
            if (sub.empty() && !data.provided) { renderDepth--; return; }
            renderWidgetAt(it->second, baseX + el.x, baseY + el.y, sub, true);
        } else {
            auto [stepX, stepY] = arrayStep(it->second);
            const UIWidgetRenderData& fullData = currentData ? *currentData : UIWidgetRenderData{};
            auto* winPtr = Components::get()->Window();
            float ss = it->second.scale > 0 ? it->second.scale : 1.0f;
            float cw = (float)winPtr->getWidthRender()  * it->second.width  * ss * currentRenderScale;
            float ch = (float)winPtr->getHeightRender() * it->second.height * ss * currentRenderScale;
            if (el.bgColor.a > 0.0f) {
                float totalW = (el.arrayAlign == "horizontal") ? (float)(resolvedCount - 1) * stepX + cw : cw;
                float totalH = (el.arrayAlign == "horizontal") ? ch : (float)(resolvedCount - 1) * stepY + ch;
                render->DrawFilledRectToFB((int)(baseX + el.x), (int)(baseY + el.y), (int)totalW, (int)totalH, applyGA(el.bgColor), currentFB);
            }
            for (int i = 0; i < resolvedCount; i++) {
                std::string prevId = lastClickedId;
                float ix = baseX + el.x + (float)i * stepX;
                float iy = baseY + el.y + (float)i * stepY;
                auto sub = filterDataByPrefix(fullData, std::to_string(i) + "_");
                renderWidgetAt(it->second, ix, iy, sub, false);
                if (el.borderWidth > 0.0f) {
                    int bord = (int)el.borderWidth;
                    Color bc = applyGA(el.borderColor);
                    render->DrawFilledRectToFB((int)ix,       (int)iy,       (int)cw, bord,   bc, currentFB);
                    render->DrawFilledRectToFB((int)ix,       (int)(iy+ch-bord), (int)cw, bord, bc, currentFB);
                    render->DrawFilledRectToFB((int)ix,       (int)iy,       bord, (int)ch,   bc, currentFB);
                    render->DrawFilledRectToFB((int)(ix+cw-bord), (int)iy,   bord, (int)ch,   bc, currentFB);
                }
                if (lastClickedId != prevId && !lastClickedId.empty())
                    lastClickedId = std::to_string(i) + "_" + lastClickedId;
            }
        }
        renderDepth--;
    }
}

void UIManager::renderText(const UIElement& el, float x, float y, const UIElementData& data)
{
    const std::string& text = data.text.empty() ? el.staticText : data.text;
    if (text.empty()) return;
    const Color col = applyGA(data.colorProvided ? data.color : el.staticTextColor);
    tw->writeTextAtlasToFB((int)(x + el.x), (int)y, text.c_str(), col, el.fontScale, currentFB);
}

void UIManager::renderImage(const UIElement& el, float x, float y, const UIElementData& data)
{
    const std::string& path = data.path.empty() ? el.staticImagePath : data.path;
    if (path.empty()) return;
    int ix = (int)(x + el.x), iy = (int)y;
    int iw = (int)(el.w * el.imageScale), ih = (int)(el.h * el.imageScale);
    render->DrawImage2DToFB(path, ix, iy, iw, ih, currentFB);
    if (el.borderWidth > 0.0f) {
        int bw = (int)el.borderWidth;
        Color bc = applyGA(el.borderColor);
        render->DrawFilledRectToFB(ix,        iy,        iw, bw, bc, currentFB);
        render->DrawFilledRectToFB(ix,        iy+ih-bw,  iw, bw, bc, currentFB);
        render->DrawFilledRectToFB(ix,        iy,        bw, ih, bc, currentFB);
        render->DrawFilledRectToFB(ix+iw-bw,  iy,        bw, ih, bc, currentFB);
    }
    const std::string& tt = !data.tooltip.empty() ? data.tooltip : el.tooltip;
    if (!tt.empty() && input) {
        int mx = input->getRawMouseX(), my = input->getRawMouseY();
        if (mx >= ix && mx <= ix+iw && my >= iy && my <= iy+ih)
            pendingTooltip = { tt, mx, my, true, "", {}, currentFB };
    }
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

    Color c = data.colorProvided ? data.color : el.rectColor;
    c.a *= el.alpha;
    render->DrawFilledRectToFB((int)rx, (int)ry, (int)w, (int)h, applyGA(c), currentFB);
    if (el.borderWidth > 0.0f) {
        int bord = (int)el.borderWidth;
        Color bc = applyGA(el.borderColor);
        render->DrawFilledRectToFB((int)rx,         (int)ry,          (int)w, bord,   bc, currentFB);
        render->DrawFilledRectToFB((int)rx,         (int)(ry+h-bord), (int)w, bord,   bc, currentFB);
        render->DrawFilledRectToFB((int)rx,         (int)ry,          bord,   (int)h, bc, currentFB);
        render->DrawFilledRectToFB((int)(rx+w-bord),(int)ry,          bord,   (int)h, bc, currentFB);
    }
}

void UIManager::renderProgressBar(const UIElement& el, float x, float y, const UIElementData& data)
{
    float maxV = data.maxValue > 0 ? data.maxValue : 1.0f;
    float ratio = data.value / maxV;
    float bw = el.barW;
    float bh = el.barH;

    if (!data.provided) return;

    if (textPass) {
        std::string label = std::to_string((int)data.value) + "/" + std::to_string((int)data.maxValue);
        const Color col = applyGA(data.colorProvided ? data.color : el.staticTextColor);
        tw->writeTextAtlasToFB((int)(x + el.x + el.textOffsetX), (int)(y + el.textOffsetY),
                               label.c_str(), col, el.barFontScale, currentFB);
        return;
    }

    render->DrawFilledRectToFB((int)(x + el.x), (int)y, (int)bw, (int)bh, applyGA(el.barBg), currentFB);

    Color fillColor = el.barOk;
    if (ratio < 0.33f) fillColor = el.barLow;
    else if (ratio < 0.66f) fillColor = el.barMid;

    int fillW = (int)(bw * ratio);
    if (fillW > 0)
        render->DrawFilledRectToFB((int)(x + el.x), (int)y, fillW, (int)bh, applyGA(fillColor), currentFB);
}

void UIManager::renderIcons(const UIElement& el, float x, float y, const UIElementData& data)
{
    float cx = x + el.x;
    for (auto& key : data.iconList) {
        auto mapIt = el.iconMapping.find(key);
        if (mapIt == el.iconMapping.end()) continue;
        render->DrawImage2DToFB(mapIt->second, (int)cx, (int)y,
                                (int)el.iconSize, (int)el.iconSize, currentFB);
        cx += el.iconSize + el.iconGap;
    }
}

void UIManager::renderButton(const UIElement& el, float x, float y, const UIElementData& data)
{
    if (!data.provided && el.staticText.empty() && el.staticImagePath.empty() && el.options.empty()) return;

    float bx = x + el.x;
    float by = y;
    float bw = el.w > 0 ? el.w : 80.0f;
    float bh = el.h > 0 ? el.h : 20.0f;

    int mx = input ? input->getRawMouseX() : -1;
    int my = input ? input->getRawMouseY() : -1;
    bool hovered = mx >= (int)bx && mx <= (int)(bx + bw) &&
                   my >= (int)by && my <= (int)(by + bh);
    bool pressed = hovered && input && input->isLeftMouseButtonPressed();
    bool clicked = hovered && input && input->isMouseButtonUp();

    // Resolve active option (by key, or options[0] as editor/fallback preview)
    const UIElementOption* activeOpt = nullptr;
    if (!el.options.empty()) {
        if (!data.key.empty()) {
            for (auto& o : el.options)
                if (o.key == data.key) { activeOpt = &o; break; }
        }
        if (!activeOpt) activeOpt = &el.options[0];
    }

    const std::string& resolvedText = (activeOpt && !activeOpt->text.empty()) ? activeOpt->text
        : (!data.provided || data.text.empty()) ? el.staticText : data.text;
    const std::string activeImage = activeOpt ? activeOpt->image : "";
    const std::string& resolvedPathOn  = !activeImage.empty() ? activeImage
        : (!data.provided || data.path.empty()) ? el.staticImagePath : data.path;
    const std::string& resolvedPathOff = (!data.provided || data.pathOff.empty()) ? el.staticImagePathOff : data.pathOff;
    const bool hasDualImage = !resolvedPathOn.empty() && !resolvedPathOff.empty();
    const std::string& resolvedPath = hasDualImage ? (data.enabled ? resolvedPathOn : resolvedPathOff) : resolvedPathOn;

    float imgSize = 0.0f;
    if (!resolvedPath.empty()) {
        imgSize = el.iconSize > 0 ? el.iconSize : bh - 4.0f;
        if (imgSize < 1.0f) imgSize = 1.0f;
    }
    float imgPad = imgSize > 0 ? (bh - imgSize) * 0.5f : 0.0f;

    if (textPass) {
        if (!resolvedText.empty()) {
            float scale = el.fontScale > 0 ? el.fontScale : 0.45f;
            // Measure using actual glyph metrics for accurate centering
            float textW = 0.0f, textH = 0.0f;
            if (tw && tw->getGlyphAtlas() && tw->getGlyphAtlas()->isBuilt()) {
                const GlyphAtlas* ga = tw->getGlyphAtlas();
                float cx = 0.0f;
                for (char c : resolvedText) {
                    const GlyphInfo& g = ga->getGlyphInfo(c);
                    cx += (g.valid ? g.advance : ga->getGlyphInfo(' ').advance) * scale;
                }
                textW = cx;
                textH = (float)ga->getLineHeight() * scale;
            } else {
                textW = scale * (float)resolvedText.size() * 7.5f;
                textH = scale * 20.0f;
            }
            int tx, ty;
            if (imgSize > 0) {
                float availX = bx + imgPad + imgSize + 2.0f;
                float availW = bw - (imgPad + imgSize + 2.0f);
                tx = (int)(availX + (availW - textW) * 0.5f);
            } else {
                tx = (int)(bx + (bw - textW) * 0.5f);
            }
            ty = (int)(by + (bh - textH) * 0.5f);
            tx += (int)el.btnTextOffsetX;
            ty += (int)el.btnTextOffsetY;
            const Color col = applyGA((activeOpt && activeOpt->colorSet) ? activeOpt->color
                : data.colorProvided ? data.color : el.staticTextColor);
            tw->writeTextAtlasToFB(tx, ty, resolvedText.c_str(), col, scale, currentFB);
        }
        return;
    }

    Color bg = applyGA(pressed ? el.btnPressed : hovered ? el.btnHover : el.btnBg);
    render->DrawFilledRectToFB((int)bx, (int)by, (int)bw, (int)bh, bg, currentFB);

    if (el.borderWidth > 0.0f) {
        int bord = (int)el.borderWidth;
        Color bc = applyGA(el.borderColor);
        render->DrawFilledRectToFB((int)bx,       (int)by,       (int)bw, bord,   bc, currentFB);
        render->DrawFilledRectToFB((int)bx,       (int)(by+bh-bord), (int)bw, bord, bc, currentFB);
        render->DrawFilledRectToFB((int)bx,       (int)by,       bord, (int)bh,   bc, currentFB);
        render->DrawFilledRectToFB((int)(bx+bw-bord), (int)by,   bord, (int)bh,   bc, currentFB);
    }

    if (imgSize > 0) {
        float imgX = resolvedText.empty() ? bx + (bw - imgSize) * 0.5f : bx + imgPad;
        render->DrawImage2DToFB(resolvedPath,
            (int)(imgX            + el.btnImgOffsetX),
            (int)(by + imgPad + el.btnImgOffsetY),
            (int)imgSize, (int)imgSize, currentFB);
    }

    if (clicked && lastClickedId.empty()) {
        lastClickedId = el.id;
        if (input) input->consumeLeftClick();
    }

    if (hovered) {
        const std::string& tt = !data.tooltip.empty() ? data.tooltip
            : (activeOpt && !activeOpt->tooltip.empty()) ? activeOpt->tooltip
            : el.tooltip;
        const std::string& wname = activeOpt ? activeOpt->tooltipWidget : "";
        if (!tt.empty() || !wname.empty()) {
            UIWidgetRenderData wdata;
            if (!wname.empty() && activeOpt) {
                if (!activeOpt->image.empty())   { UIElementData d; d.path = activeOpt->image;   d.provided = true; wdata["icon"]  = d; }
                if (!activeOpt->text.empty())    { UIElementData d; d.text = activeOpt->text;    d.provided = true; wdata["title"] = d; }
                if (!activeOpt->tooltip.empty()) { UIElementData d; d.text = activeOpt->tooltip; d.provided = true; wdata["desc"]  = d; }
            }
            pendingTooltip = { tt, mx, my, true, wname, wdata, currentFB };
        }
    }
}

// ---------------------------------------------------------------------------
// Tooltip flush — called once per top-level drawWidget / drawWidgetAtPos

void UIManager::flushTooltip(float dt)
{
    if (!pendingTooltip.active) {
        hoverKey     = "";
        hoverElapsed = 0.0f;
        return;
    }
    if (pendingTooltip.text.empty() && pendingTooltip.widgetName.empty()) {
        pendingTooltip.active = false;
        hoverKey     = "";
        hoverElapsed = 0.0f;
        return;
    }
    pendingTooltip.active = false;

    // Hover tracking — reset timer when the tooltip target changes
    std::string key = pendingTooltip.text + "|" + pendingTooltip.widgetName;
    if (key != hoverKey) {
        hoverKey     = key;
        hoverElapsed = 0.0f;
    }
    hoverElapsed += dt;

    if (hoverElapsed < TOOLTIP_DELAY) return;

    const float fadeT = std::min(1.0f, (hoverElapsed - TOOLTIP_DELAY) / TOOLTIP_FADE_IN);
    globalAlpha = fadeT;

    auto* win = Components::get()->Window();

    // Rich widget tooltip
    if (!pendingTooltip.widgetName.empty()) {
        auto wit = widgets.find(pendingTooltip.widgetName);
        if (wit != widgets.end()) {
            UIWidget& tw_w = wit->second;
            float s  = tw_w.scale > 0 ? tw_w.scale : 1.0f;
            float ww = (float)win->getWidth()  * tw_w.width  * s;
            float wh = (float)win->getHeight() * tw_w.height * s;
            float bx = (float)pendingTooltip.x + 14.0f;
            float by = (float)pendingTooltip.y - wh - 6.0f;
            if (bx + ww > (float)win->getWidth()) bx = (float)win->getWidth() - ww - 4.0f;
            if (by < 0)                           by = (float)pendingTooltip.y + 16.0f;
            currentFB = pendingTooltip.fb;
            renderWidgetAt(tw_w, bx, by, pendingTooltip.widgetData, true);
            globalAlpha = 1.0f;
            return;
        }
    }

    // Plain text tooltip fallback
    if (pendingTooltip.text.empty()) { globalAlpha = 1.0f; return; }

    constexpr float scale  = 0.45f;
    constexpr float charW  = scale * 13.0f;
    constexpr float textH  = scale * 22.0f;
    constexpr float pad    = 8.0f;

    float boxW = charW * (float)pendingTooltip.text.size() + pad * 2.0f;
    float boxH = textH + pad * 2.0f;

    float bx = (float)pendingTooltip.x + 14.0f;
    float by = (float)pendingTooltip.y - boxH - 6.0f;
    if (bx + boxW > (float)win->getWidth())  bx = (float)win->getWidth()  - boxW - 2.0f;
    if (by < 0)                               by = (float)pendingTooltip.y + 16.0f;

    const Color bg     {0.06f, 0.06f, 0.09f, 0.96f};
    const Color border {0.52f, 0.55f, 0.65f, 1.00f};
    const Color text   {0.92f, 0.92f, 0.95f, 1.00f};

    const std::string& tfb = pendingTooltip.fb;
    render->DrawFilledRectToFB((int)bx, (int)by, (int)boxW, (int)boxH, applyGA(bg), tfb);
    render->DrawFilledRectToFB((int)bx,            (int)by,            (int)boxW, 1, applyGA(border), tfb);
    render->DrawFilledRectToFB((int)bx,            (int)(by+boxH-1),   (int)boxW, 1, applyGA(border), tfb);
    render->DrawFilledRectToFB((int)bx,            (int)by,            1, (int)boxH, applyGA(border), tfb);
    render->DrawFilledRectToFB((int)(bx+boxW-1),   (int)by,            1, (int)boxH, applyGA(border), tfb);
    tw->writeTextAtlasToFB((int)(bx + pad), (int)(by + pad), pendingTooltip.text.c_str(), applyGA(text), scale, tfb);

    globalAlpha = 1.0f;
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
    if (colorObj.valid()) { d.color = colorObj.as<Color>(); d.colorProvided = true; }

    sol::object pathObj = t["path"];
    if (pathObj.valid() && pathObj.get_type() == sol::type::string)
        d.path = pathObj.as<std::string>();

    sol::object pathOffObj = t["pathOff"];
    if (pathOffObj.valid() && pathOffObj.get_type() == sol::type::string)
        d.pathOff = pathOffObj.as<std::string>();

    sol::object enabledObj = t["enabled"];
    if (enabledObj.valid() && enabledObj.get_type() == sol::type::boolean)
        d.enabled = enabledObj.as<bool>();

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

    sol::object countObj = t["count"];
    if (countObj.valid()) d.count = countObj.as<int>();

    sol::object keyObj = t["key"];
    if (keyObj.valid() && keyObj.get_type() == sol::type::string)
        d.key = keyObj.as<std::string>();

    sol::object tooltipObj = t["tooltip"];
    if (tooltipObj.valid() && tooltipObj.get_type() == sol::type::string)
        d.tooltip = tooltipObj.as<std::string>();

    return d;
}

static UIWidgetRenderData solTableToRenderData(sol::table data)
{
    UIWidgetRenderData renderData;
    data.for_each([&](sol::object key, sol::object val) {
        if (key.get_type() == sol::type::string && val.get_type() == sol::type::table)
            renderData[key.as<std::string>()] = UIManager::solTableToElementData(val.as<sol::table>());
    });
    return renderData;
}

std::tuple<float, std::string> UIManager::drawWidgetLua(const std::string& name, sol::table data, const std::string& fb, float scaleOverride)
{
    auto it = widgets.find(name);
    if (it == widgets.end()) {
        printf("[UIManager] WARN: widget '%s' not found\n", name.c_str());
        return {0.0f, ""};
    }

    float nextY = drawWidget(name, solTableToRenderData(data), fb, scaleOverride);
    return {nextY, lastClickedId};
}

std::tuple<float, std::string> UIManager::drawWidgetAtPosLua(const std::string& name, float x, float y, sol::table data, const std::string& fb, float scaleOverride)
{
    auto it = widgets.find(name);
    if (it == widgets.end()) {
        printf("[UIManager] WARN: widget '%s' not found\n", name.c_str());
        return {y, ""};
    }

    float nextY = drawWidgetAtPos(name, x, y, solTableToRenderData(data), fb, scaleOverride);
    return {nextY, lastClickedId};
}
