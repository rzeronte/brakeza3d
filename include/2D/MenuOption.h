
#ifndef BRAKEDA3D_MENUOPTION_H
#define BRAKEDA3D_MENUOPTION_H


#include <SDL2/SDL_surface.h>
#include <string>

class MenuOption {

private:
    std::string label;
    std::string alt;
    int action;

public:

    MenuOption(const std::string& label, int action);

    std::string &getLabel();

    void setLabel(const std::string &label);

    int getAction() const;

    void setAction(int action);

    std::string &getAlt();

    void setAlt(const std::string &alt);
};


#endif //BRAKEDA3D_MENUOPTION_H
