#define SDL_MAIN_HANDLED
#define CL_TARGET_OPENCL_VERSION 120
#define USE_IMGUI_API 1

#include <windows.h>
#include <dbghelp.h>
#include <cstdio>
#include <iostream>
#include <exception>
#include "include/Brakeza.h"

#pragma pack(push, MAIN)
#pragma pack(pop, MAIN)

static bool g_symInit = false;

static void EnsureSymInit()
{
    if (g_symInit) return;
    SymSetOptions(SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
    SymInitialize(GetCurrentProcess(), nullptr, TRUE);
    g_symInit = true;
}

// CaptureStackBackTrace devolvia 0 frames para un crash dentro de un worker thread del
// ThreadPool (visto persiguiendo un access violation en ThreadJobLoadMesh3DAnimation, direccion
// de dato ~0xFFFFFFFFFFFFFFFF/F0 -- tipico de leer .back()/[size-1] sobre un contenedor vacio).
// CaptureStackBackTrace hace su propio stack walk implicito del hilo que la llama, y con codigo
// -O3 (frame-pointer omission, tail calls) eso puede fallar en devolver nada util. StackWalk64
// con el CONTEXT EXACTO que el propio SEH ya recibio (ep->ContextRecord) es el patron estandar
// y mas fiable para stack walking en el momento exacto de una excepcion.
// PENDIENTE (2026-09-15): en ese mismo caso (crash en worker thread durante carga masiva de
// enemy_unit_N concurrente), NI ESTE metodo consiguio devolver frames -- 0 tambien. No se llego
// a diagnosticar la causa raiz de ESE crash concreto (candidatos sin confirmar: GLSLTypeMapping
// como std::map global sin lock tocado desde varios ThreadJobLoadMesh3DAnimation en paralelo, o
// alguna llamada gl* colandose en la fase "Background" que deberia ser solo CPU-side). Requiere
// depuracion en vivo (breakpoint en CLion) para seguir. Ver [[project_rts]] o memoria de sesion.
static WORD CaptureStackFromContext(const CONTEXT* ctxIn, void** outFrames, WORD maxFrames)
{
    EnsureSymInit();

    HANDLE process = GetCurrentProcess();
    HANDLE thread  = GetCurrentThread();

    CONTEXT ctx = *ctxIn; // StackWalk64 muta el context -- trabajar sobre una copia

    STACKFRAME64 frame = {};
    frame.AddrPC.Offset    = ctx.Rip;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = ctx.Rbp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrStack.Offset = ctx.Rsp;
    frame.AddrStack.Mode   = AddrModeFlat;

    WORD count = 0;
    DWORD64 lastPC = 0;
    while (count < maxFrames) {
        BOOL ok = StackWalk64(
            IMAGE_FILE_MACHINE_AMD64, process, thread, &frame, &ctx,
            nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr
        );
        if (!ok || frame.AddrPC.Offset == 0) break;
        if (frame.AddrPC.Offset == lastPC) break; // salvaguarda contra bucles sin avance
        lastPC = frame.AddrPC.Offset;
        outFrames[count++] = reinterpret_cast<void*>(frame.AddrPC.Offset);
    }
    return count;
}

static void PrintStackWithSymbols(void* const* stack, WORD frames)
{
    EnsureSymInit();
    HANDLE process = GetCurrentProcess();

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

    // Para un 0xC0000005 (EXCEPTION_ACCESS_VIOLATION), ExceptionInformation[0] es
    // 0=lectura/1=escritura y ExceptionInformation[1] es la DIRECCION DE MEMORIA a la que se
    // intento acceder -- no la direccion de codigo que fallo (esa ya se imprime arriba), sino
    // el dato que se intento leer/escribir. Para un UAF sobre un Object3D, esto suele caer
    // dentro (o muy cerca) del bloque ya liberado -- cruzando este valor con los logs de
    // "creado"/"~Object3D" por puntero se puede identificar el objeto sin necesidad de leer
    // su memoria ya corrupta (asi se identifico el UAF de 'rally_marker' en HUDManager.lua).
    if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION &&
        ep->ExceptionRecord->NumberParameters >= 2) {
        std::fprintf(stderr, "[SEH] Access violation %s at data address %p\n",
                     ep->ExceptionRecord->ExceptionInformation[0] ? "WRITE" : "READ",
                     (void*)ep->ExceptionRecord->ExceptionInformation[1]);
    }
    std::fflush(stderr);

    void* stack[64];
    WORD frames = 0;
    if (ep->ContextRecord != nullptr) {
        frames = CaptureStackFromContext(ep->ContextRecord, stack, 64);
    }
    if (frames == 0) {
        // Fallback: el metodo original, por si StackWalk64 tampoco puede (p.ej. sin .pdb/CFI
        // utilizable para ese modulo). Mejor esto que nada.
        frames = CaptureStackBackTrace(0, 64, stack, nullptr);
    }
    PrintStackWithSymbols(stack, frames);

    return EXCEPTION_CONTINUE_SEARCH;
}

// Complementa al SehHandler (ese cubre 0xC0000005 y similares -- violaciones de acceso).
// Este cubre el otro tipo de crash "aleatorio": una excepción de C++ (p.ej. sol::error de un
// error de Lua) que se propaga sin que nadie la capture, terminando en std::terminate() sin más
// info que "terminate called after throwing an instance of 'sol::error'". Reutiliza la misma
// resolución de símbolos que el SEH handler para al menos dar mensaje completo + stack de C++.
static void TerminateHandler()
{
    std::fprintf(stderr, "\n[TERMINATE] Uncaught C++ exception -- capturing details...\n");

    if (std::exception_ptr eptr = std::current_exception()) {
        try {
            std::rethrow_exception(eptr);
        } catch (const std::exception& e) {
            std::fprintf(stderr, "[TERMINATE] typeid: %s\n", typeid(e).name());
            std::fprintf(stderr, "[TERMINATE] what(): %s\n", e.what());
        } catch (...) {
            std::fprintf(stderr, "[TERMINATE] non-std::exception thrown (no what() available)\n");
        }
    } else {
        std::fprintf(stderr, "[TERMINATE] no active exception -- std::terminate() called directly\n");
    }
    std::fflush(stderr);

    void* stack[64];
    WORD frames = CaptureStackBackTrace(0, 64, stack, nullptr);
    PrintStackWithSymbols(stack, frames);

    std::abort();
}

int main(int argc, char *argv[])
{
    SetUnhandledExceptionFilter(SehHandler);
    std::set_terminate(TerminateHandler);

    Brakeza::get()->Start(argc, argv);

    return 0;
}