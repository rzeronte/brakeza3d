//
// Created by eduardo on 2/07/23.
//

#include "StoreItem.h"

StoreItem::StoreItem(Image *image, Image *imageSmall, Image *imageOff, Image *description, int cost, Weapon *weapon, bool available)
:
    image(image),
    imageSmall(imageSmall),
    imageOff(imageOff),
    description(description),
    cost(cost),
    weapon(weapon),
    available(available),
    bought(false)
{

}

Image *StoreItem::getImage() const {
    return image;
}


int StoreItem::getCost() const {
    return cost;
}

Weapon *StoreItem::getWeapon() const {
    return weapon;
}

bool StoreItem::isAvailable() const {
    return available;
}

Image *StoreItem::getDescription() const {
    return description;
}

Image *StoreItem::getImageOff() const {
    return imageOff;
}

bool StoreItem::isBought() const {
    return bought;
}

void StoreItem::setBought(bool bought) {
    StoreItem::bought = bought;
}

Image *StoreItem::getImageSmall() const {
    return imageSmall;
}
