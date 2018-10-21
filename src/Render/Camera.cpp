
#include <iostream>
#include "../../headers/Camera.h"
#include "../../headers/Tools.h"
#include "../../headers/Line2D.h"
#include "../../headers/Color.h"
#include "../../headers/EngineSetup.h"
#include "../../headers/Vector3D.h"
#include "../../headers/Drawable.h"
#include "../../headers/Transforms.h"
#include "../../headers/M3.h"
#include "../../headers/Maths.h"


Camera::Camera()
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
        Vertex(0, 0, 1),
        Vertex(0, 1, 0),
        Vertex(1, 0, 0),
        getNearDistance(),
        calcCanvasNearHeight(), calcCanvasNearWidth(),
        farDistance,
        calcCanvasFarHeight(), calcCanvasFarWidth()
    );
}

float Camera::getNearDistance() {
    return (1 / tanf( Maths::degreesToRadians(this->horizontal_fov/2) ));
}

float Camera::getVerticalFOV() {
    float vfov = 2 * atanf( getScreenAspectRatio() / getNearDistance() );

    return Maths::radiansToDegrees( vfov );
}

float Camera::calcCanvasNearWidth() {
    float width = ( 2 * tanf( Maths::degreesToRadians(horizontal_fov / 2) ) * getNearDistance() ) ;

    return width;
}

float Camera::calcCanvasNearHeight() {
    float height = ( 2 * tanf( Maths::degreesToRadians( getVerticalFOV() / 2) ) * getNearDistance() ) * getScreenAspectRatio();

    return height;
}

float Camera::calcCanvasFarWidth() {
    float width = ( 2 * tanf( Maths::degreesToRadians(horizontal_fov / 2) ) * farDistance) ;

    return width;
}

float Camera::calcCanvasFarHeight() {
    float height = (2 * tanf( Maths::degreesToRadians( getVerticalFOV() / 2) ) * farDistance) * getScreenAspectRatio();

    return height;
}

float Camera::getScreenAspectRatio() {
    return this->aspectRatio;
}

void Camera::syncFrustum() {

    frustum->position  = this->position;
    frustum->direction = this->eyeVector();
    frustum->up        = this->upVector();
    frustum->right     = this->rightVector();

    frustum->updateCenters();
    frustum->updatePoints();
    frustum->updatePlanes();
}

Vertex Camera::rightVector() {
    M3 MRX = M3::RX(getRotation()->x);
    M3 MRY = M3::RY(getRotation()->y);
    M3 MRZ = M3::RZ(getRotation()->z);

    Vertex v = (MRZ * MRY * MRX) * Vertex(1, 0, 0);
    v.z = -v.z;

    return v.getNormalize();
}

Vertex Camera::upVector() {
    M3 MRX = M3::RX(getRotation()->x);
    M3 MRY = M3::RY(getRotation()->y);
    M3 MRZ = M3::RZ(getRotation()->z);

    Vertex v = (MRZ * MRY * MRX) * Vertex(0, 1, 0);
    v.z = -v.z;

    return v.getNormalize();
}

Vertex Camera::eyeVector() {
    M3 MRX = M3::RX(getRotation()->x);
    M3 MRY = M3::RY(getRotation()->y);
    M3 MRZ = M3::RZ(getRotation()->z);

    Vertex v = (MRZ * MRY * MRX) * Vertex(0, 0, -1);
    v.z = -v.z;

    return v.getNormalize();
}

void Camera::consoleInfo()
{
    printf("Camera Aspect Ratio: %f | HFOV: %f | VFOV: %f | NearDistance: %f | Canvas W: %f | Canvas H: %f\r\n",
           aspectRatio,
           horizontal_fov,
           getVerticalFOV(),
           getNearDistance(),
           calcCanvasNearWidth(), calcCanvasNearHeight()
    );
}