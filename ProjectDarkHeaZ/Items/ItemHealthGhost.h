#ifndef BRAKEDA3D_ITEMHEALTHGHOST_H
#define BRAKEDA3D_ITEMHEALTHGHOST_H


#include "../../include/Physics/Mesh3DGhost.h"
#include "ItemWithTutorial.h"

class ItemHealthGhost : public Mesh3DGhost, public ItemWithTutorial  {
    float aid;
public:
    ItemHealthGhost();

    [[nodiscard]] float getAid() const;

    void onUpdate() override;
};


#endif //BRAKEDA3D_ITEMHEALTHGHOST_H
