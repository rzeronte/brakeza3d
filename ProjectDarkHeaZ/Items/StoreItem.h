//
// Created by eduardo on 2/07/23.
//

#ifndef BRAKEZA3D_STOREITEM_H
#define BRAKEZA3D_STOREITEM_H


#include "../Weapons/Weapon.h"

class StoreItem {
    int id;
    Image *image;
    Image *imageOff;
    Image *imageSmall;
    Image *description;
    int cost;
    Weapon *weapon;
    bool available;
    bool bought;

public:
    StoreItem(int id, Image *image, Image *imageSmall, Image *imageOff, Image *description, int cost, Weapon *weapon, bool available);

    [[nodiscard]] Image *getImage() const;

    [[nodiscard]] Image *getDescription() const;

    [[nodiscard]] int getCost() const;

    [[nodiscard]] Weapon *getWeapon() const;

    [[nodiscard]] bool isAvailable() const;

    [[nodiscard]] Image *getImageOff() const;

    [[nodiscard]] bool isBought() const;

    [[nodiscard]] Image *getImageSmall() const;

    void setBought(bool bought);

    int getId() const;
};


#endif //BRAKEZA3D_STOREITEM_H
