#ifndef BRAKEDA3D_UIMANAGER_H
#define BRAKEDA3D_UIMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>
#include "../../../sol/sol.hpp"
#include "../Color.h"

class ComponentInput;

// Per-element data used by the core renderer (no Lua dependency)
struct UIElementData {
    std::string text      = "Sample";
    Color       color     = {1.0f, 1.0f, 1.0f, 1.0f};
    bool        provided  = false;   // true only when explicitly passed from Lua
    std::string path      = "../assets/images/me.png";
    float       w         = 0;
    float       h         = 0;
    float       value     = 75.0f;
    float       maxValue  = 100.0f;
    std::vector<std::string> iconList;
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
    float wPct{0};   // 0 = fixed px; >0 = fraction of window width  (e.g. 1.0 = 100%)
    float hPct{0};   // 0 = fixed px; >0 = fraction of window height
    std::string alignH{"left"};   // "left" | "center" | "right"
    std::string alignV{"top"};    // "top"  | "center" | "bottom"

    // text
    float fontScale{0.5f};

    // image
    float imageScale{1.0f};

    // button
    Color btnBg     {0.12f, 0.20f, 0.32f, 0.92f};
    Color btnHover  {0.22f, 0.38f, 0.58f, 0.96f};
    Color btnPressed{0.08f, 0.13f, 0.22f, 1.00f};

    // progressbar
    float barW{0}, barH{0};
    float textOffsetX{0};
    float barOffsetY{0};
    float barFontScale{0.338f};
    Color barBg{0.18f, 0.18f, 0.18f, 0.85f};
    Color barOk{0.0f, 0.78f, 0.12f, 1.0f};
    Color barMid{0.82f, 0.55f, 0.0f, 1.0f};
    Color barLow{0.85f, 0.0f, 0.0f, 1.0f};

    // icons
    float iconSize{16}, iconGap{2};
    std::unordered_map<std::string, std::string> iconMapping;
};

struct UIWidget {
    float gap{0};
    float height{0};
    float scale{1.0f};
    std::vector<UIElement> elements;
};

class UIManager {
public:
    void init(ComponentRender* render, const std::string& widgetsDir);
    void loadWidgets();
    void reloadWidgets();
    void clearWidgets();

    // Core renderer — no Lua
    float drawWidget(const std::string& name, float x, float y, const UIWidgetRenderData& data);

    // Lua wrapper — converts sol::table → UIWidgetRenderData and calls core
    // Returns {nextY, clickedButtonId} — clickedButtonId is "" if no button was clicked
    std::tuple<float, std::string> drawWidgetLua(const std::string& name, float x, float y, sol::table data);

    std::unordered_map<std::string, UIWidget>& getWidgets() { return widgets; }
    const std::string& getWidgetsDir() const { return widgetsDir; }

private:
    ComponentRender* render{nullptr};
    ComponentInput*  input{nullptr};
    TextWriter* tw{nullptr};
    std::string widgetsDir;
    std::unordered_map<std::string, UIWidget> widgets;
    std::string lastClickedId;

    void loadWidget(const std::string& filePath);
    void renderElement(const UIElement& el, float baseX, float baseY, const UIElementData& data);
    void renderText(const UIElement& el, float x, float y, const UIElementData& data);
    void renderImage(const UIElement& el, float x, float y, const UIElementData& data);
    void renderRect(const UIElement& el, float x, float y, const UIElementData& data);
    void renderProgressBar(const UIElement& el, float x, float y, const UIElementData& data);
    void renderIcons(const UIElement& el, float x, float y, const UIElementData& data);
    void renderButton(const UIElement& el, float x, float y, const UIElementData& data);
    static float getElementHeight(const UIElement& el);
    void computeWidgetHeight(UIWidget& w);
    static UIElementData solTableToElementData(sol::table t);
};

#endif
