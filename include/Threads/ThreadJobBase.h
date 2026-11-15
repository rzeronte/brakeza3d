/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef BRAKEZA3D_JOB_H
#define BRAKEZA3D_JOB_H
#include <functional>

class ThreadJobBase {
public:
    std::function<void()> function;
    std::function<void()> callback;

    // Un job puede ponerlo a true dentro de function() para decir "ahora no puedo avanzar sin
    // bloquear el worker" (p.ej. otro hilo está parseando el mismo fichero). El ThreadPool lo
    // reencola al final sin ejecutar su callback. Solo lo respeta enqueueWithMainThreadCallback().
    bool deferred = false;

    ThreadJobBase() = default;
    // Virtual: clase base polimórfica (los ThreadJob* derivan de aquí).
    virtual ~ThreadJobBase() = default;

    ThreadJobBase(std::function<void()> func, std::function<void()> cb)
    :
        function(std::move(func)),
        callback(std::move(cb))
    {
    }
};


#endif //BRAKEZA3D_JOB_H