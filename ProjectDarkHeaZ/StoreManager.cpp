#include "StoreManager.h"
#include "../include/ComponentsManager.h"

StoreManager::StoreManager(Player *player, TextWriter *writer)
:
    player(player),
    writer(writer),
    offsetX(340),
    offsetY(215),
    currentItemIndex(0),
    itemSelector(new Image(EngineSetup::get()->IMAGES_FOLDER + "store/item_selector.png")),
    itemBought(new Image(EngineSetup::get()->IMAGES_FOLDER + "store/item_bought.png"))
{
    loadDefaultItems();
}

void StoreManager::addItem(StoreItem *item)
{
    items.push_back(item);
}

void StoreManager::update(float alpha)
{
    int separation = 70;
    int separationVertical = 70;

    int numRows = 4;
    int numColumns = 4;
    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();

    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numColumns; col++) {
            int offsetX = this->offsetX + col * separation;
            int offsetY = this->offsetY + row * separationVertical;
            int index = row * numColumns + col;

            if (index < items.size()) {
                drawItem(items[index], offsetX, offsetY, alpha);

                if (index == currentItemIndex) {
                    itemSelector->drawFlatAlpha(offsetX, offsetY, alpha, fb);
                    drawItemDetails(items[index], alpha);
                }
            }
        }
    }

    auto game = ComponentsManager::get()->getComponentGame();

    // PLAYER COINS
    auto const offsetYCoins = 130;
    writer->writeTextTTFAutoSize(
        offsetX + 348,
        offsetY + 245,
        (std::string("TOTAL - ")+ std::to_string(game->getPlayer()->getCoins())).c_str(),PaletteColors::getMenuOptions(),
        0.75f
    );
}

void StoreManager::drawItem(StoreItem *item, int x, int y, float alpha)
{
    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();

    if (item->isAvailable()) {
        item->getImage()->drawFlatAlpha(x, y, alpha, fb);

        if (item->isBought()) {
            itemBought->drawFlatAlpha(x, y, alpha, fb);
        }
        return;
    }

    item->getImageOff()->drawFlatAlpha(x, y, alpha, fb);
}

void StoreManager::increaseItemSelected()
{
    if (currentItemIndex + 1 < (int) items.size()) {
        currentItemIndex++;
    }
}

void StoreManager::decreaseItemSelected()
{
    if (currentItemIndex > 0) {
        currentItemIndex--;
    }
}

void StoreManager::drawItemDetails(StoreItem *item, float alpha)
{
    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();

    item->getDescription()->drawFlatAlpha(0, 0, alpha, fb);

    // ITEM COINS
    writer->writeTextTTFAutoSize(
        offsetX + 348,
        offsetY + 215,
        (std::string("Item cost - ") + std::to_string(item->getCost())).c_str(),
        PaletteColors::getStatisticsText(),
        0.75f
    );
}

