
#include <iostream>
#include "../../headers/Objects/Camera3D.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Objects/Line2D.h"
#include "../../headers/Render/Color.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Objects/Vector3D.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/M3.h"
#include "../../headers/Render/Maths.h"


Camera3D::Camera3D()
{
    this->pixelPerUnit = 10000.f;

    // Establecemos el FOV horizontal, el FOV vertical va en función del ratio y la nearDistance
    horizontal_fov = 90;
    aspectRatio = ( (float) EngineSetup::getInstance()->SCREEN_HEIGHT / (float) EngineSetup::getInstance()->SCREEN_WIDTH);
    farDistance = 10000;

    this->consoleInfo();

    // Inicializamos el frusutm que acompañará a la cámara
    frustum = new Frustum();
    frustum->setup(
        *getPosition(),
        Vertex3D(0, 0, 1),
        Vertex3D(0, 1, 0),
        Vertex3D(1, 0, 0),
        getNearDistance(),
        calcCanvasNearHeight(), calcCanvasNearWidth(),
        farDistance,
        calcCanvasFarHeight(), calcCanvasFarWidth()
    );
}

float Camera3D::getNearDistance() {
    return (1 / tanf( Maths::degreesToRadians(this->horizontal_fov/2) ));
}

float Camera3D::getVerticalFOV() {
    float vfov = 2 * atanf( getScreenAspectRatio() / getNearDistance() );

    return Maths::radiansToDegrees( vfov );
}

float Camera3D::calcCanvasNearWidth() {
    float width = ( 2 * tanf( Maths::degreesToRadians(horizontal_fov / 2) ) * getNearDistance() ) ;

    return width;
}

float Camera3D::calcCanvasNearHeight() {
    float height = ( 2 * tanf( Maths::degreesToRadians( getVerticalFOV() / 2) ) * getNearDistance() ) * getScreenAspectRatio();

    return height;
}

float Camera3D::calcCanvasFarWidth() {
    float width = ( 2 * tanf( Maths::degreesToRadians(horizontal_fov / 2) ) * farDistance) ;

    return width;
}

float Camera3D::calcCanvasFarHeight() {
    float height = (2 * tanf( Maths::degreesToRadians( getVerticalFOV() / 2) ) * farDistance) * getScreenAspectRatio();

    return height;
}

float Camera3D::getScreenAspectRatio() {
    return this->aspectRatio;
}

void Camera3D::syncFrustum() {

    frustum->position  = this->position;
    frustum->direction = this->eyeVector();
    frustum->up        = this->upVector();
    frustum->right     = this->rightVector();

    frustum->updateCenters();
    frustum->updatePoints();
    frustum->updatePlanes();
}

Vertex3D Camera3D::rightVector() {
    M3 MRX = M3::RX(getRotation()->x);
    M3 MRY = M3::RY(getRotation()->y);
    M3 MRZ = M3::RZ(getRotation()->z);

    Vertex3D v = (MRZ * MRY * MRX) * Vertex3D(1, 0, 0);
    v.z = -v.z;

    return v.getNormalize();
}

Vertex3D Camera3D::upVector() {
    M3 MRX = M3::RX(getRotation()->x);
    M3 MRY = M3::RY(getRotation()->y);
    M3 MRZ = M3::RZ(getRotation()->z);

    Vertex3D v = (MRZ * MRY * MRX) * Vertex3D(0, 1, 0);
    v.z = -v.z;

    return v.getNormalize();
}

Vertex3D Camera3D::eyeVector() {
    M3 MRX = M3::RX(getRotation()->x);
    M3 MRY = M3::RY(getRotation()->y);
    M3 MRZ = M3::RZ(getRotation()->z);

    Vertex3D v = (MRZ * MRY * MRX) * Vertex3D(0, 0, -1);
    v.z = -v.z;

    return v.getNormalize();
}

void Camera3D::consoleInfo()
{
    printf("Camera Aspect Ratio: %f | HFOV: %f | VFOV: %f | NearDistance: %f | Canvas W: %f | Canvas H: %f\r\n",
           aspectRatio,
           horizontal_fov,
           getVerticalFOV(),
           getNearDistance(),
           calcCanvasNearWidth(), calcCanvasNearHeight()
    );
}