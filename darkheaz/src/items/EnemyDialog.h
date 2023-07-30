//
// Created by eduardo on 31/05/23.
//

#ifndef BRAKEZA3D_ENEMYDIALOG_H
#define BRAKEZA3D_ENEMYDIALOG_H


#include "../../../include/2D/TextWriter.h"
#include "../../../include/Misc/Image.h"
#include "../../../include/2D/TextureAnimated.h"
#include "../../../include/Objects/Object3D.h"

class EnemyDialog {
    bool showed;
    Object3D *parent;
    std::string message;
    std::string from;

    Image *background;

    TextWriter *writer;
    Color color;
    Image *avatar;
    Image *avatarSmall;
public:
    EnemyDialog(Object3D *parent, Image *avatar, Image * avatarSmall, float staminaPercentage, const char *message, const char *from, TTF_Font *font, Color color);

    void update();

    void drawDialog(float alpha, Image *avatar);

    [[nodiscard]] bool isShowed() const;

    void setShowed(bool showed);

    float staminaPercentage;
};


#endif //BRAKEZA3D_ENEMYDIALOG_H
