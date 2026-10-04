#ifndef BRAKEDA3D_UIMANAGER_H
#define BRAKEDA3D_UIMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include "../../../sol/sol.hpp"
#include "../Color.h"
#include <GL/glew.h>

class ComponentInput;
class Image;

// Per-element data used by the core renderer (no Lua dependency)
struct UIElementData {
    std::string text      = "";
    Color       color     = {1.0f, 1.0f, 1.0f, 1.0f};
    bool        provided  = false;   // true only when explicitly passed from Lua
    bool        colorProvided = false; // true only when color key was explicitly passed
    std::string path      = "";
    std::string pathOff   = "";      // button off-state image
    bool        enabled   = true;    // button toggle state (true=on image, false=off image)
    std::string key       = "";      // selects an option from UIElement::options
    std::string tooltip   = "";      // dynamic tooltip override from Lua
    float       w         = 0;
    float       h         = 0;
    float       value     = 75.0f;
    float       maxValue  = 100.0f;
    std::vector<std::string> iconList;
    int         count     = 0;            // array count override (0 = use element's arrayCount)
    int         page      = -1;           // pagination: -1 = off, 0+ = show page N (N*count .. (N+1)*count-1)
    float       scrollTo  = -1.0f;        // "scroll" elements: >= 0 sets the scroll offset (px) this frame (Lua: { scrollTo = 0 })
    bool        hidden    = false;        // true: the element is neither drawn nor hit-tested this frame

    bool operator==(const UIElementData& o) const {
        return provided == o.provided && colorProvided == o.colorProvided && hidden == o.hidden &&
               enabled == o.enabled && key == o.key && text == o.text &&
               path == o.path && pathOff == o.pathOff && tooltip == o.tooltip &&
               value == o.value && maxValue == o.maxValue && w == o.w && h == o.h &&
               count == o.count && page == o.page && scrollTo == o.scrollTo && iconList == o.iconList &&
               (!colorProvided || (color.r == o.color.r && color.g == o.color.g &&
                                   color.b == o.color.b && color.a == o.color.a));
    }
    bool operator!=(const UIElementData& o) const { return !(*this == o); }
};

// Named static option baked into a button element definition
struct UIElementOption {
    std::string key;
    std::string text;
    std::string image;
    std::string tooltip;
    std::string tooltipWidget;   // widget name rendered as rich tooltip (overrides plain text tooltip)
    Color       color    {1.0f, 1.0f, 1.0f, 1.0f};
    bool        colorSet {false};
};

using UIWidgetRenderData = std::unordered_map<std::string, UIElementData>;

class ComponentRender;
class TextWriter;

struct UIElement {
    std::string id;
    std::string type; // "text", "image", "rect", "progressbar", "icons"
    float x{0}, y{0};
    bool yAuto{false};
    float w{0}, h{0};

    // rect
    float alpha{1.0f};
    Color rectColor{1.0f, 1.0f, 1.0f, 1.0f};
    float wPct{0};   // 0 = fixed px; >0 = fraction of window width  (e.g. 1.0 = 100%)
    float hPct{0};   // 0 = fixed px; >0 = fraction of window height
    std::string alignH{"left"};   // "left" | "center" | "right"
    std::string alignV{"top"};    // "top"  | "center" | "bottom"

    // text
    float fontScale{0.5f};
    std::string font;                  // "text" elements: own font file (.ttf/.otf); empty = the widget's font
    Color staticTextColor{1.0f, 1.0f, 1.0f, 1.0f};
    std::string textAlign{"left"};  // "left" | "center" | "right" — dentro del w del item
    bool cached{false};             // true → cachea el texto en FBO, solo reconstruye si cambia

    // image
    float imageScale{1.0f};

    // button
    Color btnBg        {0.12f, 0.20f, 0.32f, 0.92f};
    Color btnHover     {0.22f, 0.38f, 0.58f, 0.96f};
    Color btnPressed   {0.08f, 0.13f, 0.22f, 1.00f};
    float btnImgOffsetX{0.0f};
    float btnImgOffsetY{0.0f};
    float btnTextOffsetX{0.0f};
    float btnTextOffsetY{0.0f};
    std::string staticText;          // text baked into the JSON definition
    std::string staticImagePath;     // on-state image baked into the JSON definition
    std::string staticImagePathOff;  // off-state image baked into the JSON definition
    std::string tooltip;             // static tooltip shown on hover
    std::string sound;               // sound ID to play once on hover-enter
    std::string cursor;              // cursor name to show on hover (e.g. "attack", "select", "move")
    std::vector<UIElementOption> options; // named static options (key → text/image/color)

