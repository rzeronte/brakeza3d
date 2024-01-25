#include "StoreManager.h"
#include "../include/ComponentsManager.h"

StoreManager::StoreManager(Player *player, TextWriter *writer)
:
    player(player),
    writer(writer),
    offsetX(340),
    offsetY(230),
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
        this->offsetX,
        this->offsetY + offsetYCoins,
        std::to_string(game->getPlayer()->getCoins()).c_str(),PaletteColors::getMenuOptions(),
        1.0f
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
        offsetY + 200,
        std::to_string(item->getCost()).c_str(),
        PaletteColors::getStatisticsText(),
        1.0f
    );
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

