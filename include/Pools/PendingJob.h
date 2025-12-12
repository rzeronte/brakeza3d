//
// Created by Eduardo on 11/12/2025.
//

#ifndef BRAKEZA3D_JOB_H
#define BRAKEZA3D_JOB_H
#include <functional>

class PendingJob {
public:
    PendingJob(const std::function<void()> &function, const std::function<void()> &callback)
    :
        function(function),
        callback(callback) {
    }

    std::function<void()> function;
    std::function<void()> callback;
};


#endif //BRAKEZA3D_JOB_H