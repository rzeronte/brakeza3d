#include "ItemHealthGhost.h"
#include "../../include/ComponentsManager.h"

ItemHealthGhost::ItemHealthGhost()
:
    aid(25)
{

}

void ItemHealthGhost::onUpdate()
{
    Mesh3D::onUpdate();

    this->magnetizableTo(ComponentsManager::get()->getComponentGame()->getPlayer());
}

float ItemHealthGhost::getAid() const {
    return aid;
}
