//
// Created by Eduardo on 04/12/2025.
//

#include "../../include/GUI/GUI.h"

#include "imgui_internal.h"
#include "../../include/Brakeza.h"
#include "../../include/Misc/Tools.h"
#include "../../include/GUI/Objects/FileSystemGUI.h"
#include "../../include/Misc/Logging.h"


// ========== ICON OBJECT DEFINITIONS ==========
namespace IconObject {
    // Definir las variables editables con valores iniciales
    #define DEFINE_ICON(name, x, y) GUIType::Sheet name = {x, y};
        ICON_OBJECT_LIST(DEFINE_ICON)
    #undef DEFINE_ICON

    // Array de metadatos para el editor
    GUIType::IconEntry ALL_ICONS_EDITOR[ICON_COUNT] = {
    #define MAKE_ENTRY(name, x, y) {#name, &name, {x, y}},
        ICON_OBJECT_LIST(MAKE_ENTRY)
        #undef MAKE_ENTRY
    };

    // Función para resetear a valores originales
    void ResetToDefault() {
        for (size_t i = 0; i < ICON_COUNT; i++) {
            *ALL_ICONS_EDITOR[i].icon = ALL_ICONS_EDITOR[i].original;
        }
    }
}

// ========== ICON GUI DEFINITIONS ==========
namespace IconGUI {
    // Definir las variables editables con valores iniciales
    #define DEFINE_ICON(name, x, y) GUIType::Sheet name = {x, y};
        ICON_GUI_LIST(DEFINE_ICON)
    #undef DEFINE_ICON

    // Array de metadatos para el editor
    GUIType::IconEntry ALL_ICONS_EDITOR[ICON_COUNT] = {
    #define MAKE_ENTRY(name, x, y) {#name, &name, {x, y}},
        ICON_GUI_LIST(MAKE_ENTRY)
        #undef MAKE_ENTRY
    };

    // Función para resetear a valores originales
    void ResetToDefault() {
        for (size_t i = 0; i < ICON_COUNT; i++) {
            *ALL_ICONS_EDITOR[i].icon = ALL_ICONS_EDITOR[i].original;
        }
    }
}

void GUI::DrawButton(const std::string &tooltip, GUIType::Sheet icon, ImVec2 size, bool active, const std::function<void()>& onClick)
{
    const auto id = tooltip + std::to_string(icon.x) + "_" + std::to_string(icon.y);
    ImGui::PushID(id.c_str());
    ImGui::PushStyleColor(ImGuiCol_Button, active ? GUIType::Colors::BTN_ON : GUIType::Colors::BTN_OFF);
    if (ImGui::ImageButton(FileSystemGUI::Icon(icon), size)) {
        onClick();
    }
    ImGui::PopStyleColor();
    ImGui::PopID();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    ImGui::SetItemTooltip(tooltip.c_str());
    ImGui::PopStyleVar();
}

void GUI::DrawButtonTransparent(const std::string &tooltip, GUIType::Sheet icon, ImVec2 size, bool active, const std::function<void()>& cb)
{
    const auto id = tooltip + std::to_string(icon.x) + "_" + std::to_string(icon.y);
    ImGui::PushID(id.c_str());

    // Colores transparentes
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));           // Fondo transparente
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 0.1f)); // Ligeramente visible al pasar el ratón
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 0.2f));  // Ligeramente visible al hacer clic

    if (ImGui::ImageButton(FileSystemGUI::Icon(icon), size)) {
        cb();
    }

    ImGui::PopStyleColor(3);
    ImGui::PopID();
    ImGui::SetItemTooltip(tooltip.c_str());
}

void GUI::DrawButtonConfirm(const std::string &title, const std::string &question, GUIType::Sheet icon, const ImVec2 size, const std::function<void()>& cb)
{
    ImGui::PushID((title + question).c_str());
    if (ImGui::ImageButton(FileSystemGUI::Icon(icon), size)) {
        ImGui::OpenPopup(title.c_str());
    }
    ImGui::SetItemTooltip(title.c_str());
    ShowPopUp(title.c_str(), question.c_str(), [&] () {
        cb();
    });
    ImGui::PopID();
}

void GUI::Toggle(bool &value)
{
    value = !value;
}

GUIType::BrowserCache GUI::CreateBrowserCache(std::string folder, const std::string &extension, std::function<void()> functionCallBack)
{
    folder = Tools::NormalizePath(folder);

    GUIType::BrowserCache cache = {
        folder,
        Tools::getFolderFiles(folder, extension),
        Tools::getFolderFolders(folder),
        extension,
        functionCallBack
    };

    return cache;
}