    // progressbar
    float barW{0}, barH{0};
    float textOffsetX{0};
    float textOffsetY{0};
    float barFontScale{0.338f};
    Color barBg{0.18f, 0.18f, 0.18f, 0.85f};
    Color barOk{0.0f, 0.78f, 0.12f, 1.0f};
    Color barMid{0.82f, 0.55f, 0.0f, 1.0f};
    Color barLow{0.85f, 0.0f, 0.0f, 1.0f};

    // icons
    float iconSize{16}, iconGap{2};
    std::unordered_map<std::string, std::string> iconMapping;

    // widget (nested)
    std::string widgetRef;

    // array
    int   arrayCount{1};
    std::string arrayAlign{"vertical"}; // "horizontal" | "vertical"
    bool  arrayDistribute{false};       // true  → reparte items en todo el ancho (H) o alto (V) de ventana
    float arrayOffset{0};              // false → px extra entre items en la dirección de alineación
    std::string arrayPrefix;           // prefix for row data keys (default "": row i uses "i_", prefix "d" → "d0_")
    bool  arrayPaginate{false};        // enables paging: arrayCount=page size, data.count=total, data.page=current page
    std::string arrayPagerWidget;      // widget name used as pager bar (default "arrayPager")
    bool  arrayPagerTop{false};        // vertical arrays: pager ABOVE the items (they move down by its height)

    // border (image, button, array items)
    Color borderColor       {1.0f, 1.0f, 1.0f, 1.0f};
    Color borderColorHover  {1.0f, 1.0f, 1.0f, 1.0f};
    Color borderColorPressed{1.0f, 1.0f, 1.0f, 1.0f};
    float borderWidth{0.0f};           // 0 = no border

    // array item background
    Color bgColor{0.0f, 0.0f, 0.0f, 0.0f};  // alpha=0 → no background

    // padding
    float paddingLeft  {0.0f};   // shifts element right (guaranteed left side)
    float paddingTop   {0.0f};   // top margin: pre-element space (yAuto) / adds to y (fixed)
    float paddingBottom{0.0f};   // bottom margin: post-element space (yAuto) / adds to effective height (fixed)

    bool enabled{true};          // false → element is skipped entirely during rendering

    // text wrap ("text" elements): word-wrap to the element width w, measured with the widget's font
    bool  wrap{false};
    float lineSpacing{1.2f};     // line advance = font line height × lineSpacing
    int   maxLines{0};           // 0 = unlimited; otherwise extra text is cut and the last line ends in "..."

    // scroll container ("scroll" elements): renders widgetRef clipped to (w × h), mouse wheel scrolls it
    float scrollStep{40.0f};     // px scrolled per wheel notch (scaled with the widget)
    bool  scrollbar{true};       // thin scrollbar on the right edge when content overflows
    Color scrollbarColor{1.0f, 1.0f, 1.0f, 0.35f};
};

struct UIWidget {
    float height{0};
    float width{0};
    float scale{1.0f};
    float posX{0.0f};     // percentage of window width  (0.0 = left, 1.0 = right)
    float posY{0.0f};     // percentage of window height (0.0 = top,  1.0 = bottom)
    float offsetX{0.0f};  // pixel offset added on top of the computed X position
    float offsetY{0.0f};  // pixel offset added on top of the computed Y position
    std::string filePath;  // absolute path to the source JSON file
    std::vector<UIElement> elements;
    UIWidgetRenderData defaultData;  // baked-in data from JSON "data" section


    // Panel background and border (drawn by renderWidgetAt around the full widget)
    Color bgColor    {0.0f, 0.0f, 0.0f, 0.0f};  // alpha=0 → no background
    std::string bgImage;   // optional: image over the widget box (declared width × height), drawn after bgColor
    float bgSlice[4]{0.0f, 0.0f, 0.0f, 0.0f};  // 9-slice cuts L,T,R,B in IMAGE px; all 0 = stretch the whole image
    float bgSliceScale{1.0f};                   // screen px per image px for the borders (× widget scale)
    float bgImageAlpha{1.0f};                   // opacity of bgImage (stretch and 9-slice), × the image's own alpha
    std::string font;      // optional: font file (.ttf/.otf) for this widget's text; empty = engine default. Nested widgets inherit it unless they set their own
    float refHeight{0.0f}; // design window height (px). >0 → text sizes × (window height / refHeight), so text follows resizes like the box does. 0 = fixed size. Nested widgets inherit it unless they set their own
    Color borderColor{1.0f, 1.0f, 1.0f, 1.0f};
    float borderWidth{0.0f};                       // 0 → no border
    std::string cursor;    // cursor name to set when mouse is anywhere within widget bounds
    bool cacheable{true};  // false → never cache (interactive: hover/press state per frame)

