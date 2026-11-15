/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by darkh on 19/01/2026.
//

#ifndef BRAKEZA3D_THREADGUI_H
#define BRAKEZA3D_THREADGUI_H
#include "../../Render/ThreadPool.h"


class ThreadGUI {
public:
    static void MenuWorkers();
    static void MenuWorker(ThreadPool &pool, std::string title);
};


#endif //BRAKEZA3D_THREADGUI_H