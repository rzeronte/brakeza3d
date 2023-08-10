#include "StoreManager.h"
#include "../include/ComponentsManager.h"

StoreManager::StoreManager(Player *player, TextWriter *writer)
:
    player(player),
    writer(writer),
    offsetX(130),
    offsetY(180),
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
    int separation = 66;
    int separationVertical = 66;

    int numRows = 3;
    int numColumns = 3;

    for (int row = 0; row < numRows; row++) {
        for (int col = 0; col < numColumns; col++) {
            int offsetX = this->offsetX + col * separation;
            int offsetY = this->offsetY + row * separationVertical;
            int index = row * numColumns + col;

            if (index < items.size()) {
                drawItem(items[index], offsetX, offsetY, alpha);

                if (index == currentItemIndex) {
                    itemSelector->drawFlatAlpha(offsetX, offsetY, alpha);
                    drawItemDetails(items[index], alpha);
                }
            }
        }
    }

    auto game = ComponentsManager::get()->getComponentGame();
    ComponentsManager::get()->getComponentHUD()->getHudTextures()->getTextureByLabel("coinIcon")->getImage()->drawFlatAlpha(130, 155, alpha);
    writer->writeTextTTFAutoSize(150, 155, std::to_string(game->getPlayer()->getCoins()).c_str(), game->getPalette().getFirst(), 0.25f);
}

void StoreManager::drawItem(StoreItem *item, int x, int y, float alpha)
{
    if (item->isAvailable()) {
        item->getImage()->drawFlatAlpha(x, y, alpha);

        if (item->isBought()) {
            itemBought->drawFlatAlpha(x, y, alpha);
        }
        return;
    }

    item->getImageOff()->drawFlatAlpha(x, y, alpha);
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
    item->getDescription()->drawFlatAlpha(0, 0, alpha);

    ComponentsManager::get()->getComponentHUD()->getHudTextures()->getTextureByLabel("coinIcon")->getImage()->drawFlatAlpha(345, 323, alpha);
    writer->writeTextTTFAutoSize(365, 323, std::to_string(item->getCost()).c_str(), ComponentsManager::get()->getComponentGame()->getPalette().getSecond(), 0.25f);
}

void StoreManager::buyCurrentSelected()
{
    if (items[currentItemIndex]->isBought()) {
        return;
    }

    const int cost = items[currentItemIndex]->getCost();

    if ( cost > player->getCoins() || !items[currentItemIndex]->isAvailable()) {
        ComponentsManager::get()->getComponentSound()->sound("error", EngineSetup::SoundChannels::SND_GLOBAL, 0);
        return;
    }

    ComponentsManager::get()->getComponentSound()->sound("selling", EngineSetup::SoundChannels::SND_GLOBAL, 0);
    player->decreaseCoins(cost);
    items[currentItemIndex]->setBought(true);
}

void StoreManager::loadDefaultItems()
{
    const auto folder = EngineSetup::get()->IMAGES_FOLDER + "store/";

    addItem(new StoreItem(
        new Image(folder + "item1_icon.png"),
        new Image(folder + "item1_icon_small.png"),
        new Image(folder + "item1_icon_off.png"),
        new Image(folder + "item1_desc.png"),
        500,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        new Image(folder + "item2_icon.png"),
        new Image(folder + "item2_icon_small.png"),
        new Image(folder + "item2_icon_off.png"),
        new Image(folder + "item2_desc.png"),
        500,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        new Image(folder + "item3_icon.png"),
        new Image(folder + "item3_icon_small.png"),
        new Image(folder + "item3_icon_off.png"),
        new Image(folder + "item3_desc.png"),
        1100,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        new Image(folder + "item4_icon.png"),
        new Image(folder + "item4_icon_small.png"),
        new Image(folder + "item4_icon_off.png"),
        new Image(folder + "item4_desc.png"),
        1100,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        new Image(folder + "item5_icon.png"),
        new Image(folder + "item5_icon_small.png"),
        new Image(folder + "item5_icon_off.png"),
        new Image(folder + "item5_desc.png"),
        1100,
        nullptr,
        true
    ));

    addItem(new StoreItem(
        new Image(folder + "item6_icon.png"),
        new Image(folder + "item6_icon_small.png"),
        new Image(folder + "item6_icon_off.png"),
        new Image(folder + "item6_desc.png"),
        1100,
        nullptr,
        true
    ));
}

void StoreManager::drawHUD(float alpha)
{
    int x = 40;
    int y = 15;
    int separation = 30;

    int cont = 0;
    for (auto item: items) {
        if (item->isAvailable() && item->isBought()) {
            item->getImageSmall()->drawFlatAlpha(x + cont * separation, y, alpha);
            cont++;
        }
    }
}

bool StoreManager::isItemEnabled(int index)
{
    return items[index]->isBought();
}

