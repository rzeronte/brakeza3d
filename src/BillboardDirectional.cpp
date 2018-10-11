
#include "../headers/BillboardDirectional.h"
#include "../headers/Core/Logging.h"

#define DIR_S 0
#define DIR_SW 1
#define DIR_W 2
#define DIR_NW 3
#define DIR_N 4
#define DIR_NE 5
#define DIR_E 6
#define DIR_SE 7

BillboardDirectional::BillboardDirectional()
{
    this->state = SPRITE_STATE_STOPPED;

    for (int i = 0; i <= directions ; i++) {
        for (int j = 0; j <= frames; j++) {
            this->frames_stopped[i][j] = new Texture();
            this->frames_walking[i][j] = new Texture();
            this->frames_shooting[i][j] = new Texture();
            this->frames_dying[i][j] = new Texture();
        }
    }
}

void BillboardDirectional::loadSprite(std::string fileName)
{
    this->loadTextureDirectional( fileName, "stopped" );
    this->loadTextureDirectional( fileName, "walking" );
    this->loadTextureDirectional( fileName, "shooting" );
    this->loadTextureDirectional( fileName, "dying" );

}

void BillboardDirectional::loadTextureDirectional(std::string fileName, std::string state)
{
    for (int i = 0; i <= directions; i++) {
        for (int j = 0; j <= frames; j++) {
            std::string fullFilename = EngineSetup::getInstance()->FOLDER_SPRITES_DEFAULT + fileName + "/" + state + "/" + std::to_string(i) + "_" + std::to_string(j) + ".png";
            Logging::getInstance()->Log(fullFilename, "SPRITE");

            if  (state == "stopped") {
                this->frames_stopped[i][j]->loadTGA( fullFilename.c_str() );
            }

            if  (state == "walking") {
                this->frames_walking[i][j]->loadTGA( fullFilename.c_str() );
            }

            if  (state == "shooting") {
                this->frames_shooting[i][j]->loadTGA( fullFilename.c_str() );
            }

            if  (state == "dying") {
                this->frames_dying[i][j]->loadTGA( fullFilename.c_str() );
            }
        }
    }
}

void BillboardDirectional::updateTextureFromCameraAngle(Object3D *o, Camera *cam)
{
    float angle = (int) Tools::getHorizontalAngleBetweenObject3DAndCamera(o, cam);
    int frame = 0;
    this->state = 0;

    switch (this->state) {
        case SPRITE_STATE_STOPPED:
            if (angle >= 0   && angle < 45)  { this->setTrianglesTexture( this->frames_stopped[DIR_S][frame] ); }
            if (angle >= 45  && angle < 90)  { this->setTrianglesTexture( this->frames_stopped[DIR_SW][frame] ); }
            if (angle >= 90  && angle < 135) { this->setTrianglesTexture( this->frames_stopped[DIR_W][frame] ); }
            if (angle >= 135 && angle < 180) { this->setTrianglesTexture( this->frames_stopped[DIR_NW][frame] ); }
            if (angle >= 180 && angle < 225) { this->setTrianglesTexture( this->frames_stopped[DIR_N][frame] ); }
            if (angle >= 225 && angle < 270) { this->setTrianglesTexture( this->frames_stopped[DIR_NE][frame] ); }
            if (angle >= 270 && angle < 315) { this->setTrianglesTexture( this->frames_stopped[DIR_E][frame] ); }
            if (angle >= 315 && angle < 360) { this->setTrianglesTexture( this->frames_stopped[DIR_SE][frame] ); }
            break;
        case SPRITE_STATE_WALKING:
            if (angle >= 0   && angle < 45)  { this->setTrianglesTexture( this->frames_walking[DIR_S][frame] ); }
            if (angle >= 45  && angle < 90)  { this->setTrianglesTexture( this->frames_walking[DIR_SW][frame] ); }
            if (angle >= 90  && angle < 135) { this->setTrianglesTexture( this->frames_walking[DIR_W][frame] ); }
            if (angle >= 135 && angle < 180) { this->setTrianglesTexture( this->frames_walking[DIR_NW][frame] ); }
            if (angle >= 180 && angle < 225) { this->setTrianglesTexture( this->frames_walking[DIR_N][frame] ); }
            if (angle >= 225 && angle < 270) { this->setTrianglesTexture( this->frames_walking[DIR_NE][frame] ); }
            if (angle >= 270 && angle < 315) { this->setTrianglesTexture( this->frames_walking[DIR_E][frame] ); }
            if (angle >= 315 && angle < 360) { this->setTrianglesTexture( this->frames_walking[DIR_SE][frame] ); }
            break;
        case SPRITE_STATE_SHOOTING:
            if (angle >= 0   && angle < 45)  { this->setTrianglesTexture( this->frames_shooting[DIR_S][frame] ); }
            if (angle >= 45  && angle < 90)  { this->setTrianglesTexture( this->frames_shooting[DIR_SW][frame] ); }
            if (angle >= 90  && angle < 135) { this->setTrianglesTexture( this->frames_shooting[DIR_W][frame] ); }
            if (angle >= 135 && angle < 180) { this->setTrianglesTexture( this->frames_shooting[DIR_NW][frame] ); }
            if (angle >= 180 && angle < 225) { this->setTrianglesTexture( this->frames_shooting[DIR_N][frame] ); }
            if (angle >= 225 && angle < 270) { this->setTrianglesTexture( this->frames_shooting[DIR_NE][frame] ); }
            if (angle >= 270 && angle < 315) { this->setTrianglesTexture( this->frames_shooting[DIR_E][frame] ); }
            if (angle >= 315 && angle < 360) { this->setTrianglesTexture( this->frames_shooting[DIR_SE][frame] ); }
            break;
        case SPRITE_STATE_DYING:
            if (angle >= 0   && angle < 45)  { this->setTrianglesTexture( this->frames_dying[DIR_S][frame] ); }
            if (angle >= 45  && angle < 90)  { this->setTrianglesTexture( this->frames_dying[DIR_SW][frame] ); }
            if (angle >= 90  && angle < 135) { this->setTrianglesTexture( this->frames_dying[DIR_W][frame] ); }
            if (angle >= 135 && angle < 180) { this->setTrianglesTexture( this->frames_dying[DIR_NW][frame] ); }
            if (angle >= 180 && angle < 225) { this->setTrianglesTexture( this->frames_dying[DIR_N][frame] ); }
            if (angle >= 225 && angle < 270) { this->setTrianglesTexture( this->frames_dying[DIR_NE][frame] ); }
            if (angle >= 270 && angle < 315) { this->setTrianglesTexture( this->frames_dying[DIR_E][frame] ); }
            if (angle >= 315 && angle < 360) { this->setTrianglesTexture( this->frames_dying[DIR_SE][frame] ); }
            break;
    }


}
