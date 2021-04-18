//
// Created by darkhead on 28/4/18.
//

#ifndef SDL2_3D_ENGINE_COLOR_H
#define SDL2_3D_ENGINE_COLOR_H


#include <SDL2/SDL_quit.h>

class Color {
public:
    Color();
    virtual ~Color();
    static Uint32 white()      { return 0xFFFFFF; }
    static Uint32 red()        { return 0xFF0000; }
    static Uint32 green()      { return 0x00FF00; }
    static Uint32 blue()       { return 0x0000FF; }
    static Uint32 black()      { return 0x000000; }
    static Uint32 yellow()     { return 0xFFFF00; }
    static Uint32 pink()       { return 0xff00bf; }
    static Uint32 cyan()       { return 0x00FFFF; }
    static Uint32 orange()     { return 0xFF8000; }
    static Uint32 magenta()    { return 0xFF00FF; }
    static Uint32 gray()       { return 0x323232; }
    static Uint32 darkwhite()  { return 0xF5F5F5; }
    static Uint32 FOGDefault() { return Color::black(); }

};


#endif //SDL2_3D_ENGINE_COLOR_H
