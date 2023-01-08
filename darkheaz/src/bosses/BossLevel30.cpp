#include "BossLevel30.h"
#include "../../../include/ComponentsManager.h"
#include "../../../include/Brakeza3D.h"

BossLevel30::BossLevel30()
{
    setBossState(BossLevel30State::ONE);
    setBehavior(nullptr);
    setEnableLights(false);
    counter = new Counter(8);
    counter->setEnabled(true);
    //Brakeza3D::get()->addObject3D(enemy, enemy->getLabel());
}

void BossLevel30::onUpdate()
{
    BossEnemy::onUpdate();

}

BossLevel30State BossLevel30::getBossState() const {
    return state;
}

void BossLevel30::setBossState(BossLevel30State value) {
    BossLevel30::state = value;
}

void BossLevel30::load()
{
}

void BossLevel30::setRandomBossState() {

    int option = Tools::random(0, 2);

    switch(option) {
        case ONE: {
            setBossState(ONE);
            break;
        };
        case TWO: {
            setBossState(TWO);
            break;
        }
        case THREE: {
            setBossState(THREE);
            break;
        }
    }

}

