//
// Created by eduardo on 31/05/23.
//

#ifndef BRAKEZA3D_ENEMYDIALOG_H
#define BRAKEZA3D_ENEMYDIALOG_H


#include "../enemies/EnemyGhost.h"

class EnemyDialog : public Object3D {
    int x;
    int y;
    float staminaPercentage;

    std::string message;
    Counter counter;
    EnemyGhost *enemy;

    Image *background;

    bool followEnemy;
public:
    EnemyDialog(int x, int y, float staminaPercentage, const char *message, float ttl, EnemyGhost *enemy);

    void onUpdate() override;

    void postUpdate() override;

    void drawCall() override;

    void onDraw() override;

    void drawDialog();
};


#endif //BRAKEZA3D_ENEMYDIALOG_H
