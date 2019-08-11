
#ifndef BRAKEDA3D_MENUOPTION_H
#define BRAKEDA3D_MENUOPTION_H


#include <SDL_surface.h>
#include <string>

class MenuOption {

public:
    MenuOption(std::string label, std::string fileOn, std::string fileOff);

    std::string label;
    SDL_Surface *image_on;
    SDL_Surface *image_off;
};


#endif //BRAKEDA3D_MENUOPTION_H
