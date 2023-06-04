//
// Created by eduardo on 31/05/23.
//

#ifndef BRAKEZA3D_ENEMYDIALOG_H
#define BRAKEZA3D_ENEMYDIALOG_H


#include "../enemies/EnemyGhost.h"
#include "../../../include/2D/TextWriter.h"

class EnemyDialog : public Object3D {
    int x;
    int y;
    float staminaPercentage;

    std::string message;
    Counter counter;

    EnemyGhost *enemy;

    Image *background;

    bool followEnemy;

    TextWriter *writer;
    TextureAnimated *radioWave;

public:
    EnemyDialog(int x, int y, float staminaPercentage, const char *message, float ttl, EnemyGhost *enemy, TTF_Font *font);

    void onUpdate() override;

    void postUpdate() override;

    void drawCall() override;

    void onDraw() override;

    void drawDialog(float alpha);
};


#endif //BRAKEZA3D_ENEMYDIALOG_H
