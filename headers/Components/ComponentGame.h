//
// Created by darkhead on 14/1/20.
//

#ifndef BRAKEDA3D_COMPONENTGAME_H
#define BRAKEDA3D_COMPONENTGAME_H


#include "Component.h"
#include "../../src/Game/Player.h"
#include "../Misc/cJSON.h"
#include "ComponentBSP.h"
#include "../Physics/Mesh3DGhost.h"

class ComponentGame : public Component {
public:
    ComponentGame();

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);

    std::string currentMapName;

    cJSON *mapsJSONList;
    cJSON *weaponsJSONList;
    cJSON *ammoTypesJSONList;
    cJSON *enemiesJSONList;


    Player *player;
    int kills = 0;

    Player *getPlayer() const;

    void onUpdateIA();

    void resolveCollisions();

    void redScreen();

    void setKills(int kills);
    int  getKills() const;

    void LoadMapsFromJSON();
    void LoadWeaponsJSON();
    void LoadEnemiesJSON();

    void setFirstMapNameFromJSON();

    void createObjects3DFromBSPEntities();
    void createMesh3DAndGhostsFromHulls();

    void makeMesh3DGhost(int indexModel, int entityIndex, bool enabled, model_t *hull);
    void makeDoorGhost(int indexModel, int entityIndex, bool enabled, model_t *hull);
    void getTrianglesHull(Mesh3DGhost *mesh, model_t *hull);

};


#endif //BRAKEDA3D_COMPONENTGAME_H
