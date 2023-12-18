//
// Created by eduardo on 5/06/23.
//

#include "ShaderZombie.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Brakeza3D.h"

void ShaderZombie::update()
{

}

void ShaderZombie::updateOffsets()
{
    offsetX = Tools::random(5, 10);
    offsetY = Tools::random(5, 10);
}
