#ifndef BRAKEDA3D_UIMANAGER_H
#define BRAKEDA3D_UIMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <cstdint>
#include <deque>
#include <unordered_set>
#include "../../../sol/sol.hpp"
#include "../Color.h"
#include <GL/glew.h>

class ComponentInput;
class Image;

// ─────────────────────────────────────────────────────────────────────────────────────────────────
// CACHÉ DE WIDGETS — qué se cachea, cómo y con qué límites
//
// Cada ELEMENTO es fijo (por defecto) o "live" (JSON "live": true):
//   · fijo  → se hornea en la imagen cacheada del widget; solo se vuelve a pintar cuando cambian sus
//             datos (y en "variants" sus datos forman la clave de la variante).
//   · live  → nunca se cachea: se pinta en cada llamada, ENCIMA de lo cacheado. Para lo que cambia a
//             menudo (barra de vida, contador, temporizador) o es interactivo (botón con hover).
//   Un "widget"/"array" anidado marcado live es live entero; los elementos live de un hijo anidado
//   cuentan como live del padre (el hijo se hornea dentro del padre).
//
// Cada WIDGET elige su modo (JSON "cache"; el antiguo "cacheable": true/false = "single"/"none"):
//   · "none"     → se pinta entero en cada llamada. Siempre así si tiene un "scroll".
//   · "single"   → UNA imagen (FBO del tamaño de la capa) con el último dato fijo dibujado. Para paneles
//                  únicos (barra inferior, minimapa…). Si dos llamadas por frame le pasan datos fijos
//                  distintos, se rehace dos veces por frame: para muchas copias, "variants".
//   · "variants" → hasta "maxVariants" imágenes (por defecto 64) en un atlas por widget, una por cada
//                  combinación distinta de los datos FIJOS (los de los elementos live no cuentan). Para
//                  muchas copias del mismo widget (avatares, etiquetas, marcadores): 200 avatares con 6
//                  retratos distintos = 6 variantes. Cada copia = 1 quad del atlas + sus elementos live,
//                  todo al lote de UI. drawWidgetInstances() pinta N copias en una sola llamada.
//
// CUÁNDO COMPENSA "variants" (medido 2026-10-10, 200 avatares RTS de imagen + barra + nombre): cada copia
// paga calcular la firma de sus datos fijos y buscarla (~5 µs), y la maquetación de sus elementos live
// se recorre igual. Si lo fijo es UNA imagen (un quad en el lote) sale más caro que "none": 6-7 µs/copia
// con "none" + drawWidgetInstances frente a 11-13 µs con "variants". Compensa cuando lo fijo es caro
// de dibujar: varios textos, marcos 9-slice, anidados. Lo que sí ahorra siempre es drawWidgetInstances
// (un lote para todas las copias: ~1000 llamadas de dibujo menos con 200 avatares).
//
// LIMITACIONES (cache "single"/"variants"; validateWidgetCache() las comprueba y el editor las muestra):
//   1. Orden: lo live va SIEMPRE encima de todo lo cacheado (no puede quedar debajo de un elemento fijo).
//   2. Interactivo: un elemento fijo se hornea en su estado normal. Botones fijos siguen recibiendo
//      clics, pero sin hover/pulsado/sonido visibles; tooltips de imágenes fijas no salen. Si hace
//      falta respuesta visual → live, o el widget en "none".
//   3. Datos cambiantes en elementos fijos: cada valor distinto es una variante nueva. Al llegar a
//      maxVariants (o llenarse el atlas) se vacían TODAS y se avisa en el log: ese elemento debería
//      ser live. Lo mismo si cambia la escala: forma parte de la clave.
//   4. Imágenes: se cargan en un hilo; si al hornear alguna no ha terminado, esa caché/variante se
//      rehace en los dibujados siguientes hasta que esté completa (una ruta rota → se rehace siempre).
//   5. Se rehace todo al cambiar el tamaño de ventana o de la capa destino, al recargar el JSON
//      (editor) o con invalidateWidget(). Un widget "variants" debe dibujarse siempre en la misma
//      capa (fb): el atlas tiene el tamaño de esa capa.
//   6. Por copia solo cambian posición, opacidad y los elementos live: nada de rotar ni deformar.
//   7. Solo se cachea el widget de primer nivel; los anidados se hornean dentro de él.
//   8. drawWidgetInstances: los rects translúcidos se mezclan como las imágenes (operador "over")
//      para no partir el lote; el color es idéntico, el alfa que queda en la capa puede variar
//      mínimamente respecto a drawWidgetAtPos.
// ─────────────────────────────────────────────────────────────────────────────────────────────────
enum class UICacheMode { None, Single, Variants };

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
    float       scale     = 1.0f;         // "image" elements: size multiplier around the element's center (Lua: { scale = 1.05 }, e.g. a pulsing logo)
    bool        flipX     = false;        // "image" elements: mirror horizontally (Lua: { flipX = true }); "color" (rgb) tints the image, e.g. { color = Color.new(0.4,0.4,0.4,1) } darkens it

    bool operator==(const UIElementData& o) const {
        return provided == o.provided && colorProvided == o.colorProvided && hidden == o.hidden &&
               enabled == o.enabled && key == o.key && text == o.text &&
               path == o.path && pathOff == o.pathOff && tooltip == o.tooltip &&
               value == o.value && maxValue == o.maxValue && w == o.w && h == o.h &&
               count == o.count && page == o.page && scrollTo == o.scrollTo && iconList == o.iconList &&
               scale == o.scale && flipX == o.flipX &&
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

// Element type as an enum: the renderer compares it many times per element and pass (before, string
// compares). The "type" string stays the source of truth (JSON, editor); kind is derived from it.
enum class UIElementKind : uint8_t { Unknown, Text, Image, Rect, ProgressBar, Icons, Button, Widget, Array, Scroll };

inline UIElementKind uiElementKindFromName(const std::string& t)
{
    if (t == "text")        return UIElementKind::Text;
    if (t == "image")       return UIElementKind::Image;
    if (t == "rect")        return UIElementKind::Rect;
    if (t == "progressbar") return UIElementKind::ProgressBar;
    if (t == "icons")       return UIElementKind::Icons;
    if (t == "button")      return UIElementKind::Button;
    if (t == "widget")      return UIElementKind::Widget;
    if (t == "array")       return UIElementKind::Array;
    if (t == "scroll")      return UIElementKind::Scroll;
    return UIElementKind::Unknown;
}

// widget / array / scroll: render another widget inside
inline bool uiElementIsContainer(UIElementKind k)
{
    return k == UIElementKind::Widget || k == UIElementKind::Array || k == UIElementKind::Scroll;
}

struct UIElement {
    std::string id;
    std::string type; // "text", "image", "rect", "progressbar", "icons"
    // Derived from type: set on parse, by the editor when it changes the type, and on every
    // renderWidgetAt scale pass (buildScaled), so a stale value never reaches the renderer.
    UIElementKind kind{UIElementKind::Unknown};
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
    // Skins: image per state stretched over the whole button (replaces the flat color rect of that state).
    // Empty hover/pressed fall back to the normal skin; empty disabled falls back to normal (see UIElementData::enabled).
    std::string btnSkin, btnSkinHover, btnSkinPressed, btnSkinDisabled;
    // 9-slice for the skins (same cuts for the four states): L/T/R/B in IMAGE px, all 0 = stretch the whole image.
    // btnSkinSliceScale = screen px per image px for corners/edges.
    float btnSkinSlice[4]{0.0f, 0.0f, 0.0f, 0.0f};
    float btnSkinSliceScale{1.0f};
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
    bool  barShowText{true};         // false = no "valor/máximo" text over the bar (JSON "barShowText")
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
    std::string arrayAlign{"vertical"}; // "horizontal" | "vertical" | "grid"
    int   arrayColumns{4};              // "grid": items per row (filled left to right, then top to bottom)
    bool  arrayDistribute{false};       // true  → reparte items en todo el ancho (H) o alto (V) de ventana
    float arrayOffset{0};              // false → px extra entre items en la dirección de alineación
    std::string arrayPrefix;           // prefix for row data keys (default "": row i uses "i_", prefix "d" → "d0_")
    bool  arrayCountIsMax{false};      // non-paginated: arrayCount is the MAXIMUM items drawn (Lua count above it is clamped); false = Lua count wins, arrayCount only when no data
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
    bool live{false};            // true → never cached: drawn on every call over the cached image (see top of file)

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


    // Panel background, frame and border (drawn by renderWidgetAt around the full widget), bottom to top:
    //   bgColor -> bgImage (fondo, estirado) -> frameImage (marco 9-slice) -> elementos -> borde
    // Fondo y marco son independientes y pueden convivir.
    Color bgColor    {0.0f, 0.0f, 0.0f, 0.0f};  // alpha=0 → no background
    std::string bgImage;   // optional: background image stretched over the widget box (declared width × height)
    float bgImageAlpha{1.0f};                   // opacity of bgImage, × the image's own alpha
    float bgImageInset{0.0f};                   // px (× widget scale): bgImage drawn inset from the edges, so it stays inside a frame with rounded/transparent corners
    float bgColorInset[4]{0.0f, 0.0f, 0.0f, 0.0f}; // px (× widget scale) L,T,R,B: bgColor rect drawn inset from each edge (margin so the fill doesn't reach the corners); JSON "bgColorInset": N or [L,T,R,B]
    float bgColorInsetScale{1.0f};                 // screen px per bgColorInset unit (× widget scale): like frameSliceScale, lets the inset be given in IMAGE px of a hi-res frame (e.g. 0.25)
    std::string frameImage;  // optional: 9-slice frame drawn OVER the background image, under the elements
    float frameSlice[4]{0.0f, 0.0f, 0.0f, 0.0f};  // 9-slice cuts L,T,R,B in IMAGE px; all 0 = frameImage stretched
    float frameSliceScale{1.0f};                    // screen px per image px for the borders (× widget scale)
    float frameImageAlpha{1.0f};                    // opacity of frameImage, × the image's own alpha
    std::string font;      // optional: font file (.ttf/.otf) for this widget's text; empty = engine default. Nested widgets inherit it unless they set their own
    float refHeight{0.0f}; // design window height (px). >0 → text sizes × (window height / refHeight), so text follows resizes like the box does. 0 = fixed size. Nested widgets inherit it unless they set their own
    Color borderColor{1.0f, 1.0f, 1.0f, 1.0f};
    float borderWidth{0.0f};                       // 0 → no border
    std::string cursor;    // cursor name to set when mouse is anywhere within widget bounds
    UICacheMode cacheMode{UICacheMode::Single};   // JSON "cache" (see top of file)
    int maxVariants{64};                          // "variants": max cached images before wiping them all

    // Runtime cache — not persisted, filled lazily on first dirty render
    mutable GLuint cacheFBO     {0};
    mutable GLuint cacheTexture {0};
    mutable int    cacheRW      {0};
    mutable int    cacheRH      {0};
    mutable int    cacheWinW    {0};   // window size the cache was laid out at (layout follows the window)
    mutable int    cacheWinH    {0};
    mutable bool   cacheDirty   {true};
    mutable std::string cachedSig;     // "single": signature of the fixed data in the cache

    // "variants": atlas (texture of the target layer's px, ≤ VARIANT_ATLAS_SIZE) + one cell per variant
    struct VariantCell {
        int ax{0}, ay{0}, w{0}, h{0};  // cell in the atlas (target px, ay from the top of the atlas)
        int offX{0}, offY{0};          // cell top-left relative to the widget origin (target px)
        float contentH{0.0f};          // content height (window px) → lastContentH
        bool  incomplete{false};       // baked while an image was still loading → re-baked in place
    };
    mutable GLuint variantFBO{0};
    mutable GLuint variantTex{0};
    mutable int    variantAW{0}, variantAH{0};           // atlas size
    mutable int    variantRW{0}, variantRH{0};           // target layer size it was built for
    mutable int    variantWinW{0}, variantWinH{0};       // window size it was laid out at
    mutable int    shelfX{0}, shelfY{0}, shelfH{0};      // shelf packer cursor
    mutable bool   variantsDirty{true};                  // wipe all cells on next draw
    mutable int    variantWipes{0};                      // times it overflowed (log + editor)
    mutable std::unordered_map<std::string, VariantCell> variants;

    // Elements already scaled (buildScaled copies every element): kept while the scale, text scale and
    // box size are the same and nobody edited elements (UIManager::markElementsEdited)
    mutable std::vector<UIElement> scaledEls;
    mutable float    scaledKey[4]{-1.0f, -1.0f, -1.0f, -1.0f};
    mutable uint64_t scaledSerial{0};
    mutable int      scaledInUse{0};
};

// One copy for UIManager::drawWidgetInstances
struct UIWidgetInstance {
    float x{0.0f}, y{0.0f};   // window px (like drawWidgetAtPos)
    float alpha{1.0f};        // × global alpha
    UIWidgetRenderData data;
};

// drawWidgetInstances result: copy indexes (0-based, -1 = none) + element id (button) that got it
struct UIInstancesResult {
    int clicked{-1}, rightClicked{-1}, hovered{-1};
    std::string clickedId, rightClickedId, hoveredId;
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
    // Fuerza el re-render de la caché en el siguiente draw ("single": su FBO; "variants": vacía todas
    // las variantes). Solo widgets de primer nivel: los anidados se hornean con su padre.
    bool invalidateWidget(const std::string& name);
    void invalidateAllWidgets();
    // UI theme: images whose path goes through ".../UI/SLOTS/" are loaded from ".../UI/SLOTS_THEMES/<theme>/" instead
    // (same file names in every theme folder). "" = no redirection. Live: drops the image pointers and rebakes the
    // cacheable widgets.
    void setUITheme(const std::string& theme);
    const std::string& getUITheme() const { return uiTheme; }
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

    // N copies of one widget in a single call, in list order (the last one on top), all in the same UI
    // batch. Works with any cache mode; with "variants" each copy is one atlas quad + its live elements.
    // hovered = the topmost copy whose drawn area contains the mouse (for picking without buttons).
    UIInstancesResult drawWidgetInstances(const std::string& name, const std::vector<UIWidgetInstance>& instances,
                                          const std::string& fb = "foreground", float scaleOverride = 0.0f);
    // Lua: instances = { {x=, y=, alpha=?, data={...}}, ... }. Returns 1-based indexes (0 = none):
    // clickedIdx, clickedId, rightClickedIdx, rightClickedId, hoveredIdx, hoveredId
    std::tuple<int, std::string, int, std::string, int, std::string> drawWidgetInstancesLua(
        const std::string& name, sol::table instances, const std::string& fb = "foreground", float scaleOverride = 0.0f);

    // Cache problems of a widget (see LIMITACIONES at the top of this file), one line each; empty = ok.
    // Logged once on the first cached draw; the editor lists them.
    std::vector<std::string> validateWidgetCache(const std::string& name);
    [[nodiscard]] static const char* cacheModeName(UICacheMode m);
    [[nodiscard]] static UICacheMode cacheModeFromName(const std::string& s);
    static void freeVariantAtlas(UIWidget& w);   // GL atlas + cells of a "variants" widget
    // Element definitions may have changed (the UI editor calls it every frame its window is open):
    // the scaled copies (UIWidget::scaledEls) are rebuilt on the next draw. Without it they are only
    // rebuilt when the scale, text scale or window size change -- copying every UIElement of every
    // widget each frame cost ~1 ms with the RTS bottom bar (2026-10-10).
    void markElementsEdited() { elementsGeneration++; }

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
    std::string uiTheme;                                           // "" = sin tema (carpeta SLOTS tal cual)
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

    // ── Cache: live elements, variants ─────────────────────────────────────
    static constexpr int VARIANT_ATLAS_SIZE = 1024;   // max atlas side (target px)
    // Which elements a render pass touches: All; SkipLive (baking the cache: live ones are left out);
    // OnlyLive (over the cached image: live ones drawn, the rest only hit-tested if liveHitTest)
    enum class LivePass { All, SkipLive, OnlyLive };
    LivePass livePass{LivePass::All};
    bool liveHitTest{false};
    bool suppressInput{false};      // baking/measuring a cache: no clicks, hover, sounds, tooltips, wheel
    bool measuringBounds{false};    // accumulate drawn bounds (window px) into mb*
    float mbx0{0}, mby0{0}, mbx1{0}, mby1{0};
    bool  lastDrawnValid{false};    // area of the last top-level renderWidgetAt (window px) → instances hover
    float ldx0{0}, ldy0{0}, ldx1{0}, ldy1{0};
    uint64_t elementsGeneration{1}; // bumped by markElementsEdited(): scaled elements are rebuilt
    std::unordered_map<std::string, bool> cacheValidated;   // widget → problems already logged

    // ── Data of nested widgets ─────────────────────────────────────────────
    // childData(parent, "id_") = the parent's entries under that prefix, without it (what a nested
    // widget / array item / scroll receives). Before, filterDataByPrefix scanned and copied the whole
    // parent map for every nested widget, twice (image and text passes) and for every array item:
    // with the RTS bottom bar (~21 nested slots, ~95 entries) that was most of its ~1.6 ms.
    // Memoized per top-level draw (cleared when renderWidgetAt starts at depth 0) and only for STABLE
    // sources -- the top-level data and memo results, alive for the whole draw. Any other map (a
    // temporary: pager data, an empty fallback) is filtered as before into childScratch, so a reused
    // address can never return another map's entries. Stable sources are sorted once per draw and
    // each prefix is a binary-searched range.
    using UIDataEntry = std::pair<const std::string, UIElementData>;
    const UIWidgetRenderData& childData(const UIWidgetRenderData& parent, const std::string& prefix);
    std::unordered_set<const UIWidgetRenderData*> stableData;
    std::unordered_map<const UIWidgetRenderData*, std::vector<const UIDataEntry*>> sortedData;
    std::unordered_map<std::string, UIWidgetRenderData> childMemo;   // node-based: references stay valid
    std::deque<UIWidgetRenderData> childScratch;                     // deque: push_back keeps references valid
    void beginTopLevelDraw(const UIWidgetRenderData& data);

    // Nested widgets/arrays: the parent's image pass draws only the child's image pass (and its
    // hit-tests), the parent's text pass only the child's text pass. Before (until 2026-10-10) both
    // parent passes drew the WHOLE child: every nested widget was drawn twice (four times two levels
    // down) -- half of the RTS bottom bar's cost, and translucent nested images/rects came out more
    // opaque than their JSON. Set by renderElement right before the call; renderWidgetAt reads and
    // clears it on entry, so it only affects that direct child (pager, scroll, tooltip: whole).
    enum class NestedPass { Both, ImagesOnly, TextOnly };
    NestedPass pendingNestedPass{NestedPass::Both};

    void renderElementFiltered(const UIElement& scaled, float baseX, float baseY, const UIElementData& data);
    void accumulateBounds(const UIElement& sc, float baseX, float drawY, const UIElementData& data, float hDyn);
    void unionBounds(float x0, float y0, float x1, float y1);
    bool widgetHasLive(const UIWidget& w, int depth = 0) const;
    bool isLiveDataKey(const UIWidget& w, const std::string& key, int depth = 0) const;
    std::string fixedDataSignature(const UIWidget& w, const UIWidgetRenderData& data) const;
    bool ensureVariantAtlas(UIWidget& w, int rW, int rH);   // false = can't create
    bool allocVariantCell(UIWidget& w, int cw, int ch, int& ax, int& ay);
    void wipeVariants(UIWidget& w);
    void clearVariantCell(UIWidget& w, const UIWidget::VariantCell& c);   // transparent again, to re-bake it
    // Set while baking a cache if an image it draws is still loading (Image loads in a thread job):
    // that cache is re-baked on the next draw. Before (2026-10-06) the first bake missed the image
    // and, with the same data, the cache never redid it (frames/icons missing from cached panels).
    bool bakeMissingImage{false};
    std::vector<std::string> collectCacheProblems(const UIWidget& w, const std::string& prefix, int depth) const;
    void drawImageCached(const std::string& path, int x, int y, int w, int h, float alpha,
                         bool flipX = false, float tintR = 1.0f, float tintG = 1.0f, float tintB = 1.0f);
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
