#define SDL_MAIN_HANDLED
#define CL_TARGET_OPENCL_VERSION 120
#define USE_IMGUI_API 1

#include "include/Brakeza.h"

#pragma pack(push, MAIN)
#pragma pack(pop, MAIN)

int main(int argc, char *argv[])
{
    Brakeza::get()->Start(argc, argv);

    return 0;
}