    // Runtime cache — not persisted, filled lazily on first dirty render
    mutable GLuint cacheFBO     {0};
    mutable GLuint cacheTexture {0};
    mutable int    cacheRW      {0};
    mutable int    cacheRH      {0};
    mutable int    cacheWinW    {0};   // window size the cache was laid out at (layout follows the window)
    mutable int    cacheWinH    {0};
    mutable bool   cacheDirty   {true};
    mutable UIWidgetRenderData cachedData;
};

class UIManager {
public:
    void init(ComponentRender* render, const std::string& widgetsDir);
    void loadWidgets();
    void reloadWidgets();
    void clearWidgets();

    // Runtime loading — can be called at any time, including from Lua
    void loadWidgetFromFile(const std::string& filePath);
    void loadWidgetsFromDir(const std::string& dir);
    void unloadWidget(const std::string& name);
    // Fuerza el re-render de la caché FBO en el siguiente draw (solo afecta a widgets cacheables
    // dibujados a primer nivel: los anidados no tienen caché propia, se re-renderizan con su padre).
    bool invalidateWidget(const std::string& name);
    void invalidateAllWidgets();
    // Design resolution of the UI (window px the layouts were made at). When set (> 0), text sizes
    // of top-level widgets follow the window like the boxes do: × min(windowW / w, windowH / h), so
    // a text never outgrows its box on either axis. 0 = off (fixed text size). refHeight on a widget
    // still overrides it.
    void setDesignResolution(float w, float h);
    [[nodiscard]] float getDesignTextScale() const;

    // Core renderer — no Lua. fb = "foreground"|"ui"|"background"|"global"|"scene"
    // Renders at the widget's stored (posX*winW, posY*winH).
    // scaleOverride > 0 replaces the widget's stored scale for this call only.
    float drawWidget(const std::string& name, const UIWidgetRenderData& data, const std::string& fb = "foreground", float scaleOverride = 0.0f);
    float drawWidgetAtPos(const std::string& name, float x, float y, const UIWidgetRenderData& data, const std::string& fb = "foreground", float scaleOverride = 0.0f);

    // Lua wrappers — convert sol::table → UIWidgetRenderData and call the core methods.
    // Return {nextY, clickedButtonId, rightClickedButtonId} — ids are "" if not clicked.
    std::tuple<float, std::string, std::string, std::string> drawWidgetLua(const std::string& name, sol::table data, const std::string& fb = "foreground", float scaleOverride = 0.0f);
    std::tuple<float, std::string, std::string, std::string> drawWidgetAtPosLua(const std::string& name, float x, float y, sol::table data, const std::string& fb = "foreground", float scaleOverride = 0.0f);

    // Call once at the very end of a frame's HUD drawing (after all drawWidget calls)
    // to render any pending tooltip on top of everything else.
    // dt = frame delta time in seconds (used for delay + fade-in).
    void flushTooltip(float dt = 0.0f);

    std::unordered_map<std::string, UIWidget>& getWidgets() { return widgets; }
    const std::string& getWidgetsDir() const { return widgetsDir; }
    const std::string& getHoveredCursorName() const { return hoveredCursorName; }
    // true if the mouse was over a "scroll" viewport in the last ~0.1 s: gameplay code (e.g. the RTS
    // camera zoom) should ignore the mouse wheel then, since the UI is using it.
    bool isScrollHovered() const;
    static UIElementData solTableToElementData(sol::table t);

    // Editor debug: when set, every render of that widget (top-level or nested
    // via array/widgetRef) gets a semi-transparent overlay drawn on top so the
    // user can see WHERE it is being applied in the scene. Empty = disabled.
    void setDebugHighlightName(const std::string& n) { debugHighlightName = n; }
    const std::string& getDebugHighlightName() const { return debugHighlightName; }

