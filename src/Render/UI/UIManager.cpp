#include <filesystem>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <cmath>
#include <cstdint>
#include <SDL2/SDL.h>

#include "../../../include/Render/UI/UIManager.h"
#include "../../../include/Misc/Logging.h"
#include "../../../include/Render/Image.h"
#include "../../../include/Render/Profiler.h"
#include "../../../include/Components/ComponentRender.h"
#include "../../../include/Components/ComponentInput.h"
#include "../../../include/Components/ComponentSound.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Misc/cJSON.h"
#include "../../../include/Misc/ToolsJSON.h"
#include "../../../include/Render/TextWriter.h"

void UIManager::init(ComponentRender* r, const std::string& dir)
{
    render = r;
    input  = Components::get()->Input();
    tw     = r->getTextWriter();
    defaultTw = tw;
    widgetsDir = dir;
    loadWidgets();
}

// Writer for a widget's "font". One TextWriter (+ its own glyph atlas) per font file, created on
// first use and kept for the whole process: never destroyed mid-frame, so no atlas in use can go
// away. Widget text is drawn immediately (writeTextAtlasToFB), not batched, so switching writers
// between widgets can't mix atlases. Unknown/unloadable font → engine default, logged once.
TextWriter* UIManager::writerForFont(const std::string& fontPath)
{
    if (fontPath.empty()) return defaultTw;
    auto it = fontWriters.find(fontPath);
    if (it != fontWriters.end()) return it->second ? it->second : defaultTw;

    TextWriter* writer = nullptr;
    TTF_Font* font = std::filesystem::exists(fontPath) ? TTF_OpenFont(fontPath.c_str(), 35) : nullptr;
    if (font) {
        writer = new TextWriter(Components::get()->Window()->getRenderer(), font);
        writer->buildGlyphAtlas(512);
        LOG_MESSAGE("[UIManager] Font loaded for widgets: '%s'", fontPath.c_str());
    } else {
        LOG_MESSAGE("[UIManager] Cannot open widget font '%s' -- using default", fontPath.c_str());
    }
    fontWriters[fontPath] = writer;   // nullptr cached too: don't retry every frame
    return writer ? writer : defaultTw;
}

static void freeWidgetCache(UIWidget& w)
{
    if (w.cacheFBO)     { glDeleteFramebuffers(1, &w.cacheFBO);  w.cacheFBO = 0; }
    if (w.cacheTexture) { glDeleteTextures(1, &w.cacheTexture);  w.cacheTexture = 0; }
    w.cacheRW = 0;  w.cacheRH = 0;  w.cacheDirty = true;
    w.cachedData.clear();
}

void UIManager::loadWidgets()
{
    for (auto& [n, w] : widgets) freeWidgetCache(w);
    widgets.clear();
    loadedDirs.clear();
    loadWidgetsFromDir(widgetsDir);
}

void UIManager::reloadWidgets()
{
    for (auto& [n, w] : widgets) freeWidgetCache(w);
    widgets.clear();
    auto dirs = loadedDirs;   // snapshot — loadWidgetsFromDir modifica loadedDirs
    loadedDirs.clear();
    for (auto& dir : dirs)
        loadWidgetsFromDir(dir);
    printf("[UIManager] reloaded\n");
}

