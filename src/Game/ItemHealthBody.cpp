//
// Created by darkhead on 2/1/20.
//

#include "../../headers/Game/ItemHealthBody.h"

void ItemHealthBody::setAid(float aid) {
    ItemHealthBody::aid = aid;
}

ItemHealthBody::ItemHealthBody() : aid(25) {

}

float ItemHealthBody::getAid() const {
    return aid;
}
