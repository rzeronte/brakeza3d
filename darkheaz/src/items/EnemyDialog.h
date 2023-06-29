//
// Created by eduardo on 31/05/23.
//

#ifndef BRAKEZA3D_ENEMYDIALOG_H
#define BRAKEZA3D_ENEMYDIALOG_H


#include "../../../include/2D/TextWriter.h"
#include "../../../include/Misc/Image.h"
#include "../../../include/2D/TextureAnimated.h"
#include "../../../include/Objects/Object3D.h"

class EnemyDialog : public Object3D {
    int x;
    int y;
    float staminaPercentage;

    std::string message;
    Counter counter;

    Image *background;

    bool followEnemy;

    TextWriter *writer;
    TextureAnimated *radioWave;
    Color color;

public:
    EnemyDialog(int x, int y, float staminaPercentage, const char *message, float ttl, TTF_Font *font, Color color);

    void onUpdate(float stamina);

    void onDraw(Image *avatar, Vertex3D position);

    void drawDialog(float alpha, Image *avatar);
};


#endif //BRAKEZA3D_ENEMYDIALOG_H
