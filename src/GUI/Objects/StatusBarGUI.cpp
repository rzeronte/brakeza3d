// ============= StatusBarGUI.cpp =============
#include "../../../include/GUI/Objects/StatusBarGUI.h"
#include <cstdarg>

#include "../../../include/Brakeza.h"
#include "../../../include/Components/Components.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/GUI/GUI.h"

void StatusBarGUI::Render(GUIAddonResourceHub* resourceHub)
{
    if (!Config::get()->ENABLE_IMGUI_STATUSBAR) return;

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    DrawGradientLine(viewport, viewport->Pos.y + viewport->Size.y - 2.0f, 2.0f);

    SetupWindow(viewport);
    ApplyStyle();

    if (ImGui::Begin("StatusBar", nullptr, GetWindowFlags())) {
        RenderContent(resourceHub);
    }
    ImGui::End();

    RestoreStyle();
}

void StatusBarGUI::SetupWindow(ImGuiViewport* viewport)
{
    const float status_bar_height = Config::get()->STATUS_BAR_HEIGHT;

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - status_bar_height));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, status_bar_height));
    ImGui::SetNextWindowViewport(viewport->ID);
}

ImGuiWindowFlags StatusBarGUI::GetWindowFlags()
{
    return ImGuiWindowFlags_NoTitleBar |
           ImGuiWindowFlags_NoResize |
           ImGuiWindowFlags_NoMove |
           ImGuiWindowFlags_NoScrollbar |
           ImGuiWindowFlags_NoSavedSettings |
           ImGuiWindowFlags_NoDocking;
}

void StatusBarGUI::ApplyStyle()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 5));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
}

void StatusBarGUI::RestoreStyle()
{
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
}

void StatusBarGUI::RenderContent(GUIAddonResourceHub* resourceHub)
{
    DrawVersionInfo();
    DrawSeparator();
    DrawFPSCounter();
    DrawSeparator();
    DrawSessionStatus(resourceHub);
    DrawSeparator();
    DrawObjectCount();
    DrawSeparator();
    DrawLightsStatus();
    DrawSeparator();
    DrawScriptsStatus();
    DrawSeparator();
    DrawSceneStatus();
    DrawSeparator();
    DrawProjectStatus();
}

void StatusBarGUI::DrawSeparator()
{
    ImGui::SameLine();
    ImGui::Text(" | ");
    ImGui::SameLine();
}

void StatusBarGUI::DrawVersionInfo()
{
    ImGui::Text("Brakeza3D %s", Config::get()->ENGINE_VERSION.c_str());
}

void StatusBarGUI::DrawFPSCounter()
{
    if (!Config::get()->DRAW_FPS_RENDER) return;

    DrawIconWithFormattedText(IconGUI::VIDEO_FPS, "FPS: %.1f", ImGui::GetIO().Framerate);
}

void StatusBarGUI::DrawSessionStatus(GUIAddonResourceHub* resourceHub)
{
    bool authenticated = resourceHub->isAuthenticated();

    auto icon = authenticated ? IconGUI::SESSION_ON : IconGUI::SESSION_OFF;
    auto label = authenticated ? "Signed in" : "Not signed in";

    DrawIconWithText(icon, label);
}

void StatusBarGUI::DrawObjectCount()
{
    DrawIconWithFormattedText(
        IconGUI::MNU_ADD_OBJECT,
        "Num objects: %d",
        Brakeza::get()->getSceneObjects().size()
    );
}

void StatusBarGUI::DrawLightsStatus()
{
    bool lightsEnabled = Config::get()->ENABLE_LIGHTS;
    auto icon = lightsEnabled ? IconGUI::LIGHT_ON : IconGUI::LIGHT_OFF;

    DrawIconWithText(icon, "Lights");
}

void StatusBarGUI::DrawScriptsStatus()
{
    bool isRunning = Components::get()->Scripting()->getStateLUAScripts() != Config::LuaStateScripts::LUA_STOP;

    auto icon = isRunning ? IconGUI::LUA_PLAY : IconGUI::LUA_STOP;
    auto label = isRunning ? "Scripts running..." : "Scripts stopped";

    DrawIconWithText(icon, label);
}

void StatusBarGUI::DrawProjectStatus()
{
    auto project = Components::get()->Scripting()->getCurrentProject();
    auto label = project != nullptr ? project->getFilePath().c_str() : "None";
    DrawIconWithText(IconGUI::PROJECT_FILE, label);
}

void StatusBarGUI::DrawSceneStatus()
{
    auto scene = Components::get()->Scripting()->getCurrentScene();
    auto label = scene != nullptr ? scene->getFilePath().c_str() : "None";
    DrawIconWithText(IconGUI::SCENE_FILE, label);
}

void StatusBarGUI::DrawIconWithText(GUIType::Sheet icon, const char* text)
{
    ImGui::Image(FileSystemGUI::Icon(icon), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    ImGui::Text("%s", text);
}

void StatusBarGUI::DrawIconWithFormattedText(GUIType::Sheet icon, const char* format, ...)
{
    ImGui::Image(FileSystemGUI::Icon(icon), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();

    va_list args;
    va_start(args, format);
    ImGui::TextV(format, args);
    va_end(args);
}


void StatusBarGUI::DrawGradientLine(ImGuiViewport* viewport, float yPosition, float lineHeight)
{
    ImDrawList* drawList = ImGui::GetForegroundDrawList(viewport);

    // Posición de la línea
    const ImVec2 lineStart = ImVec2(viewport->Pos.x, yPosition);
    const ImVec2 lineEnd = ImVec2(viewport->Pos.x + viewport->Size.x, yPosition + lineHeight);

    // Colores lime y cyan
    const ImU32 colorLime = IM_COL32(132, 204, 22, 255);   // lime-500
    const ImU32 colorCyan = IM_COL32(6, 182, 212, 255);    // cyan-500

    // Degradado en 3 segmentos: lime → cyan → lime
    const float width = viewport->Size.x;
    const float segment = width / 3.0f;

    // Segmento izquierdo: lime → cyan
    drawList->AddRectFilledMultiColor(
        ImVec2(lineStart.x, lineStart.y),
        ImVec2(lineStart.x + segment, lineEnd.y),
        colorLime, colorCyan, colorCyan, colorLime
    );

    // Segmento central: cyan puro
    drawList->AddRectFilled(
        ImVec2(lineStart.x + segment, lineStart.y),
        ImVec2(lineStart.x + segment * 2.0f, lineEnd.y),
        colorCyan
    );

    // Segmento derecho: cyan → lime
    drawList->AddRectFilledMultiColor(
        ImVec2(lineStart.x + segment * 2.0f, lineStart.y),
        ImVec2(lineEnd.x, lineEnd.y),
        colorCyan, colorLime, colorLime, colorCyan
    );
}