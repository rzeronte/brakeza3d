//
// Created by Eduardo on 04/01/2026.
//

#ifndef BRAKEZA3D_EDITABLEOPENRESOURCE_H
#define BRAKEZA3D_EDITABLEOPENRESOURCE_H
#include <string>

#include "../GUI.h"

enum EditableOpenResourceWinType {
    TAB = 0,
    POPUP = 1
};

class EditableOpenBaseResource
{
    std::string uniqueId;
    std::string tabLabel;
    std::string path;
    bool removed = false;
    EditableOpenResourceWinType winType = TAB;

public:
    EditableOpenBaseResource(const std::string &uniqueId, const std::string &tabLabel, const std::string &path)
    : uniqueId(uniqueId), tabLabel(tabLabel), path(path) { }

    virtual ~EditableOpenBaseResource() {}
    [[nodiscard]] EditableOpenResourceWinType getWinType() const    { return winType;}
    [[nodiscard]] std::string getUniqueId() const                   { return uniqueId; }
    [[nodiscard]] std::string getTabLabel() const                   { return tabLabel; }
    [[nodiscard]] std::string getPath() const                       { return path; }
    [[nodiscard]] bool isRemoved() const                            { return removed; }

    void setTabLabel(const std::string &tab_label)                  { tabLabel = tab_label; }
    void setRemoved(bool value)                                     { removed = value; }
    void setWinType(EditableOpenResourceWinType value)              { winType = value; }

    virtual void DrawTabEdition(int i) {};


    void DrawSwitchButton()
    {
        if (getWinType() == TAB) {
            GUI::DrawButton("PopUp mode", IconGUI::NODE_EDITOR_POPUP, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
                setWinType(POPUP);
            });
        } else if (getWinType() == POPUP) {
            GUI::DrawButton("Tab mode", IconGUI::NODE_EDITOR_TAB, GUIType::Sizes::ICONS_CODE_EDITOR, false, [&] {
                setWinType(TAB);
          });
        }
    }
};


#endif //BRAKEZA3D_EDITABLEOPENRESOURCE_H
