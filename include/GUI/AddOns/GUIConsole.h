//
// Created by eduardo on 4/10/23.
//

#ifndef BRAKEZA3D_GUICONSOLE_H
#define BRAKEZA3D_GUICONSOLE_H

#include <cctype>
#include <cstdio>
#include "imgui.h"
#include "../GUI.h"
#include "../../../sol/sol.hpp"

class Brakeza;

struct GuiAddonConsole
{
    char                  InputBuf[256];
    ImVector<char*>       Items;
    ImVector<const char*> Commands;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImGuiTextFilter       Filter;
    bool                  AutoScroll;
    bool                  ScrollToBottom;
    GUIType::Sheet              LogIcon;

    sol::state &lua;

    GuiAddonConsole(sol::state &lua);
    ~GuiAddonConsole();
    void setLogIcon(GUIType::Sheet icon);

    void ClearLog();
    void AddLog(const char* fmt, ...);
    void Draw();
    void ExecCommand(const char* command_line, sol::state &lua);
    int TextEditCallback(ImGuiInputTextCallbackData* data);
    static int TextEditCallbackStub(ImGuiInputTextCallbackData* data);
    // Portable helpers
    static int   Stricmp(const char* s1, const char* s2)         { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
    static int   Strnicmp(const char* s1, const char* s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
    static char* Strdup(const char* s)                           { IM_ASSERT(s); size_t len = strlen(s) + 1; void* buf = malloc(len); IM_ASSERT(buf); return (char*)memcpy(buf, (const void*)s, len); }
    static void  Strtrim(char* s)                                { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }
};
#endif //BRAKEZA3D_GUICONSOLE_H