void StoreManager::buyCurrentSelected()
{
    auto item = items[currentItemIndex];
    if (item->isBought()) {
        return;
    }

    const int cost = item->getCost();

    if ( cost > player->getCoins() || !item->isAvailable()) {
        ComponentsManager::get()->getComponentSound()->sound("error", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        return;
    }

    ComponentsManager::get()->getComponentSound()->sound("selling", EngineSetup::SoundChannels::SND_GLOBAL, 0);
    player->decreaseCoins(cost);
    item->setBought(true);

    switch(item->getId()) {
        case EngineSetup::StoreItems::ITEM_AMMO_WEAPON_PROJECTILES: {
            player->getWeapons()[WEAPON_PROJECTILE]->addAmount(300);
            item->setBought(false);
            break;
        }
        case EngineSetup::StoreItems::ITEM_AMMO_WEAPON_LASER: {
            player->getWeapons()[WEAPON_LASER]->addAmount(500);
            item->setBought(false);
            break;
        }
        case EngineSetup::StoreItems::ITEM_AMMO_WEAPON_RAY: {
            player->getWeapons()[WEAPON_RAYLIGHT]->addAmount(1000);
            item->setBought(false);
            break;
        }
        case EngineSetup::StoreItems::ITEM_AMMO_WEAPON_BOMBS: {
            player->getWeapons()[WEAPON_BOMB]->addAmount(15);
            item->setBought(false);
            break;
        }
        case EngineSetup::StoreItems::ITEM_AMMO_WEAPON_REFLECTIONS: {
            player->getWeapons()[WEAPON_REFLECTION]->addAmount(15);
            item->setBought(false);
            break;
        }
        case EngineSetup::StoreItems::ITEM_AMMO_WEAPON_SHIELDS: {
            player->getWeapons()[WEAPON_SHIELD]->addAmount(25);
            item->setBought(false);
            break;
        }
    }
}

void StoreManager::loadDefaultItems()
{
    const auto folder = EngineSetup::get()->IMAGES_FOLDER + "store/";
    auto weapons = ComponentsManager::get()->getComponentGame()->getPlayer()->getWeapons();

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_EXTRA_POWER,
        new Image(folder + "item1_icon.png"),
        new Image(folder + "item1_icon_small.png"),
        new Image(folder + "item1_icon_off.png"),
        new Image(folder + "item1_desc.png"),
        600,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_FAST_ENERGY_RELOAD,
        new Image(folder + "item2_icon.png"),
        new Image(folder + "item2_icon_small.png"),
        new Image(folder + "item2_icon_off.png"),
        new Image(folder + "item2_desc.png"),
        600,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_SATELLITE,
        new Image(folder + "item3_icon.png"),
        new Image(folder + "item3_icon_small.png"),
        new Image(folder + "item3_icon_off.png"),
        new Image(folder + "item3_desc.png"),
        600,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_MIRROR_SHOOT,
        new Image(folder + "item4_icon.png"),
        new Image(folder + "item4_icon_small.png"),
        new Image(folder + "item4_icon_off.png"),
        new Image(folder + "item4_desc.png"),
        900,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_EXTRA_DASH,
        new Image(folder + "item5_icon.png"),
        new Image(folder + "item5_icon_small.png"),
        new Image(folder + "item5_icon_off.png"),
        new Image(folder + "item5_desc.png"),
        900,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_FAST_SHOOT_CADENCE,
        new Image(folder + "item6_icon.png"),
        new Image(folder + "item6_icon_small.png"),
        new Image(folder + "item6_icon_off.png"),
        new Image(folder + "item6_desc.png"),
        900,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_FAST_ROTATION_TO_ENEMY,
        new Image(folder + "item7_icon.png"),
        new Image(folder + "item7_icon_small.png"),
        new Image(folder + "item7_icon_off.png"),
        new Image(folder + "item7_desc.png"),
        1000,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_LONG_LIVE,
        new Image(folder + "item8_icon.png"),
        new Image(folder + "item8_icon_small.png"),
        new Image(folder + "item8_icon_off.png"),
        new Image(folder + "item8_desc.png"),
        1000,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_AMMO_WEAPON_PROJECTILES,
        new Image(folder + "item9_icon.png"),
        nullptr,
        nullptr,
        new Image(folder + "item9_desc.png"),
        100,
        weapons[WEAPON_PROJECTILE],
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_AMMO_WEAPON_LASER,
        new Image(folder + "item10_icon.png"),
        nullptr,
        nullptr,
        new Image(folder + "item10_desc.png"),
        150,
        weapons[WEAPON_LASER],
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_AMMO_WEAPON_RAY,
        new Image(folder + "item11_icon.png"),
        nullptr,
        nullptr,
        new Image(folder + "item11_desc.png"),
        200,
        weapons[WEAPON_RAYLIGHT],
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_AMMO_WEAPON_BOMBS,
        new Image(folder + "item12_icon.png"),
        nullptr,
        nullptr,
        new Image(folder + "item12_desc.png"),
        250,
        weapons[WEAPON_BOMB],
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_AMMO_WEAPON_REFLECTIONS,
        new Image(folder + "item13_icon.png"),
        nullptr,
        nullptr,
        new Image(folder + "item13_desc.png"),
        300,
        weapons[WEAPON_REFLECTION],
        true
    ));

    addItem(new StoreItem(
        EngineSetup::StoreItems::ITEM_AMMO_WEAPON_SHIELDS,
        new Image(folder + "item14_icon.png"),
        nullptr,
        nullptr,
        new Image(folder + "item14_desc.png"),
        400,
        weapons[WEAPON_SHIELD],
        true
    ));
}

void StoreManager::drawBoughtItemsOnHUD(float alpha)
{
    const int x = 100;
    const int y = 598;
    const int separation = 30;
    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();

    int cont = 0;
    for (auto item: items) {
        if (item->isBought() && item->isAvailable()) {
            item->getImageSmall()->drawFlatAlpha(x + cont * separation, y, alpha, fb);
            cont++;
        }
    }
}

bool StoreManager::isItemEnabled(int index)
{
    return items[index]->isBought();
}

