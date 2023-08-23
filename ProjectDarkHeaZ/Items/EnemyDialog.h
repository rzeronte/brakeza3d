//
// Created by eduardo on 31/05/23.
//

#ifndef BRAKEZA3D_ENEMYDIALOG_H
#define BRAKEZA3D_ENEMYDIALOG_H


#include "../../include/2D/TextWriter.h"
#include "../../include/Misc/Image.h"
#include "../../include/2D/TextureAnimated.h"
#include "../../include/Objects/Object3D.h"

class EnemyDialog {
    int offsetX;
    int offsetY;

    float smoothEffect;

    bool showed;
    Object3D *parent;
    std::string message;
    std::string sound;
    std::string from;

    Image *background;

    Image *degradateBottom;

    TextWriter *writer;
    Image *avatar;
    Image *avatarSmall;

    Counter counterTTL;
public:
    EnemyDialog(Object3D *parent, Image *avatar, Image * avatarSmall, float staminaPercentage, const char *message, const char *sound,const char *from, TTF_Font *font);

    void update();

    void drawDialog(float alpha);

    [[nodiscard]] bool isShowed() const;

    void setShowed(bool showed);

    float staminaPercentage;

    void start();

    void updateOffsets();

    virtual ~EnemyDialog();
};


#endif //BRAKEZA3D_ENEMYDIALOG_H
