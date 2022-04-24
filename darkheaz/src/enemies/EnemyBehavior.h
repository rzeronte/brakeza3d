//
// Created by eduardo on 19/3/22.
//

#ifndef BRAKEDA3D_ENEMYBEHAVIOR_H
#define BRAKEDA3D_ENEMYBEHAVIOR_H


#include "../../../include/Misc/Timer.h"
#include "../../../include/Objects/Vertex3D.h"

class EnemyBehavior {
    float deltaTime = 0;
    float last_ticks = 0;
    float current_ticks = 0;
    float executionTime = 0;
    Timer timer;
public:
    EnemyBehavior();

    virtual ~EnemyBehavior();

    virtual void onUpdate(Vertex3D &position);

    float getExecutionTime() const;

    virtual EnemyBehavior *clone() = 0;

    void resetTimer();
};



#endif //BRAKEDA3D_ENEMYBEHAVIOR_H
