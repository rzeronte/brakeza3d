#ifndef BRAKEZA3D_STOREMANAGER_H
#define BRAKEZA3D_STOREMANAGER_H


#include <vector>
#include "src/items/StoreItem.h"
#include "../include/2D/TextWriter.h"
#include "Player.h"

class StoreManager {
private:
    std::vector<StoreItem*> items;
    int offsetX;
    int offsetY;
    TextWriter *writer;
    Image *itemSelector;
    Image *itemBought;

    int currentItemIndex;
    Player *player;
public:
    explicit StoreManager(Player *player, TextWriter *writer);

    void update(float alpha);
    void addItem(StoreItem *item);
    void drawItem(StoreItem *item, int x, int y, float alpha);
    void increaseItemSelected();
    void decreaseItemSelected();

    void drawItemDetails(StoreItem *item, float alpha);

    void buyCurrentSelected();

    void loadDefaultItems();

    void drawHUD(float alpha);

    bool isItemEnabled(int index);
};


#endif //BRAKEZA3D_STOREMANAGER_H
