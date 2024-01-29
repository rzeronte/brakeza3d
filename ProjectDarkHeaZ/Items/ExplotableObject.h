//
// Created by edu on 18/01/24.
//

#ifndef BRAKEZA3D_EXPLOTABLEOBJECT_H
#define BRAKEZA3D_EXPLOTABLEOBJECT_H


class ExplotableObject {
    float explodeDamage = 0;

protected:
    bool explode = false;
public:
    [[nodiscard]] bool isExplode() const;

    void setExplode(bool explode);

    [[nodiscard]] float getExplodeDamage() const;

    void setExplodeDamage(float explodeDamage);
};


#endif //BRAKEZA3D_EXPLOTABLEOBJECT_H
