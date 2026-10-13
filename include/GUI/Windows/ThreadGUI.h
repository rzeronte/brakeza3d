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