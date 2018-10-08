
#include "../headers/BillboardDirectional.h"
#include "../headers/Core/Logging.h"

BillboardDirectional::BillboardDirectional()
{
    for (int i = 0; i <= 7 ; i++) {
        this->textures[i] = new Texture();
    }
}

void BillboardDirectional::loadTextureDirectional(std::string fileName)
{
    for (int i = 0; i<=7; i++) {
        std::string fullFilename = fileName + "_" + std::to_string(i) + ".png";
        Logging::getInstance()->Log(fullFilename, "LOG");

        this->textures[i]->loadTGA( fullFilename.c_str() );
    }
}

void BillboardDirectional::updateTextureFromCameraAngle(Object3D *o, Camera *cam)
{
    int angle = (int) Tools::getHorizontalAngleBetweenObject3DAndCamera(o, cam);

    if (angle >= 0   && angle < 45)  { this->setTrianglesTexture( this->textures[0] ); }
    if (angle >= 45  && angle < 90)  { this->setTrianglesTexture( this->textures[1] ); }
    if (angle >= 90  && angle < 135) { this->setTrianglesTexture( this->textures[2] ); }
    if (angle >= 135 && angle < 180) { this->setTrianglesTexture( this->textures[3] ); }
    if (angle >= 180 && angle < 225) { this->setTrianglesTexture( this->textures[4] ); }
    if (angle >= 225 && angle < 270) { this->setTrianglesTexture( this->textures[5] ); }
    if (angle >= 270 && angle < 315) { this->setTrianglesTexture( this->textures[6] ); }
    if (angle >= 315 && angle < 360) { this->setTrianglesTexture( this->textures[7] ); }
}
