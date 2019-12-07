
#ifndef BRAKEDA3D_MENUOPTION_H
#define BRAKEDA3D_MENUOPTION_H


#include <SDL_surface.h>
#include <string>

class MenuOption {

private:
    std::string label;
    std::string alt;
    int action;

public:

    MenuOption(std::string label, int action);

    const std::string &getLabel() const;
    void  setLabel(const std::string &label);

    int  getAction() const;
    void setAction(int action);

    const std::string &getAlt() const;
    void  setAlt(const std::string &alt);
};


#endif //BRAKEDA3D_MENUOPTION_H