void GUI::ImGuiSetColors()
{
    ImGuiStyle * style = &ImGui::GetStyle();

    style->WindowPadding            = ImVec2(15, 15);
    style->WindowRounding           = 5.0f;
    style->FramePadding             = ImVec2(5, 5);
    style->FrameRounding            = 4.0f;
    style->ItemSpacing              = ImVec2(12, 8);
    style->ItemInnerSpacing         = ImVec2(8, 6);
    style->IndentSpacing            = 25.0f;
    style->ScrollbarSize            = 12.0f;
    style->ScrollbarRounding        = 9.0f;
    style->GrabMinSize              = 5.0f;
    style->GrabRounding             = 3.0f;

    style->TabRounding              = 4.0f;
    style->TabBarBorderSize         = 1.0f;
    style->TabBorderSize            = 0.0f;
    style->WindowBorderSize         = 0.5f;
    style->ChildBorderSize          = 0.5f;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.55f, 0.60f, 1.00f);

    colors[ImGuiCol_WindowBg]               = ImVec4(0.13f, 0.14f, 0.15f, 0.90f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.13f, 0.14f, 0.15f, 0.90f);

    colors[ImGuiCol_PopupBg]                = ImVec4(0.11f, 0.11f, 0.14f, 0.94f);

    colors[ImGuiCol_Border]                 = ImVec4(0.25f, 0.55f, 0.35f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    colors[ImGuiCol_FrameBg]                = ImVec4(0.20f, 0.22f, 0.24f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.35f, 0.65f, 0.45f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.52f, 0.80f, 0.09f, 0.67f);

    colors[ImGuiCol_TitleBg]                = ImVec4(0.15f, 0.20f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.20f, 0.45f, 0.35f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.12f, 0.16f, 0.14f, 1.00f);

    colors[ImGuiCol_Tab]                    = ImVec4(0.13f, 0.18f, 0.15f, 1.00f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.20f, 0.40f, 0.30f, 1.00f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.30f, 0.70f, 0.52f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.10f, 0.14f, 0.12f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.18f, 0.35f, 0.26f, 1.00f);

    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.15f, 0.16f, 0.18f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.11f, 0.11f, 0.11f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.33f, 0.35f, 0.37f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.70f, 0.50f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.52f, 0.80f, 0.09f, 1.00f);

    colors[ImGuiCol_CheckMark]              = ImVec4(0.52f, 0.80f, 0.09f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.35f, 0.65f, 0.45f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.52f, 0.80f, 0.09f, 1.00f);

    colors[ImGuiCol_Button]                 = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.35f, 0.65f, 0.45f, 0.80f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.52f, 0.80f, 0.09f, 1.00f);

    colors[ImGuiCol_Header]                 = ImVec4(0.22f, 0.35f, 0.28f, 0.55f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.35f, 0.65f, 0.45f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.52f, 0.80f, 0.09f, 1.00f);

    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.30f, 0.32f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.35f, 0.65f, 0.45f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.52f, 0.80f, 0.09f, 1.00f);

    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.35f, 0.65f, 0.45f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.35f, 0.65f, 0.45f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.52f, 0.80f, 0.09f, 0.95f);

    colors[ImGuiCol_DockingPreview]         = ImVec4(0.35f, 0.65f, 0.45f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

    colors[ImGuiCol_PlotLines]              = ImVec4(0.35f, 0.65f, 0.45f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.52f, 0.80f, 0.09f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.52f, 0.80f, 0.09f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.35f, 0.65f, 0.45f, 1.00f);

    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.35f, 0.65f, 0.45f, 0.70f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);

    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.35f, 0.65f, 0.45f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.52f, 0.80f, 0.09f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.52f, 0.80f, 0.09f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void GUI::ShowPopUp(const char* title, const char *message, const std::function<void()>& onConfirm)
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text(message);
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            onConfirm();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void GUI::CLIWelcomeMessage()
{
    LOG_MESSAGE("***************************************************************");
    LOG_MESSAGE("*                   B R A K E Z A 3 D                         *");
    LOG_MESSAGE("***************************************************************");
    LOG_MESSAGE("*                                                             *");
    LOG_MESSAGE("*         Open source game engine for developers              *");
    LOG_MESSAGE("***************************************************************");
    LOG_MESSAGE(Config::get()->ENGINE_WEBSITE);
    LOG_MESSAGE(Config::get()->ENGINE_SOURCE_WEBSITE);
    LOG_MESSAGE(Config::get()->ENGINE_TITLE);
    LOG_MESSAGE("***************************************************************");
    LOG_MESSAGE("");
}

