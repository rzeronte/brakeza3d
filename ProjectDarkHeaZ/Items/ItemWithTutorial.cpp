//
// Created by eduardo on 27/06/23.
//

#include "ItemWithTutorial.h"

bool ItemWithTutorial::isHasTutorial() const {
    return hasTutorial;
}

void ItemWithTutorial::setHasTutorial(bool hasTutorial) {
    ItemWithTutorial::hasTutorial = hasTutorial;
}

int ItemWithTutorial::getTutorialIndex() const {
    return tutorialIndex;
}

void ItemWithTutorial::setTutorialIndex(int tutorialIndex) {
    ItemWithTutorial::tutorialIndex = tutorialIndex;
}
