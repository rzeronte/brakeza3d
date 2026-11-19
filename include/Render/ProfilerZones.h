/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_PROFILERZONES_H
#define BRAKEZA3D_PROFILERZONES_H

// Zonas para el profiler externo Tracy (https://github.com/wolfpld/tracy).
// Solo se compilan con -DBRAKEZA_TRACY=ON en CMake; sin esa opción todas las macros quedan vacías
// y no hay coste ni dependencia. Complementa a Profiler (el panel interno): éste da la línea de
// tiempo por frame y por hilo en la aplicación Tracy.

#ifdef BZ_TRACY
    #include <GL/glew.h>
    #include <tracy/Tracy.hpp>
    #include <tracy/TracyC.h>
    #include <tracy/TracyOpenGL.hpp>

    #define BZ_FRAME_MARK()        FrameMark
    #define BZ_ZONE()              ZoneScoped
    #define BZ_ZONE_N(name)        ZoneScopedN(name)
    // Nombre calculado en tiempo de ejecución (std::string)
    #define BZ_ZONE_DYN(str)       ZoneScoped; const std::string bzZoneName_ = (str); ZoneName(bzZoneName_.c_str(), bzZoneName_.size())
    #define BZ_THREAD_NAME(name)   tracy::SetThreadName(name)
    #define BZ_PLOT(name, value)   TracyPlot(name, value)
    // GPU (OpenGL): el contexto se crea una vez, con el contexto GL ya activo y GLEW inicializado, y
    // BZ_GPU_COLLECT() se llama una vez por frame después de SwapWindow para recoger los resultados.
    #define BZ_GPU_CONTEXT()       TracyGpuContext
    #define BZ_GPU_COLLECT()       TracyGpuCollect
#else
    #define BZ_FRAME_MARK()
    #define BZ_ZONE()
    #define BZ_ZONE_N(name)
    #define BZ_ZONE_DYN(str)
    #define BZ_THREAD_NAME(name)
    #define BZ_PLOT(name, value)
    #define BZ_GPU_CONTEXT()
    #define BZ_GPU_COLLECT()
#endif

#endif //BRAKEZA3D_PROFILERZONES_H
