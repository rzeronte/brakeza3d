//
// Created by eduardo on 5/06/23.
//

#ifndef BRAKEZA3D_SHADERZOMBIE_H
#define BRAKEZA3D_SHADERZOMBIE_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Render/MeshOpenCLRenderer.h"
#include "../../../include/Objects/Mesh3D.h"
#include "../../../include/Misc/Counter.h"

class ShaderZombie : public ShaderOpenCL {
    MeshOpenCLRenderer *meshOpenCLRenderer;
    Mesh3D* object;
    int offsetX;
    int offsetY;
    Counter counter;
    Image image;
public:
    ShaderZombie(bool active, const std::string& filename, Mesh3D* object, MeshOpenCLRenderer *renderer);

    void update() override;

    void updateOffsets();
};


#endif //BRAKEZA3D_SHADERZOMBIE_H
