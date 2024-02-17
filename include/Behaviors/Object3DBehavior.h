//
// Created by eduardo on 19/3/22.
//

#ifndef BRAKEDA3D_ENEMYBEHAVIOR_H
#define BRAKEDA3D_ENEMYBEHAVIOR_H


#include "../Misc/Timer.h"
#include "../Objects/Vertex3D.h"

class Object3DBehavior {
    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    Timer timer;
    bool enabled;
protected:
    float executionTime = 0;
public:
    Object3DBehavior();

    virtual void onUpdate(Vertex3D &position);

    virtual Object3DBehavior *clone() = 0;

    void resetTimer();

    void setEnabled(bool value);

    virtual void setStartPosition(Vertex3D &position)  = 0;

    [[nodiscard]] float getExecutionTime() const;

    [[nodiscard]] bool isEnabled() const;

    void resetExecutionTime();
};



#endif //BRAKEDA3D_ENEMYBEHAVIOR_H
