//
// Created by eduardo on 20/10/23.
//

#include "../../include/Misc/TileManager.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"

TileManager::TileManager(int sizeTileWidth, int sizeTileHeight)
:
    sizeTileWidth(sizeTileWidth),
    sizeTileHeight(sizeTileHeight)
{
    auto SETUP = EngineSetup::get();

    if (SETUP->screenWidth % sizeTileWidth != 0) {
        printf("Bad sizeTileWidth\r\n");
        exit(-1);
    }

    if (SETUP->screenHeight % sizeTileHeight != 0) {
        printf("Bad sizeTileHeight\r\n");
        exit(-1);
    }

    this->numberTilesHorizontal = SETUP->screenWidth / this->sizeTileWidth;
    this->numberTilesVertical = SETUP->screenHeight / this->sizeTileHeight;
    this->numberTiles = numberTilesHorizontal * numberTilesVertical;
    this->tilePixelsBufferSize = this->sizeTileWidth * this->sizeTileHeight;

    for (int y = 0; y < SETUP->screenHeight; y += this->sizeTileHeight) {
        for (int x = 0; x < SETUP->screenWidth; x += this->sizeTileWidth) {

            const int idX = (x / this->sizeTileWidth);
            const int idY = (y / this->sizeTileHeight);

            OCTileRender t(
                true,
                idX,
                idY,
                idY * this->numberTilesHorizontal + idX,
                x,
                y,
                this->sizeTileWidth,
                this->sizeTileHeight
            );

            this->tiles.emplace_back(t);
        }
    }

    printf("TileManager -> Number Tiles: %d | tileSize(%d x %d) | Horizontal: %d | Vertrical: %d\n",
           numberTiles,
           this->sizeTileWidth,
           this->sizeTileHeight,
           this->numberTilesHorizontal,
           this->numberTilesVertical
   );
}
