#include "ItemWeaponGhost.h"
#include "../../../include/ComponentsManager.h"

ItemWeaponGhost::ItemWeaponGhost(Weapon *weaponType) : weaponType(weaponType) {
}

Weapon *ItemWeaponGhost::getWeaponType() const {
    return weaponType;
}

void ItemWeaponGhost::onUpdate()
{
    Mesh3D::onUpdate();

    if (!isEnabled()) return;

    if (ComponentsManager::get()->getComponentGame()->getGameState() == EngineSetup::GAMING) {
        this->magnetizableTo(ComponentsManager::get()->getComponentGame()->getPlayer());
    }
}