void UIManager::clearWidgets()
{
    for (auto& [n, w] : widgets) freeWidgetCache(w);
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

bool UIManager::invalidateWidget(const std::string& name)
{
    auto it = widgets.find(name);
    if (it == widgets.end()) return false;
    it->second.cacheDirty = true;
    return true;
}

void UIManager::setDesignResolution(float w, float h)
{
    designW = std::max(0.0f, w);
    designH = std::max(0.0f, h);
    invalidateAllWidgets();   // cacheable widgets hold text rendered at the old size
}

float UIManager::getDesignTextScale() const
{
    if (designW <= 0.0f || designH <= 0.0f) return 1.0f;
    auto* win = Components::get()->Window();
    if (!win || win->getWidth() <= 0 || win->getHeight() <= 0) return 1.0f;
    return std::min((float)win->getWidth() / designW, (float)win->getHeight() / designH);
}

void UIManager::invalidateAllWidgets()
{
    for (auto& [_, w] : widgets) w.cacheDirty = true;
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
    cJSON* offsetXItem = cJSON_GetObjectItem(root, "offsetX");
    if (offsetXItem) widget.offsetX = (float)offsetXItem->valuedouble;
    cJSON* offsetYItem = cJSON_GetObjectItem(root, "offsetY");
    if (offsetYItem) widget.offsetY = (float)offsetYItem->valuedouble;
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
    cJSON* bgImageItem = cJSON_GetObjectItem(root, "bgImage");
    if (bgImageItem && bgImageItem->valuestring) widget.bgImage = bgImageItem->valuestring;
    // "bgSlice": [L, T, R, B] (image px) or a single number for all four sides
    cJSON* bgSliceItem = cJSON_GetObjectItem(root, "bgSlice");
    if (bgSliceItem && cJSON_IsArray(bgSliceItem)) {
        for (int k = 0; k < 4 && k < cJSON_GetArraySize(bgSliceItem); k++)
            widget.bgSlice[k] = (float)cJSON_GetArrayItem(bgSliceItem, k)->valuedouble;
    } else if (bgSliceItem && cJSON_IsNumber(bgSliceItem)) {
        for (float& v : widget.bgSlice) v = (float)bgSliceItem->valuedouble;
    }
    cJSON* bgSliceScaleItem = cJSON_GetObjectItem(root, "bgSliceScale");
    if (bgSliceScaleItem) widget.bgSliceScale = (float)bgSliceScaleItem->valuedouble;
    cJSON* bgImageAlphaItem = cJSON_GetObjectItem(root, "bgImageAlpha");
    if (bgImageAlphaItem) widget.bgImageAlpha = std::clamp((float)bgImageAlphaItem->valuedouble, 0.0f, 1.0f);
    cJSON* fontItem = cJSON_GetObjectItem(root, "font");
    if (fontItem && fontItem->valuestring) widget.font = fontItem->valuestring;
    cJSON* refHeightItem = cJSON_GetObjectItem(root, "refHeight");
    if (refHeightItem) widget.refHeight = std::max(0.0f, (float)refHeightItem->valuedouble);
    cJSON* bwItem = cJSON_GetObjectItem(root, "borderWidth");
    if (bwItem) widget.borderWidth = (float)bwItem->valuedouble;
    cJSON* wCursorItem = cJSON_GetObjectItem(root, "cursor");
    if (wCursorItem && wCursorItem->valuestring) widget.cursor = wCursorItem->valuestring;
    cJSON* cacheableItem = cJSON_GetObjectItem(root, "cacheable");
    if (cacheableItem && cJSON_IsBool(cacheableItem)) widget.cacheable = cJSON_IsTrue(cacheableItem);

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
            cJSON* elFont = cJSON_GetObjectItem(item, "font"); if (elFont && elFont->valuestring) el.font = elFont->valuestring;
            cJSON* textCol = cJSON_GetObjectItem(item, "textColor"); if (textCol) el.staticTextColor = ToolsJSON::getColorByJSON(textCol);
            cJSON* staticText = cJSON_GetObjectItem(item, "text"); if (staticText && staticText->valuestring) el.staticText = staticText->valuestring;
            cJSON* txtAlign = cJSON_GetObjectItem(item, "textAlign"); if (txtAlign && txtAlign->valuestring) el.textAlign = txtAlign->valuestring;
            cJSON* cached = cJSON_GetObjectItem(item, "cached"); if (cached && cJSON_IsBool(cached)) el.cached = cJSON_IsTrue(cached);
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
            cJSON* arrayPaginate = cJSON_GetObjectItem(item, "arrayPaginate");
            if (arrayPaginate) el.arrayPaginate = (arrayPaginate->type == cJSON_True);
            cJSON* arrayPagerWidget = cJSON_GetObjectItem(item, "arrayPagerWidget");
            if (arrayPagerWidget && arrayPagerWidget->valuestring) el.arrayPagerWidget = arrayPagerWidget->valuestring;
            cJSON* arrayPagerTop = cJSON_GetObjectItem(item, "arrayPagerTop");
            if (arrayPagerTop) el.arrayPagerTop = (arrayPagerTop->type == cJSON_True);
            cJSON* arrayOffset = cJSON_GetObjectItem(item, "arrayOffset");
            if (arrayOffset) el.arrayOffset = (float)arrayOffset->valuedouble;
            cJSON* arrayPrefix = cJSON_GetObjectItem(item, "arrayPrefix");
            if (arrayPrefix && arrayPrefix->valuestring) el.arrayPrefix = arrayPrefix->valuestring;
            cJSON* borderCol = cJSON_GetObjectItem(item, "borderColor");
            if (borderCol) el.borderColor = ToolsJSON::getColorByJSON(borderCol);
            cJSON* borderColHov = cJSON_GetObjectItem(item, "borderColorHover");
            if (borderColHov) el.borderColorHover = ToolsJSON::getColorByJSON(borderColHov);
            else if (borderCol) el.borderColorHover = el.borderColor;
            cJSON* borderColPrs = cJSON_GetObjectItem(item, "borderColorPressed");
            if (borderColPrs) el.borderColorPressed = ToolsJSON::getColorByJSON(borderColPrs);
            else if (borderCol) el.borderColorPressed = el.borderColor;
            cJSON* borderW = cJSON_GetObjectItem(item, "borderWidth");
            if (borderW) el.borderWidth = (float)borderW->valuedouble;
            cJSON* bgColItem = cJSON_GetObjectItem(item, "bgColor");
            if (bgColItem) el.bgColor = ToolsJSON::getColorByJSON(bgColItem);
            cJSON* tooltipItem = cJSON_GetObjectItem(item, "tooltip");
            if (tooltipItem && tooltipItem->valuestring) el.tooltip = tooltipItem->valuestring;
            cJSON* soundItem = cJSON_GetObjectItem(item, "sound");
            if (soundItem && soundItem->valuestring) el.sound = soundItem->valuestring;
            cJSON* cursorItem = cJSON_GetObjectItem(item, "cursor");
            if (cursorItem && cursorItem->valuestring) el.cursor = cursorItem->valuestring;
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
            cJSON* enabledItem = cJSON_GetObjectItem(item, "enabled");
            if (enabledItem && cJSON_IsBool(enabledItem)) el.enabled = cJSON_IsTrue(enabledItem);
            cJSON* wrapItem = cJSON_GetObjectItem(item, "wrap");
            if (wrapItem && cJSON_IsBool(wrapItem)) el.wrap = cJSON_IsTrue(wrapItem);
            cJSON* lineSp = cJSON_GetObjectItem(item, "lineSpacing"); if (lineSp) el.lineSpacing = (float)lineSp->valuedouble;
            cJSON* maxL   = cJSON_GetObjectItem(item, "maxLines");    if (maxL)   el.maxLines    = maxL->valueint;
            cJSON* scStep = cJSON_GetObjectItem(item, "scrollStep");  if (scStep) el.scrollStep  = (float)scStep->valuedouble;
            cJSON* scBar  = cJSON_GetObjectItem(item, "scrollbar");
            if (scBar && cJSON_IsBool(scBar)) el.scrollbar = cJSON_IsTrue(scBar);
            cJSON* scBarC = cJSON_GetObjectItem(item, "scrollbarColor");
            if (scBarC) el.scrollbarColor = ToolsJSON::getColorByJSON(scBarC);
            // Scroll content moves with the wheel every frame: a widget holding one can't be FBO-cached.
            if (el.type == "scroll") widget.cacheable = false;
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
        if ((el.type == "widget" || el.type == "array" || el.type == "scroll") && !el.widgetRef.empty()) {
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
    if (el.type == "scroll")      return el.h;
    // "widget"/"array": measured in pixels by renderWidgetAt's getHeightDynamic (needs the render
    // scale and the data's item count, which this static helper doesn't have).
    return 0;
}

// ---------------------------------------------------------------------------
// Core renderer — takes plain C++ data, no Lua

float UIManager::renderWidgetAt(UIWidget& w, float x, float y, const UIWidgetRenderData& data, bool useDefaultData)
{
    if (widgetRenderDepth == 0) { lastClickedId = ""; lastRightClickedId = ""; lastHoveredId = ""; hoveredCursorName = ""; }
    widgetRenderDepth++;
    // Pixel offset applied here (not in drawWidget/drawWidgetAtPos) so it also works when the
    // widget is rendered nested (widget/array elements, scroll, tooltip, pager).
    x += w.offsetX;
    y += w.offsetY;
    static const UIElementData defaultData;
    const UIWidgetRenderData* prevData = currentData;
    currentData = &data;
    const UIWidget* prevWidget = currentWidget;
    currentWidget = &w;
    bool prevTextPass = textPass;
    // Widget font: switch writer for this widget (and its nested widgets, unless they set their
    // own); restored at the single exit point below.
    TextWriter* prevTw = tw;
    if (!w.font.empty()) tw = writerForFont(w.font);

    float prevRenderScale = currentRenderScale;
    float ownScale = w.scale > 0 ? w.scale : 1.0f;
    float s = ownScale * prevRenderScale;   // accumulated: parent scale × own scale
    currentRenderScale = s;

    // x/y/w/h on elements are 0..1 fractions of the widget box (cw × ch in screen px)
    auto* winPtr = Components::get()->Window();
    const float cw = (float)winPtr->getWidth()  * (w.width  > 0 ? w.width  : 1.0f);
    const float ch = (float)winPtr->getHeight() * (w.height > 0 ? w.height : 1.0f);

    // Text size: at top level, the design-resolution factor (1 = fixed size when no design
    // resolution is set; see setDesignResolution), inherited by nested widgets -- unless a widget
    // sets refHeight, then it follows the window height. The boxes scale with the window on each
    // axis (cw/ch); before the design factor, text kept its pixel size and overflowed its box when
    // the window shrank.
    const float prevTextScale = currentTextScale;
    if (widgetRenderDepth == 1) currentTextScale = getDesignTextScale();
    if (w.refHeight > 0.0f) currentTextScale = (float)winPtr->getHeight() / w.refHeight;
    const float ts = currentTextScale;

    auto buildScaled = [s, ts, cw, ch](const UIElement& el) {
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
        sc.fontScale      = el.fontScale      * s * ts;
        sc.barFontScale   = el.barFontScale   * s * ts;
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

    // Height of one element in window px, accounting for dynamic array count override from data.
    // Nested widgets/arrays are measured exactly as renderElement places them. (Before 2026-09-27
    // these two branches returned widget-FRACTION values -- e.g. 0.019 instead of ~19 px -- so a yAuto
    // element after a nested widget/array barely moved down.)
    auto getHeightDynamic = [&](const UIElement& sc, const UIElementData& elData) -> float {
        if (sc.type == "widget") {
            auto it2 = widgets.find(sc.widgetRef);
            if (it2 == widgets.end()) return 0.0f;
            float ss = it2->second.scale > 0 ? it2->second.scale : 1.0f;
            // child box: renderWidgetAt uses window height × its height × accumulated scale
            return (float)winPtr->getHeight() * it2->second.height * ss * s;
        }
        if (sc.type == "array") {
            auto it2 = widgets.find(sc.widgetRef);
            if (it2 != widgets.end()) {
                float ss = it2->second.scale > 0 ? it2->second.scale : 1.0f;
                int totalIt  = (elData.count > 0) ? elData.count : 0;
                int pg       = (elData.page >= 0) ? elData.page : 0;
                if (sc.arrayPaginate && totalIt > 0 && sc.arrayCount > 0)   // mismo límite que renderElement
                    pg = std::min(pg, (totalIt + sc.arrayCount - 1) / sc.arrayCount - 1);
                int pgOffset = sc.arrayPaginate ? (pg * sc.arrayCount) : 0;
                int onPage   = (sc.arrayPaginate && totalIt > 0) ? std::min(sc.arrayCount, std::max(0, totalIt - pgOffset)) : 0;
                int cnt      = sc.arrayPaginate ? onPage : ((elData.count > 0) ? elData.count : sc.arrayCount);
                int totPages = (sc.arrayPaginate && totalIt > 0) ? ((totalIt + sc.arrayCount - 1) / sc.arrayCount) : -1;
                // Same step as renderElement's arrayStep (window height × child height × scales + offset)
                const float childH = (float)winPtr->getHeight() * it2->second.height * ss * s;
                if (sc.arrayAlign == "horizontal") return childH;
                float step = childH + sc.arrayOffset;
                float navH = (sc.arrayPaginate && totPages > 1) ? childH : 0.0f;
                return step * (float)cnt + navH;
            }
        }
        // Wrapped text: as tall as its lines (the text comes from data, so it can't be static)
        if (sc.type == "text" && sc.wrap && sc.w > 0.0f) {
            const std::string& txt = elData.text.empty() ? sc.staticText : elData.text;
            if (txt.empty()) return 0.0f;
            auto lines = wrapText(txt, sc.w, sc.fontScale, sc.maxLines);
            return (float)lines.size() * textLineHeight(sc.fontScale) * sc.lineSpacing;
        }
        return getElementHeight(sc);
    };

    float contentBottom = y;   // lowest element edge seen in pass 1 → lastContentH (scroll viewports)

    float dynamicH = ch * s;
    float dynamicW = cw * s;

    // Widget-level cursor: checked every frame (not cached — it's a side effect, not rendering).
    if (!w.cursor.empty() && !textPass) {
        int mx = input ? input->getRawMouseX() : -1;
        int my = input ? input->getRawMouseY() : -1;
        if (mx >= (int)x && mx <= (int)(x + dynamicW) && my >= (int)y && my <= (int)(y + dynamicH)
            && pointerInClip(mx, my))
            hoveredCursorName = w.cursor;
    }

    // Lambda encapsulating all draw operations for this widget (background + elements + border).
    // Called either directly (non-cacheable) or inside the dirty-render path (cacheable).
    auto renderContent = [&]() {
        if (!hitTestOnly && w.bgColor.a > 0.0f)
            render->DrawFilledRectToFB((int)x, (int)y, (int)dynamicW, (int)dynamicH, applyGA(w.bgColor), currentFB);
        // Optional background image: stretched over the widget box (its declared width × height --
        // dynamicH is the declared height, it does NOT grow with content), under every element.
        // Same helper/alpha as "image" elements.
        if (!hitTestOnly && !w.bgImage.empty() && w.bgImageAlpha > 0.0f) {
            const float bgAlpha = globalAlpha * w.bgImageAlpha;
            const bool nineSlice = w.bgSlice[0] > 0.0f || w.bgSlice[1] > 0.0f || w.bgSlice[2] > 0.0f || w.bgSlice[3] > 0.0f;
            if (nineSlice)   // frame mode: corners keep their size (scaled with the widget), edges/center stretch
                render->DrawImage2DNineSliceToFB(getCachedImage(w.bgImage), x, y, dynamicW, dynamicH,
                                                 w.bgSlice, w.bgSliceScale * s, currentFB, bgAlpha);
            else
                drawImageCached(w.bgImage, (int)x, (int)y, (int)dynamicW, (int)dynamicH, bgAlpha);
        }

        // Pass 1: rects, images, icons (no text) — also runs hit-test
        textPass = false;
        float autoY = y;
        for (auto& el : w.elements) {
            UIElement scaled = buildScaled(el);
            if (el.yAuto) autoY += scaled.paddingTop;
            float drawY = el.yAuto ? autoY : y + scaled.y + scaled.paddingTop;
            renderElement(scaled, x, drawY, resolveData(el));
            if (el.enabled) contentBottom = std::max(contentBottom, drawY + getHeightDynamic(scaled, resolveData(el)) + scaled.paddingBottom);
            if (el.yAuto) autoY += getHeightDynamic(scaled, resolveData(el)) + scaled.paddingBottom;
        }

        // Pass 2: text only — skipped entirely in hit-test-only mode
        if (!hitTestOnly) {
            textPass = true;
            autoY = y;
            for (auto& el : w.elements) {
                UIElement scaled = buildScaled(el);
                if (el.yAuto) autoY += scaled.paddingTop;
                float drawY = el.yAuto ? autoY : y + scaled.y + scaled.paddingTop;
                renderElement(scaled, x, drawY, resolveData(el));
                if (el.yAuto) autoY += getHeightDynamic(scaled, resolveData(el)) + scaled.paddingBottom;
            }
        }

        if (!hitTestOnly && w.borderWidth > 0.0f) {
            int bw = (int)w.borderWidth;
            Color bc = applyGA(w.borderColor);
            int ix = (int)x, iy = (int)y, iw = (int)dynamicW, ih = (int)dynamicH;
            render->DrawFilledRectToFB(ix,       iy,       iw, bw, bc, currentFB);
            render->DrawFilledRectToFB(ix,       iy+ih-bw, iw, bw, bc, currentFB);
            render->DrawFilledRectToFB(ix,       iy,       bw, ih, bc, currentFB);
            render->DrawFilledRectToFB(ix+iw-bw, iy,       bw, ih, bc, currentFB);
        }

        if (!hitTestOnly && !debugHighlightName.empty()) {
            auto hit = widgets.find(debugHighlightName);
            if (hit != widgets.end() && &hit->second == &w) {
                int ix = (int)x, iy = (int)y, iw = (int)dynamicW, ih = (int)dynamicH;
                render->DrawFilledRectToFB(ix, iy, iw, ih, Color(1.0f, 0.5f, 0.0f, 0.30f), currentFB);
                constexpr int hbw = 2;
                Color hbc(1.0f, 0.75f, 0.0f, 0.95f);
                render->DrawFilledRectToFB(ix,        iy,        iw,  hbw, hbc, currentFB);
                render->DrawFilledRectToFB(ix,        iy+ih-hbw, iw,  hbw, hbc, currentFB);
                render->DrawFilledRectToFB(ix,        iy,        hbw, ih,  hbc, currentFB);
                render->DrawFilledRectToFB(ix+iw-hbw, iy,        hbw, ih,  hbc, currentFB);
            }
        }
    };

    // ── FBO Cache path (only at top-level depth, non-interactive widgets) ──────
    if (w.cacheable && widgetRenderDepth == 1) {
        // The cache matches its TARGET layer ("ui" = window sized, the rest = render sized), so the
        // blit is 1:1 and the draws inside map onto it (setFBOOverride with its size below)
        int rW, rH;
        ComponentRender::getTargetSize(currentFB, rW, rH);

        // Recreate FBO if it doesn't exist or render resolution changed
        if (w.cacheFBO == 0 || w.cacheRW != rW || w.cacheRH != rH) {
            if (w.cacheFBO)     { glDeleteFramebuffers(1, &w.cacheFBO);  w.cacheFBO = 0; }
            if (w.cacheTexture) { glDeleteTextures(1, &w.cacheTexture);  w.cacheTexture = 0; }
            glGenTextures(1, &w.cacheTexture);
            glBindTexture(GL_TEXTURE_2D, w.cacheTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, rW, rH, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glGenFramebuffers(1, &w.cacheFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, w.cacheFBO);
            Profiler::get()->incrementFboChanges();
            Components::get()->Render()->setLastFrameBufferUsed(w.cacheFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, w.cacheTexture, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            Profiler::get()->incrementFboChanges();
            Components::get()->Render()->setLastFrameBufferUsed(0);
            w.cacheRW = rW;  w.cacheRH = rH;
            w.cacheDirty = true;
        }

        // Layout (cw/ch) follows the WINDOW size, not the render size: a window resize with a fixed
        // render resolution must re-render the cache too.
        if (w.cacheWinW != winPtr->getWidth() || w.cacheWinH != winPtr->getHeight()) {
            w.cacheWinW = winPtr->getWidth();
            w.cacheWinH = winPtr->getHeight();
            w.cacheDirty = true;
        }

        if (w.cacheDirty || data != w.cachedData) {
            // Clear cache FBO (transparent)
            glBindFramebuffer(GL_FRAMEBUFFER, w.cacheFBO);
            Profiler::get()->incrementFboChanges();
            Components::get()->Render()->setLastFrameBufferUsed(w.cacheFBO);
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            Profiler::get()->incrementFboChanges();
            Components::get()->Render()->setLastFrameBufferUsed(0);

            // Redirect all draw calls to the cache FBO, render content, restore. The cache is rendered
            // OPAQUE (globalAlpha 1) and setWidgetAlpha is applied when blitting it below: before, a
            // widget faded in with setWidgetAlpha (npcDialogue with the letterbox bars) was cached
            // at its first, almost transparent alpha and stayed like that until its data changed.
            const float fadeAlpha = globalAlpha;
            globalAlpha = 1.0f;
            render->setFBOOverride(w.cacheFBO, rW, rH);
            renderContent();
            render->clearFBOOverride();
            globalAlpha = fadeAlpha;

            w.cachedData = data;
            w.cacheDirty = false;
        }

        // Always blit the cached texture to the current target FB
        render->DrawWidgetCacheToFB(w.cacheTexture, rW, rH, currentFB, globalAlpha);

        // Hit-test pass: no GPU draw calls, only mouse interaction detection
        hitTestOnly = true;
        renderContent();
        hitTestOnly = false;
    } else {
        renderContent();
    }

    textPass = prevTextPass;
    currentData = prevData;
    currentWidget = prevWidget;
    currentRenderScale = prevRenderScale;
    currentTextScale = prevTextScale;
    tw = prevTw;
    // Set AFTER nested calls (they overwrite it): the caller reads this widget's own value.
    lastContentH = std::max(0.0f, contentBottom - y);
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
    if (!el.enabled) return;
    if (data.hidden) return;   // ocultado por datos este frame (p. ej. flecha desactivada del paginador)

    // Helper: compute step for array items.
    // - arrayDistribute=false (default): step = child natural size × scales (fixed, set at load time).
    // - arrayDistribute=true: step = full window dimension / arrayCount (computed at runtime).
    //   In distribute mode the child widget is still rendered at its own scale; only the spacing changes.
    bool isPaginated  = el.arrayPaginate;
    int  pageSize     = el.arrayCount;
    int  currentPage  = (isPaginated && data.page >= 0) ? data.page : 0;
    int  totalItems   = (data.count > 0) ? data.count : 0;
    int  totalPages   = (isPaginated && totalItems > 0) ? ((totalItems + pageSize - 1) / pageSize) : -1;
    // Página fuera de rango (Lua pasó de la última, o la lista encogió): se muestra la última en vez
    // de una página vacía (antes la lista "desaparecía" al pulsar > en la última página).
    if (totalPages > 0 && currentPage > totalPages - 1) currentPage = totalPages - 1;
    int  pageOffset   = isPaginated ? (currentPage * pageSize) : 0;
    int  itemsOnPage  = (isPaginated && totalItems > 0) ? std::min(pageSize, std::max(0, totalItems - pageOffset)) : 0;
    int  resolvedCount = isPaginated ? itemsOnPage : ((data.count > 0) ? data.count : pageSize);

    auto arrayStep = [&](UIWidget& child) -> std::pair<float,float> {
        float stepX = 0.0f, stepY = 0.0f;
        if (el.arrayDistribute) {
            auto* win = Components::get()->Window();
            if (el.arrayAlign == "horizontal")
                stepX = (float)win->getWidth() / (float)resolvedCount;
            else
                stepY = (float)win->getHeight() / (float)resolvedCount;
        } else {
            auto* win = Components::get()->Window();
            float ss = child.scale > 0 ? child.scale : 1.0f;
            if (el.arrayAlign == "horizontal")
                stepX = (float)win->getWidth()  * child.width  * ss * currentRenderScale + el.arrayOffset;
            else
                stepY = (float)win->getHeight() * child.height * ss * currentRenderScale + el.arrayOffset;
        }
        return {stepX, stepY};
    };

    // Pager bar of a paginated array: widget + the scale that makes it as wide as one item (cw).
    const bool showPager = isPaginated && totalPages > 1;
    UIWidget* pagerW = nullptr;
    if (showPager) {
        auto pagerIt = widgets.find(el.arrayPagerWidget.empty() ? "arrayPager" : el.arrayPagerWidget);
        if (pagerIt != widgets.end()) pagerW = &pagerIt->second;
    }
    auto pagerScaleFor = [&](float cw) -> float {
        float natW = (float)Components::get()->Window()->getWidth() * pagerW->width;
        return (natW > 0.0f && currentRenderScale > 0.0f) ? cw / (natW * currentRenderScale) : 1.0f;
    };
    // arrayPagerTop (vertical only): the pager goes above and the items move down by its height.
    auto itemsTopOffset = [&](UIWidget& child) -> float {
        if (!pagerW || !el.arrayPagerTop || el.arrayAlign == "horizontal") return 0.0f;
        auto* win = Components::get()->Window();
        float ss = child.scale > 0 ? child.scale : 1.0f;
        float cw = (float)win->getWidth() * child.width * ss * currentRenderScale;
        return (float)win->getHeight() * pagerW->height * pagerScaleFor(cw) * currentRenderScale + el.arrayOffset;
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
                        const float topOff = itemsTopOffset(it->second);
                        const UIWidgetRenderData& fullData = currentData ? *currentData : UIWidgetRenderData{};
                        for (int i = 0; i < resolvedCount; i++) {
                            auto sub = filterDataByPrefix(fullData, el.arrayPrefix + std::to_string(pageOffset + i) + "_");
                            renderWidgetAt(it->second,
                                baseX + el.x + (float)i * stepX,
                                baseY + el.y + topOff + (float)i * stepY, sub, false);
                        }
                        // arrayPager widget renders its own text pass; nothing to do here.
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
    // Scroll: only in pass 1 -- the child widget draws its own text inside renderWidgetAt, so a
    // second full render in the text pass would just double the cost and the hit-tests.
    if (el.type == "scroll")      { if (renderDepth < 4) { renderDepth++; renderScroll(el, baseX, baseY, data); renderDepth--; } return; }
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
            float cw = (float)winPtr->getWidth()  * it->second.width  * ss * currentRenderScale;
            float ch = (float)winPtr->getHeight() * it->second.height * ss * currentRenderScale;
            const float topOff = itemsTopOffset(it->second);
            if (el.bgColor.a > 0.0f) {
                float totalW = (el.arrayAlign == "horizontal") ? (float)(resolvedCount - 1) * stepX + cw : cw;
                float totalH = (el.arrayAlign == "horizontal") ? ch : (float)(resolvedCount - 1) * stepY + ch;
                render->DrawFilledRectToFB((int)(baseX + el.x), (int)(baseY + el.y + topOff), (int)totalW, (int)totalH, applyGA(el.bgColor), currentFB);
            }
            for (int i = 0; i < resolvedCount; i++) {
                std::string prevId      = lastClickedId;
                std::string prevHoverId = lastHoveredId;
                float ix = baseX + el.x + (float)i * stepX;
                float iy = baseY + el.y + topOff + (float)i * stepY;
                auto sub = filterDataByPrefix(fullData, el.arrayPrefix + std::to_string(pageOffset + i) + "_");
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
                    lastClickedId = std::to_string(pageOffset + i) + "_" + lastClickedId;
                if (lastHoveredId != prevHoverId && !lastHoveredId.empty())
                    lastHoveredId = std::to_string(pageOffset + i) + "_" + lastHoveredId;
            }
            // Pagination nav bar — rendered via arrayPager widget
            // (arrayPagerTop: above the items, which were moved down by topOff)
            if (pagerW) {
                float navX = baseX + el.x;
                float navY = (topOff > 0.0f) ? baseY + el.y : baseY + el.y + (float)resolvedCount * stepY;
                // Scale widget to match the array element pixel width
                float pagerScale = pagerScaleFor(cw);
                bool prevEnabled = currentPage > 0;
                bool nextEnabled = (totalPages < 0) || (currentPage < totalPages - 1);
                // Build pager data
                UIWidgetRenderData pagerData;
                Color cActive{0.85f, 0.85f, 0.85f, 1.0f};
                Color cDim   {0.28f, 0.28f, 0.35f, 1.0f};
                // Flecha desactivada (primera/última página): oculta y sin hit-test
                UIElementData prevEl; prevEl.provided = true; prevEl.colorProvided = true;
                prevEl.color = prevEnabled ? cActive : cDim;
                prevEl.hidden = !prevEnabled;
                pagerData["__prev"] = prevEl;
                UIElementData nextEl; nextEl.provided = true; nextEl.colorProvided = true;
                nextEl.color = nextEnabled ? cActive : cDim;
                nextEl.hidden = !nextEnabled;
                pagerData["__next"] = nextEl;
                UIElementData lblEl; lblEl.provided = true;
                lblEl.text = totalPages > 0
                    ? std::to_string(currentPage + 1) + " / " + std::to_string(totalPages)
                    : std::to_string(currentPage + 1);
                pagerData["pageLabel"] = lblEl;
                // Render pager widget (handles both its own text+non-text passes)
                std::string savedClick = lastClickedId;
                float prevPagerScale = pagerW->scale;
                pagerW->scale = pagerScale;
                renderWidgetAt(*pagerW, navX, navY, pagerData, false);
                pagerW->scale = prevPagerScale;
                // Reject clicks on disabled buttons
                if ((lastClickedId == "__prev" && !prevEnabled) ||
                    (lastClickedId == "__next" && !nextEnabled))
                    lastClickedId = savedClick;
            }
        }
        renderDepth--;
    }
}

void UIManager::renderText(const UIElement& el, float x, float y, const UIElementData& data)
{
    if (hitTestOnly) return;
    const std::string& text = data.text.empty() ? el.staticText : data.text;
    if (text.empty()) return;
    // Element font ("font" on a text element) overrides the widget's for this text only; the
    // previous writer is restored on every return path.
    struct WriterRestore { TextWriter*& ref; TextWriter* prev; ~WriterRestore() { ref = prev; } } restoreTw{ tw, tw };
    if (!el.font.empty()) tw = writerForFont(el.font);
    const Color col = applyGA(data.colorProvided ? data.color : el.staticTextColor);

    // Wrapped text: one immediate draw per line, each aligned on its own within w.
    if (el.wrap && el.w > 0.0f) {
        const auto lines = wrapText(text, el.w, el.fontScale, el.maxLines);
        const float lineAdvance = textLineHeight(el.fontScale) * el.lineSpacing;
        float ly = y;
        for (const auto& line : lines) {
            int lx = (int)(x + el.x);
            if (el.textAlign != "left" && !line.empty()) {
                int lineW = tw->measureTextWidthAtlas(line.c_str(), el.fontScale);
                if      (el.textAlign == "center") lx += (int)((el.w - (float)lineW) * 0.5f);
                else if (el.textAlign == "right")  lx += (int)(el.w - (float)lineW);
            }
            if (!line.empty()) tw->writeTextAtlasToFB(lx, (int)ly, line.c_str(), col, el.fontScale, currentFB);
            ly += lineAdvance;
        }
        return;
    }

    int tx = (int)(x + el.x);
    if (el.w > 0.0f && el.textAlign != "left") {
        int textW = tw->measureTextWidthAtlas(text.c_str(), el.fontScale);
        if      (el.textAlign == "center") tx += (int)((el.w - (float)textW) * 0.5f);
        else if (el.textAlign == "right")  tx += (int)(el.w - (float)textW);
    }

    // Inside a scroll viewport the cached path is skipped: rebuilding the text cache binds its own
    // FBO and clears it, and the active glScissor (window-space rect) would clip that clear/draw.
    if (!el.cached || !clipStack.empty()) {
        tw->writeTextAtlasToFB(tx, (int)y, text.c_str(), col, el.fontScale, currentFB);
        return;
    }

    // Cached path: rebuild the FBO when anything that changes its pixels changes -- text, size
    // (el.fontScale is already scaled by the widget/render scale, so it changes on a window
    // resize) or color. Before, only the text was compared and the key was just el.id: after a
    // resize the texture kept its old size, and widgets sharing an id ("name", "title") fought
    // over the same FBO.
    const std::string cacheKey = (currentWidget ? currentWidget->filePath : std::string()) + "#" + el.id;
    char sig[96];
    snprintf(sig, sizeof(sig), "|%.4f|%.3f,%.3f,%.3f,%.3f", el.fontScale, col.r, col.g, col.b, col.a);
    const std::string signature = text + sig;
    if (textCacheContent[cacheKey] != signature) {
        int tw_px = tw->measureTextWidthAtlas(text.c_str(), el.fontScale);
        int th_px = (int)(el.fontScale * 32.0f);
        tw->beginTextCache(cacheKey, std::max(tw_px, 1), std::max(th_px, 1));
        tw->writeTextAtlas(0, 0, text.c_str(), col, el.fontScale);
        tw->endTextCache();
        textCacheContent[cacheKey] = signature;
    }
    tw->drawTextCache(cacheKey, tx, (int)y);
}

// ---------------------------------------------------------------------------
// Text wrap

float UIManager::textLineHeight(float scale) const
{
    const GlyphAtlas* ga = tw ? tw->getGlyphAtlas() : nullptr;
    const int lh = (ga && ga->isBuilt()) ? ga->getLineHeight() : 32;
    return (float)lh * scale;
}

// Greedy word wrap to maxW (window px), measured with the CURRENT writer (so the widget's own font).
// Explicit '\n' starts a new line; a single word wider than maxW is broken by characters.
// maxLines > 0 cuts the rest and ends the last kept line in "...".
std::vector<std::string> UIManager::wrapText(const std::string& text, float maxW, float scale, int maxLines)
{
    const std::string key = std::to_string((uintptr_t)tw) + "|" + std::to_string((int)maxW) + "|" +
                            std::to_string((int)(scale * 1000.0f)) + "|" + std::to_string(maxLines) + "|" + text;
    auto cached = wrapCache.find(key);
    if (cached != wrapCache.end()) return cached->second;
    if (wrapCache.size() > 1024) wrapCache.clear();   // texts change (timers, counters): keep it bounded

    auto width = [&](const std::string& s) { return (float)tw->measureTextWidthAtlas(s.c_str(), scale); };
    std::vector<std::string> lines;
    size_t start = 0;
    while (true) {
        const size_t nl = text.find('\n', start);
        const std::string para = text.substr(start, nl == std::string::npos ? std::string::npos : nl - start);
        std::string line;
        size_t p = 0;
        while (p <= para.size()) {
            size_t sp = para.find(' ', p);
            if (sp == std::string::npos) sp = para.size();
            std::string word = para.substr(p, sp - p);
            p = sp + 1;
            if (!word.empty()) {
                const std::string cand = line.empty() ? word : line + " " + word;
                if (width(cand) <= maxW) {
                    line = cand;
                } else {
                    if (!line.empty()) { lines.push_back(line); line.clear(); }
                    while (word.size() > 1 && width(word) > maxW) {
                        size_t n = word.size() - 1;
                        while (n > 1 && width(word.substr(0, n)) > maxW) n--;
                        lines.push_back(word.substr(0, n));
                        word = word.substr(n);
                    }
                    line = word;
                }
            }
            if (sp >= para.size()) break;
        }
        lines.push_back(line);   // may be empty: blank line between paragraphs
        if (nl == std::string::npos) break;
        start = nl + 1;
    }

    if (maxLines > 0 && (int)lines.size() > maxLines) {
        lines.resize(maxLines);
        std::string& last = lines.back();
        while (!last.empty() && width(last + "...") > maxW) last.pop_back();
        while (!last.empty() && last.back() == ' ') last.pop_back();
        last += "...";
    }
    wrapCache[key] = lines;
    return lines;
}

// ---------------------------------------------------------------------------
// Scroll containers: clip stack (glScissor) + "scroll" element

void UIManager::pushClip(const ClipRect& r)
{
    ClipRect c = r;
    if (!clipStack.empty()) {   // nested viewport: intersect with the enclosing one
        const ClipRect& o = clipStack.back();
        const float x0 = std::max(c.x, o.x), y0 = std::max(c.y, o.y);
        const float x1 = std::min(c.x + c.w, o.x + o.w), y1 = std::min(c.y + c.h, o.y + o.h);
        c = { x0, y0, std::max(0.0f, x1 - x0), std::max(0.0f, y1 - y0) };
    }
    clipStack.push_back(c);
    applyScissor();
}

void UIManager::popClip()
{
    if (!clipStack.empty()) clipStack.pop_back();
    applyScissor();
}

// Widget coordinates are window px; the target layer has its own size (ComponentRender::
// getTargetSize: "ui" = window, the rest = render; the draw helpers scale by target/window), and
// glScissor wants FB px with the origin at the BOTTOM-left.
// The scissor test is always switched off again when the stack empties (popClip), so no other
// render step ever sees it enabled.
void UIManager::applyScissor() const
{
    if (clipStack.empty()) { glDisable(GL_SCISSOR_TEST); return; }
    auto* win = Components::get()->Window();
    int tw, th;
    ComponentRender::getTargetSize(currentFB, tw, th);
    const float rw = (float)tw, rh = (float)th;
    const float rx = rw / (float)win->getWidth(), ry = rh / (float)win->getHeight();
    const ClipRect& c = clipStack.back();
    const GLint sx = (GLint)std::floor(c.x * rx);
    const GLint sy = (GLint)std::floor(rh - (c.y + c.h) * ry);
    const GLint sw = (GLint)std::ceil(c.w * rx);
    const GLint sh = (GLint)std::ceil(c.h * ry);
    glEnable(GL_SCISSOR_TEST);
    glScissor(sx, sy, std::max(0, sw), std::max(0, sh));
}

bool UIManager::pointerInClip(int mx, int my) const
{
    if (clipStack.empty()) return true;
    const ClipRect& c = clipStack.back();
    return (float)mx >= c.x && (float)mx <= c.x + c.w && (float)my >= c.y && (float)my <= c.y + c.h;
}

bool UIManager::isScrollHovered() const
{
    return lastScrollHoverTime >= 0.0f && ((float)SDL_GetTicks() / 1000.0f - lastScrollHoverTime) < 0.1f;
}

// "scroll" element: renders widgetRef inside a (w × h) viewport clipped with glScissor, shifted up
// by the scroll offset. Mouse wheel over the viewport scrolls it; offset clamped to the content
// height measured on the previous render (renderWidgetAt → lastContentH). Lua can force the
// offset with { scrollTo = px } in the element's data (e.g. 0 to go back to the top).
void UIManager::renderScroll(const UIElement& el, float x, float y, const UIElementData& data)
{
    auto it = widgets.find(el.widgetRef);
    if (it == widgets.end() || el.w <= 0.0f || el.h <= 0.0f) return;

    const float vx = x + el.x, vy = y, vw = el.w, vh = el.h;
    ScrollState& st = scrollStates[el.widgetRef + "#" + el.id];
    if (data.scrollTo >= 0.0f) st.offset = data.scrollTo;

    const int mx = input ? input->getRawMouseX() : -1;
    const int my = input ? input->getRawMouseY() : -1;
    const bool over = (float)mx >= vx && (float)mx <= vx + vw && (float)my >= vy && (float)my <= vy + vh
                      && pointerInClip(mx, my);
    if (over) {
        lastScrollHoverTime = (float)SDL_GetTicks() / 1000.0f;
        const int wheel = input->getMouseWheelY();
        if (wheel != 0 && !hitTestOnly) st.offset -= (float)wheel * el.scrollStep * currentRenderScale;
    }
    auto clampOffset = [&]() { st.offset = std::clamp(st.offset, 0.0f, std::max(0.0f, st.contentH - vh)); };
    clampOffset();

    static const UIWidgetRenderData emptyData;
    const UIWidgetRenderData& fullData = currentData ? *currentData : emptyData;
    auto sub = filterDataByPrefix(fullData, el.id + "_");

    pushClip({ vx, vy, vw, vh });
    renderWidgetAt(it->second, vx, vy - st.offset, sub, true);
    st.contentH = lastContentH;
    popClip();
    clampOffset();

    if (el.scrollbar && !hitTestOnly && st.contentH > vh + 0.5f) {
        const float barW   = std::max(2.0f, 4.0f * currentRenderScale);
        const float barX   = vx + vw - barW;
        const float thumbH = std::max(barW * 2.0f, vh * vh / st.contentH);
        const float thumbY = vy + (vh - thumbH) * (st.offset / (st.contentH - vh));
        Color track = el.scrollbarColor;
        track.a *= 0.35f;
        render->DrawFilledRectToFB((int)barX, (int)vy,     (int)barW, (int)vh,     applyGA(track),              currentFB);
        render->DrawFilledRectToFB((int)barX, (int)thumbY, (int)barW, (int)thumbH, applyGA(el.scrollbarColor), currentFB);
    }
}

Image* UIManager::getCachedImage(const std::string& path)
{
    if (path.empty() || !render) return nullptr;
    auto it = imagePointerCache.find(path);
    if (it == imagePointerCache.end()) {
        Image* img = render->getOrLoadImage(path);
        imagePointerCache[path] = img;
        return img;
    }
    return it->second;
}

void UIManager::drawImageCached(const std::string& path, int x, int y, int w, int h, float alpha)
{
    Image* img = getCachedImage(path);
    if (img)
        render->DrawImage2DFromImageToFB(img, x, y, w, h, currentFB, alpha);
}

void UIManager::renderImage(const UIElement& el, float x, float y, const UIElementData& data)
{
    if (hitTestOnly) return;
    const std::string& path = data.path.empty() ? el.staticImagePath : data.path;
    if (path.empty()) return;
    int ix = (int)(x + el.x), iy = (int)y;
    int iw = (int)(el.w * el.imageScale), ih = (int)(el.h * el.imageScale);
    drawImageCached(path, ix, iy, iw, ih, globalAlpha);
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
        if (mx >= ix && mx <= ix+iw && my >= iy && my <= iy+ih && pointerInClip(mx, my))
            pendingTooltip = { tt, mx, my, true, "", {}, currentFB };
    }
}

void UIManager::renderRect(const UIElement& el, float x, float y, const UIElementData& data)
{
    if (hitTestOnly) return;
    auto* win = Components::get()->Window();
    float w = el.wPct > 0 ? (float)win->getWidth()  * el.wPct
            : data.w  > 0 ? data.w : el.w;
    float h = el.hPct > 0 ? (float)win->getHeight() * el.hPct
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
    if (hitTestOnly) return;
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
    if (hitTestOnly) return;
    float cx = x + el.x;
    for (auto& key : data.iconList) {
        auto mapIt = el.iconMapping.find(key);
        if (mapIt == el.iconMapping.end()) continue;
        drawImageCached(mapIt->second, (int)cx, (int)y,
                        (int)el.iconSize, (int)el.iconSize, globalAlpha);
        cx += el.iconSize + el.iconGap;
    }
}

void UIManager::renderButton(const UIElement& el, float x, float y, const UIElementData& data)
{
    const bool hasContent = data.provided || !el.staticText.empty() || !el.staticImagePath.empty() || !el.options.empty();
    const bool hasVisuals = el.btnBg.a > 0.0f || el.btnHover.a > 0.0f || el.btnPressed.a > 0.0f || el.borderWidth > 0.0f;
    if (!hasContent && !hasVisuals) return;

    float bx = x + el.x;
    float by = y;
    float bw = el.w > 0 ? el.w : 80.0f;
    float bh = el.h > 0 ? el.h : 20.0f;

    int mx = input ? input->getRawMouseX() : -1;
    int my = input ? input->getRawMouseY() : -1;
    // pointerInClip: a button scrolled out of a "scroll" viewport must not react to the mouse.
    bool hovered = mx >= (int)bx && mx <= (int)(bx + bw) &&
                   my >= (int)by && my <= (int)(by + bh) && pointerInClip(mx, my);
    bool pressed = hovered && input && input->isLeftMouseButtonPressed();
    bool clicked = hovered && input && input->isMouseButtonUp();

    const std::string hoverKey = el.id + "@" + std::to_string((int)bx) + "," + std::to_string((int)by);
    if (hovered && !el.sound.empty()) {
        bool wasHovered = lastHoverState.count(hoverKey) && lastHoverState[hoverKey];
        if (!wasHovered)
            Components::get()->Sound()->PlaySound(el.sound, ComponentSound::CHANNEL_UI_HOVER, 0);
    }
    if (hovered && !el.cursor.empty()) hoveredCursorName = el.cursor;
    lastHoverState[hoverKey] = hovered;

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
        if (!hitTestOnly && !resolvedText.empty()) {
            float scale = el.fontScale > 0 ? el.fontScale : 0.45f;
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

    if (!hitTestOnly) {
        Color bg = applyGA(pressed ? el.btnPressed : hovered ? el.btnHover : el.btnBg);
        render->DrawFilledRectToFB((int)bx, (int)by, (int)bw, (int)bh, bg, currentFB);

        if (el.borderWidth > 0.0f) {
            int bord = (int)el.borderWidth;
            Color bc = applyGA(pressed ? el.borderColorPressed : hovered ? el.borderColorHover : el.borderColor);
            render->DrawFilledRectToFB((int)bx,           (int)by,           (int)bw, bord,   bc, currentFB);
            render->DrawFilledRectToFB((int)bx,           (int)(by+bh-bord), (int)bw, bord,   bc, currentFB);
            render->DrawFilledRectToFB((int)bx,           (int)by,           bord, (int)bh,   bc, currentFB);
            render->DrawFilledRectToFB((int)(bx+bw-bord), (int)by,           bord, (int)bh,   bc, currentFB);
        }

        if (imgSize > 0) {
            if (resolvedText.empty() && el.iconSize <= 0) {
                drawImageCached(resolvedPath,
                    (int)(bx + el.btnImgOffsetX), (int)(by + el.btnImgOffsetY),
                    (int)bw, (int)bh, globalAlpha);
            } else {
                float imgX = resolvedText.empty() ? bx + (bw - imgSize) * 0.5f : bx + imgPad;
                drawImageCached(resolvedPath,
                    (int)(imgX + el.btnImgOffsetX), (int)(by + imgPad + el.btnImgOffsetY),
                    (int)imgSize, (int)imgSize, globalAlpha);
            }
        }
    }

    if (clicked && lastClickedId.empty()) {
        lastClickedId = el.id;
        if (input) input->consumeLeftClick();
    }

    bool rightClicked = hovered && input && input->isClickRightUp();
    if (rightClicked && lastRightClickedId.empty()) {
        lastRightClickedId = el.id;
    }

    if (hovered && lastHoveredId.empty()) {
        lastHoveredId = el.id;
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

    // Siempre en la capa "ui" (la de arriba al componer: background -> foreground -> ui), no en la
    // del elemento que lo pidió (pendingTooltip.fb): un tooltip de un widget en "foreground" quedaba
    // tapado por cualquier cosa de "ui". Lo llama ComponentRender::postUpdate() al final del frame.
    const std::string tfb = "ui";

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
            currentFB = tfb;
            renderWidgetAt(tw_w, bx, by, pendingTooltip.widgetData, true);
            globalAlpha = 1.0f;
            return;
        }
    }

    // Plain text tooltip fallback
    if (pendingTooltip.text.empty()) { globalAlpha = 1.0f; return; }

    constexpr float scale  = 0.45f;
    constexpr float pad    = 8.0f;

    // Medidas reales del atlas (como el texto de los botones en renderElement): antes eran
    // estimaciones fijas (13 px de ancho por carácter, 22 px de alto) y la fuente mide ~35-42 px de
    // alto de línea, así que el texto se salía por abajo del cuadro, pegado al borde.
    float textW = scale * 13.0f * (float)pendingTooltip.text.size();
    float textH = scale * 22.0f;
    if (tw->getGlyphAtlas() && tw->getGlyphAtlas()->isBuilt()) {
        const GlyphAtlas* ga = tw->getGlyphAtlas();
        float cx = 0.0f;
        for (char c : pendingTooltip.text) {
            const GlyphInfo& g = ga->getGlyphInfo(c);
            cx += (float)(g.valid ? g.advance : ga->getGlyphInfo(' ').advance) * scale;
        }
        textW = cx;
        textH = (float)ga->getLineHeight() * scale;
    }

    float boxW = std::ceil(textW + pad * 2.0f);
    float boxH = std::ceil(textH + pad * 2.0f);

    float bx = (float)pendingTooltip.x + 14.0f;
    float by = (float)pendingTooltip.y - boxH - 6.0f;
    if (bx + boxW > (float)win->getWidth())  bx = (float)win->getWidth()  - boxW - 2.0f;
    if (by < 0)                               by = (float)pendingTooltip.y + 16.0f;

    const Color bg     {0.06f, 0.06f, 0.09f, 0.96f};
    const Color border {0.52f, 0.55f, 0.65f, 1.00f};
    const Color text   {0.92f, 0.92f, 0.95f, 1.00f};

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

    sol::object pageObj = t["page"];
    if (pageObj.valid()) d.page = pageObj.as<int>();

    sol::object scrollToObj = t["scrollTo"];
    if (scrollToObj.valid() && scrollToObj.get_type() == sol::type::number) d.scrollTo = scrollToObj.as<float>();

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

std::tuple<float, std::string, std::string, std::string> UIManager::drawWidgetLua(const std::string& name, sol::table data, const std::string& fb, float scaleOverride)
{
    auto it = widgets.find(name);
    if (it == widgets.end()) {
        printf("[UIManager] WARN: widget '%s' not found\n", name.c_str());
        return {0.0f, "", "", ""};
    }

    float nextY = drawWidget(name, solTableToRenderData(data), fb, scaleOverride);
    return {nextY, lastClickedId, lastRightClickedId, lastHoveredId};
}

std::tuple<float, std::string, std::string, std::string> UIManager::drawWidgetAtPosLua(const std::string& name, float x, float y, sol::table data, const std::string& fb, float scaleOverride)
{
    auto it = widgets.find(name);
    if (it == widgets.end()) {
        printf("[UIManager] WARN: widget '%s' not found\n", name.c_str());
        return {y, "", "", ""};
    }

    float nextY = drawWidgetAtPos(name, x, y, solTableToRenderData(data), fb, scaleOverride);
    return {nextY, lastClickedId, lastRightClickedId, lastHoveredId};
}
