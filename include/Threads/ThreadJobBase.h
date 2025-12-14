#ifndef BRAKEZA3D_JOB_H
#define BRAKEZA3D_JOB_H
#include <functional>

class ThreadJobBase {
public:
    std::function<void()> function;
    std::function<void()> callback;

    ThreadJobBase() = default;

    ThreadJobBase(std::function<void()> func, std::function<void()> cb)
    :
        function(std::move(func)),
        callback(std::move(cb))
    {
    }
};


#endif //BRAKEZA3D_JOB_H