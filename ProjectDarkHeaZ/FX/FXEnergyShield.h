//
// Created by eduardo on 26/06/23.
//

#ifndef BRAKEZA3D_FXENERGYSHIELD_H
#define BRAKEZA3D_FXENERGYSHIELD_H


#include "../../include/Misc/Image.h"
#include "../../include/OpenGL/FXEffectOpenGL.h"
#include "../../include/Objects/Mesh3D.h"

class FXEnergyShield : public FXEffectOpenGL{
    Image texture;
    Image mask;
    unsigned int maxAlpha;
    Mesh3D *parent;
public:
    FXEnergyShield(bool active, Mesh3D* parent, const std::string &textureFilename, const std::string &maskFilename);

    void executeKernelOpenCL();

    void update() override;

    void setMaxAlpha(unsigned int maxAlpha);
};


#endif //BRAKEZA3D_FXENERGYSHIELD_H