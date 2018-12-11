#include <string>
#include "../../headers/Objects/Line2D.h"
#include "../../headers/Render/Color.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Render/EngineBuffers.h"

Line2D::Line2D()
{
    this->x1 = 0;
    this->y1 = 0;
    this->x2 = 0;
    this->y2 = 0;
}

Line2D::Line2D(int x1, int y1, int x2, int y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
}