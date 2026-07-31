#define SDL_MAIN_HANDLED
#define CL_TARGET_OPENCL_VERSION 120
#define USE_IMGUI_API 1

#include <windows.h>
#include <dbghelp.h>
#include <cstdio>
#include <iostream>
#include "include/Brakeza.h"

#pragma pack(push, MAIN)
#pragma pack(pop, MAIN)

static void PrintStackWithSymbols(void* const* stack, WORD frames)
{
    HANDLE process = GetCurrentProcess();
    static bool symInit = false;
    if (!symInit) {
        SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
        SymInitialize(process, nullptr, TRUE);
        symInit = true;
    }

    constexpr size_t NAME_LEN = 512;
    unsigned char buffer[sizeof(SYMBOL_INFO) + NAME_LEN];
    auto *symbol = reinterpret_cast<SYMBOL_INFO*>(buffer);
    symbol->MaxNameLen   = NAME_LEN - 1;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    IMAGEHLP_LINE64 line = {};
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

    for (WORD i = 0; i < frames; ++i) {
        auto addr = reinterpret_cast<DWORD64>(stack[i]);
        const char* symName = "??";
        DWORD64 disp64 = 0;
        if (SymFromAddr(process, addr, &disp64, symbol)) {
            symName = symbol->Name;
        }

        DWORD disp32 = 0;
        const char* file = nullptr;
        DWORD lineNo = 0;
        if (SymGetLineFromAddr64(process, addr, &disp32, &line)) {
            file = line.FileName;
            lineNo = line.LineNumber;
        }

        HMODULE mod = nullptr;
        char modName[MAX_PATH] = "?";
        if (GetModuleHandleExA(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCSTR)addr, &mod) && mod) {
            GetModuleFileNameA(mod, modName, MAX_PATH);
        }

        std::fprintf(stderr, "  [%02u] %p  %s!%s+0x%llx",
                     (unsigned)i, stack[i], modName, symName, (unsigned long long)disp64);
        if (file) {
            std::fprintf(stderr, "  (%s:%lu)", file, (unsigned long)lineNo);
        }
        std::fputc('\n', stderr);
    }
    std::fflush(stderr);
}

static LONG WINAPI SehHandler(_EXCEPTION_POINTERS* ep)
{
    std::fprintf(stderr, "\n[SEH] Unhandled exception 0x%lx at address %p\n",
                 (unsigned long)ep->ExceptionRecord->ExceptionCode,
                 ep->ExceptionRecord->ExceptionAddress);
    std::fflush(stderr);

    void* stack[64];
    WORD frames = CaptureStackBackTrace(0, 64, stack, nullptr);
    PrintStackWithSymbols(stack, frames);

    return EXCEPTION_CONTINUE_SEARCH;
}

int main(int argc, char *argv[])
{
    SetUnhandledExceptionFilter(SehHandler);

    Brakeza::get()->Start(argc, argv);

    return 0;
}