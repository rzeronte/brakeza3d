#ifndef BRAKEDA3D_PARALLELLS_H
#define BRAKEDA3D_PARALLELLS_H


void ParallellDrawTileTriangles(int i, std::vector<Triangle *> *visibleTriangles) {
    ComponentsManager::get()->getComponentRender()->drawTileTriangles(i, *visibleTriangles);
}

#endif //BRAKEDA3D_PARALLELLS_H
