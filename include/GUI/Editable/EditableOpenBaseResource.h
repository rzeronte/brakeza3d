//
// Created by Eduardo on 04/01/2026.
//

#ifndef BRAKEZA3D_EDITABLEOPENRESOURCE_H
#define BRAKEZA3D_EDITABLEOPENRESOURCE_H
#include <string>


class EditableOpenBaseResource
{
    std::string tabLabel;
    std::string path;
    bool removed = false;

public:
    explicit EditableOpenBaseResource(const std::string &tabLabel, const std::string &path);

    [[nodiscard]] std::string getTabLabel() const   { return tabLabel; }
    [[nodiscard]] std::string getPath() const       { return path; }
    [[nodiscard]] bool isRemoved() const            { return removed; }

    void setTabLabel(const std::string &tab_label)  { tabLabel = tab_label; }
    void setRemoved(bool value)                     { removed = value; }

    virtual void DrawTabEdition(int i) = 0;
    virtual ~EditableOpenBaseResource();
};


#endif //BRAKEZA3D_EDITABLEOPENRESOURCE_H
