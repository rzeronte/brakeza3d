
#include <SDL_image.h>
#include "../../headers/2D/MenuOption.h"

MenuOption::MenuOption(std::string label, std::string fileOn, std::string fileOff)
{
    this->label = label;
    this->image_on = IMG_Load(fileOn.c_str());
    this->image_off = IMG_Load(fileOff.c_str());
}