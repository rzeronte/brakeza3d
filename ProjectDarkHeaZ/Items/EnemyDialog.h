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

    TextWriter *writer;

    Image *background;
    Image *degradateBottom;
    Image *avatar;

    Counter counterTTL;
    bool empty;
public:
    EnemyDialog(Object3D *parent, Image *avatar, float staminaPercentage, const char *message, const char *sound,const char *from);

    explicit EnemyDialog();

    void update();

    void drawDialog(float alpha);

    [[nodiscard]] bool isShowed() const;

    void setShowed(bool showed);

    float staminaPercentage;

    void start();

    void updateOffsets();


    void setMessage(const std::string &message);

    void setFrom(const std::string &from);

    void setAvatar(Image *avatar);

    virtual ~EnemyDialog();

    bool isEmpty() const;

    void setEmpty(bool empty);

    void setStaminaPercentage(float staminaPercentage);

    void setSound(const std::string &sound);

    void setStep(float step);
};


#endif //BRAKEZA3D_ENEMYDIALOG_H
