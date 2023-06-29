//
// Created by eduardo on 27/06/23.
//

#ifndef BRAKEZA3D_ITEMWITHTUTORIAL_H
#define BRAKEZA3D_ITEMWITHTUTORIAL_H


class ItemWithTutorial {
    bool hasTutorial = false;
    int tutorialIndex = -1;
public:
    [[nodiscard]] bool isHasTutorial() const;

    void setHasTutorial(bool hasTutorial);

    [[nodiscard]] int getTutorialIndex() const;

    void setTutorialIndex(int tutorialIndex);
};


#endif //BRAKEZA3D_ITEMWITHTUTORIAL_H