    void setGlobalAlpha(float a) { globalAlpha = (a < 0.0f ? 0.0f : a > 1.0f ? 1.0f : a); }

private:
    ComponentRender* render{nullptr};
    ComponentInput*  input{nullptr};
    TextWriter* tw{nullptr};
    TextWriter* defaultTw{nullptr};                               // engine default writer (render->getTextWriter())
    std::unordered_map<std::string, TextWriter*> fontWriters;     // font path → writer with its own glyph atlas (lives for the whole process)
    TextWriter* writerForFont(const std::string& fontPath);
    std::string widgetsDir;
    std::vector<std::string> loadedDirs;
    std::unordered_map<std::string, UIWidget> widgets;
    std::string lastClickedId;
    std::string lastRightClickedId;
    std::string lastHoveredId;
    std::string hoveredCursorName;
    std::unordered_map<std::string, bool> lastHoverState;
    std::unordered_map<std::string, std::string> textCacheContent; // dirty-check para "cached": texto + tamaño + color
    std::unordered_map<std::string, Image*> imagePointerCache;     // path → Image* para evitar getOrLoad cada frame
    std::string currentFB{"foreground"};
    bool textPass{false};
    bool hitTestOnly{false};
    int renderDepth{0};
    int widgetRenderDepth{0};   // tracks nesting: only outermost call resets lastClickedId
    float currentRenderScale{1.0f};
    float designW{0.0f}, designH{0.0f};   // setDesignResolution(); 0 = off
    float currentTextScale{1.0f};   // render height / refHeight of the nearest widget (self or parent) that sets refHeight
    const UIWidgetRenderData* currentData{nullptr};
    const UIWidget* currentWidget{nullptr};   // widget being rendered (key of "cached" text FBOs)

    // ── Text wrap ──────────────────────────────────────────────────────────
    // Wrapped lines cached per (writer, text, width, scale): wrapping measures every word.
    std::unordered_map<std::string, std::vector<std::string>> wrapCache;
    std::vector<std::string> wrapText(const std::string& text, float maxW, float scale, int maxLines);
    float textLineHeight(float scale) const;   // one line of the current writer at this scale (window px)

    // ── Scroll containers ──────────────────────────────────────────────────
    struct ScrollState { float offset{0.0f}; float contentH{0.0f}; };
    std::unordered_map<std::string, ScrollState> scrollStates;   // key: widgetRef + "#" + element id
    struct ClipRect { float x{0}, y{0}, w{0}, h{0}; };           // window px
    std::vector<ClipRect> clipStack;                             // active scroll viewports (intersected)
    float lastContentH{0.0f};          // real content height measured by the last renderWidgetAt call
    float lastScrollHoverTime{-1.0f};  // execution time the mouse was last over a scroll viewport
    void pushClip(const ClipRect& r);
    void popClip();
    void applyScissor() const;         // sets/clears GL_SCISSOR_TEST from clipStack
    bool pointerInClip(int mx, int my) const;
    void renderScroll(const UIElement& el, float x, float y, const UIElementData& data);

    void loadWidget(const std::string& filePath);
    void parseWidgetJSON(const std::string& filePath);   // parse + store; no ref-resolve, no height
    float renderWidgetAt(UIWidget& w, float x, float y, const UIWidgetRenderData& data, bool useDefaultData = true);
    void drawImageCached(const std::string& path, int x, int y, int w, int h, float alpha);
    Image* getCachedImage(const std::string& path);   // path → Image* (imagePointerCache)
    void renderElement(const UIElement& el, float baseX, float baseY, const UIElementData& data);
    void renderText(const UIElement& el, float x, float y, const UIElementData& data);
    void renderImage(const UIElement& el, float x, float y, const UIElementData& data);
    void renderRect(const UIElement& el, float x, float y, const UIElementData& data);
    void renderProgressBar(const UIElement& el, float x, float y, const UIElementData& data);
    void renderIcons(const UIElement& el, float x, float y, const UIElementData& data);
    void renderButton(const UIElement& el, float x, float y, const UIElementData& data);
    float getElementHeight(const UIElement& el);

    struct PendingTooltip {
        std::string text;
        int x{0}, y{0};
        bool active{false};
        std::string widgetName;          // non-empty → render widget instead of plain text box
        UIWidgetRenderData widgetData;   // data forwarded to the tooltip widget
        std::string fb{"foreground"};    // FB in which this tooltip should be rendered
    };
    PendingTooltip pendingTooltip;

    // Tooltip delay + fade-in state
    static constexpr float TOOLTIP_DELAY   = 0.35f;  // seconds before showing
    static constexpr float TOOLTIP_FADE_IN = 0.15f;  // seconds to fade in
    std::string hoverKey;           // identifies the currently-hovered tooltip
    float       hoverElapsed{0.0f}; // seconds the current tooltip has been hovered
    float       globalAlpha{1.0f};  // multiplied into every color during render (1.0 = normal)
    std::string debugHighlightName; // editor overlay: name of widget to visualize

    Color applyGA(Color c) const { c.a *= globalAlpha; return c; }
};

#endif