void GUI::ShowLoadTime(const std::string &text, const Timer &t)
{
    Brakeza::get()->UpdateTimer();
    LOG_MESSAGE("***************************************************************");
    LOG_MESSAGE("[Brakeza] %s: %f secs", text.c_str(), t.getTotalTime());
    LOG_MESSAGE("***************************************************************");
}

bool GUI::ImageButtonSmall(GUIType::Sheet texture, const std::string &text, const std::function<void()> &onClick)
{
    return AutoImageButton(texture, text.c_str(), GUIType::Sizes::ICON_LOCKS, 18, 6, onClick);
}

bool GUI::ImageButtonNormal(GUIType::Sheet texture, const std::string &text, const std::function<void()> &onClick)
{
    return AutoImageButton(texture, text.c_str(), GUIType::Sizes::ICONS_OBJECTS_ALLOWED, 26, 6, onClick);
}

bool GUI::ImageButtonBig(GUIType::Sheet texture, const std::string &text, const std::function<void()> &onClick)
{
    return AutoImageButton(texture, text.c_str(), GUIType::Sizes::ICONS_CODE_EDITOR, 38, 6, onClick);
}

bool GUI::AutoImageButton(
    GUIType::Sheet texture,
    const char* label,
    const ImVec2& imageSize,
    float height,
    float padding,
    const std::function<void()> &onClick)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    // Calcular ancho automáticamente
    ImVec2 textSize = ImGui::CalcTextSize(label);
    float autoWidth = padding + imageSize.x + padding + textSize.x + padding;

    ImVec2 size(autoWidth, height);
    const ImVec2 pos = window->DC.CursorPos;
    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    // Colores
    ImVec4 bgColor = style.Colors[ImGuiCol_Button];
    if (held)
        bgColor = style.Colors[ImGuiCol_ButtonActive];
    else if (hovered)
        bgColor = style.Colors[ImGuiCol_ButtonHovered];

    // Fondo
    ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(bgColor), true, style.FrameRounding);

    // Imagen
    ImVec2 imagePos = ImVec2(bb.Min.x + padding, bb.Min.y + (size.y - imageSize.y) * 0.5f);

    window->DrawList->AddImage(FileSystemGUI::Icon(texture), imagePos,
                               ImVec2(imagePos.x + imageSize.x, imagePos.y + imageSize.y),
                               ImVec2(0, 0), ImVec2(1, 1),
                               IM_COL32(255, 255, 255, 255)
    );

    // Texto
    ImVec2 textPos = ImVec2(imagePos.x + imageSize.x + padding, bb.Min.y + (size.y - textSize.y) * 0.5f);

    ImGui::RenderText(textPos, label);

    if (pressed) onClick();

    return pressed;
}

void GUI::ImageButtonSmallConfirm(
    GUIType::Sheet texture,
    const std::string &text,
    const std::string &title,
    const std::string &question,
    const std::function<void()> &onConfirm)
{
    ImGui::PushID((title + question + text).c_str());

    bool pressed = AutoImageButton(
        texture,
        text.c_str(),
        GUIType::Sizes::ICON_LOCKS,
        18,
        6,
        [](){}
    );

    if (pressed) {
        ImGui::OpenPopup(title.c_str());
    }

    ShowPopUp(title.c_str(), question.c_str(), [&]() {
        onConfirm();
    });

    ImGui::PopID();
}

void GUI::ImageButtonNormalConfirm(
    GUIType::Sheet texture,
    const std::string &text,
    const std::string &title,
    const std::string &question,
    const std::function<void()> &onConfirm)
{
    ImGui::PushID((title + question + text).c_str());

    bool pressed = AutoImageButton(
        texture,
        text.c_str(),
        GUIType::Sizes::ICONS_OBJECTS_ALLOWED,
        26,
        6,
        [](){}
    );

    if (pressed) {
        ImGui::OpenPopup(title.c_str());
    }

    ShowPopUp(title.c_str(), question.c_str(), [&]() {
        onConfirm();
    });

    ImGui::PopID();
}

void GUI::ImageButtonBigConfirm(
    GUIType::Sheet texture,
    const std::string &text,
    const std::string &title,
    const std::string &question,
    const std::function<void()> &onConfirm)
{
    ImGui::PushID((title + question + text).c_str());

    bool pressed = AutoImageButton(
        texture,
        text.c_str(),
        GUIType::Sizes::ICONS_CODE_EDITOR,
        38,
        6,
        [](){}
    );

    if (pressed) {
        ImGui::OpenPopup(title.c_str());
    }

    ShowPopUp(title.c_str(), question.c_str(), [&]() {
        onConfirm();
    });

    ImGui::PopID();
}