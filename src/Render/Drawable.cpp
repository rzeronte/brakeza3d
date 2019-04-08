
#include "../../headers/Render/Drawable.h"
#include "../../headers/Objects/Line2D.h"
#include "../../headers/Objects/Point2D.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/EngineBuffers.h"
#include "../../headers/Render/Billboard.h"

void Drawable::drawBox2D(SDL_Rect r)
{
    int x = r.x;
    int y = r.y;

    // top
    Line2D l1 = Line2D(x, y, x + r.w, y);
    //left
    Line2D l2 = Line2D(x, y, x, y + r.h);
    //bottom
    Line2D l3 = Line2D(x, y + r.h, x + r.w, y + r.h);
    //right
    Line2D l4 = Line2D(x + r.w, y, x + r.w, y + r.h);

    Drawable::drawLine2D(l1, Color::green());
    Drawable::drawLine2D(l2, Color::green());
    Drawable::drawLine2D(l3, Color::green());
    Drawable::drawLine2D(l4, Color::green());
}

void Drawable::drawFrustum(Frustum *f, Camera3D *cam, bool drawNP, bool drawFP, bool drawSides)
{
    // Center Near Plane
    //Drawable::drawVertex( f->fc, cam, Color::red());
    //Drawable::drawVertex( f->nc, cam, Color::white());

    //Drawable::drawObject3DAxis(screen, f, cam, false, false, true);

    if (drawSides) {
        Drawable::drawPlane( f->planes[EngineSetup::getInstance()->LEFT_PLANE], cam, Color::white() );
        Drawable::drawPlane( f->planes[EngineSetup::getInstance()->RIGHT_PLANE], cam, Color::white() );
        Drawable::drawPlane( f->planes[EngineSetup::getInstance()->TOP_PLANE], cam, Color::white() );
        Drawable::drawPlane( f->planes[EngineSetup::getInstance()->BOTTOM_PLANE], cam, Color::white() );
    }

    if (drawFP) {
        // draw FAR PLANE
        Drawable::drawVector3D( f->far_top, cam, Color::red() );
        Drawable::drawVector3D( f->far_bottom, cam, Color::red() );
        Drawable::drawVector3D( f->far_left, cam, Color::red() );
        Drawable::drawVector3D( f->far_right, cam, Color::red() );
    }

    if (drawNP) {
        // draw NEAR PLANE
        Drawable::drawVector3D( f->near_top, cam, Color::pink() );
        Drawable::drawVector3D( f->near_bottom, cam, Color::green() );
        Drawable::drawVector3D( f->near_left, cam, Color::blue() );
        Drawable::drawVector3D( f->near_right, cam, Color::yellow() );
    }
}

void Drawable::drawVertex(Vertex3D V, Camera3D *cam, Uint32 color) {

    Vertex3D A = Transforms::cameraSpace(V, cam);
    A = Transforms::NDCSpace(A, cam);

    Point2D P1 = Transforms::screenSpace(A, cam);

    if (Tools::isPixelInWindow(P1.x, P1.y)) {
        EngineBuffers::getInstance()->setVideoBuffer(P1.x, P1.y, color);
    }
}

void Drawable::drawLine2D(Line2D L, Uint32 color)
{
    int x1 = L.x1;
    int y1 = L.y1;
    int x2 = L.x2;
    int y2 = L.y2;

    Uint32 col = color;
    int pasoy;
    int pasox;
    int deltaX = (x2 - x1);
    int deltaY = (y2 - y1);

    if (deltaY < 0) {
        deltaY = -deltaY;
        pasoy = -1;
    } else {
        pasoy = 1;
    }


    if (deltaX < 0) {
        deltaX = -deltaX;
        pasox = -1;
    } else {
        pasox = 1;
    }

    int x = x1;
    int y = y1;

    if (deltaX>deltaY){
        int p = 2 * deltaY - deltaX;
        int incE = 2 * deltaY;
        int incNE = 2 * (deltaY - deltaX);
        while (x != x2){
            x = x + pasox;
            if (p < 0){
                p = p + incE;
            } else {
                y = y + pasoy;
                p = p + incNE;
            }
            if (Tools::isPixelInWindow(x, y)) {
                EngineBuffers::getInstance()->setVideoBuffer(x, y, col);
            }
        }
    } else{
        int p = 2 * deltaX - deltaY;
        int incE = 2 * deltaX;
        int incNE = 2 * (deltaX - deltaY);
        while (y != y2){
            y = y + pasoy;
            if (p < 0) {
                p = p + incE;
            } else {
                x = x + pasox;
                p = p + incNE;
            }
            if (Tools::isPixelInWindow(x, y)) {
                EngineBuffers::getInstance()->setVideoBuffer(x, y, col);
            }

        }
    }
}

void Drawable::drawVector3D(Vector3D V, Camera3D *cam, Uint32 color)
{
    // apply view matrix
    Vertex3D V1 = Transforms::cameraSpace(V.vertex1, cam);
    Vertex3D V2 = Transforms::cameraSpace(V.vertex2, cam);

    V1 = Transforms::NDCSpace(V1, cam);
    V2 = Transforms::NDCSpace(V2, cam);

    // get 2d coordinates
    Point2D P1 = Transforms::screenSpace(V1, cam);
    Point2D P2 = Transforms::screenSpace(V2, cam);

    Line2D line(P1.x,P1.y, P2.x, P2.y);

    Drawable::drawLine2D(line, color);
}


void Drawable::drawPlane(Plane plane, Camera3D *cam, Uint32 color)
{
    Vector3D AB = Vector3D( plane.A, plane.B );
    Vector3D AC = Vector3D( plane.A, plane.C );
    Vector3D BC = Vector3D( plane.B, plane.C );

    Drawable::drawVector3D(AB, cam, color);
    Drawable::drawVector3D(AC, cam, color);
    Drawable::drawVector3D(BC, cam, color);
}


void Drawable::drawMainAxis(Camera3D *cam)
{
    Drawable::drawMainAxisOffset( cam, Vertex3D(0, 0, 0) );

    Point2D fixed_position = Point2D(EngineSetup::getInstance()->SCREEN_WIDTH - 50, 30);
    Drawable::drawMainAxisOffset( cam, Transforms::Point2DToWorld( fixed_position, cam) );
}

void Drawable::drawMainAxisOffset(Camera3D *cam, Vertex3D offset)
{
    float axis_length = 0.1;
    Vertex3D origin = offset;

    // Creamos unas coordenadas de eje sobre 0, 0, 0
    // start points
    Vertex3D VXstart( origin.x, origin.y, origin.z );
    Vertex3D VYstart( origin.x, origin.y, origin.z );
    Vertex3D VZstart( origin.x, origin.y, origin.z );
    Vertex3D VelocityStart( origin.x, origin.y, origin.z );

    // end points
    Vertex3D VXend( origin.x + (axis_length), origin.y , origin.z );
    Vertex3D VYend( origin.x, origin.y + (axis_length), origin.z );
    Vertex3D VZend( origin.x, origin.y, origin.z + (axis_length) );

    Vertex3D VelocityEnd( origin.x + (cam->collider->velocity.x), origin.y + (cam->collider->velocity.y), origin.z + (cam->collider->velocity.z));

    Vector3D axis_x = Vector3D(VXstart, VXend);
    Vector3D axis_y = Vector3D(VYstart, VYend);
    Vector3D axis_z = Vector3D(VZstart, VZend);

    Vector3D velocity = Vector3D(VelocityStart, VelocityEnd);

    Drawable::drawVector3D( axis_x, cam, Color::red() );
    Drawable::drawVector3D( axis_y, cam, Color::green() );
    Drawable::drawVector3D( axis_z, cam, Color::blue() );

    Drawable::drawVector3D( velocity, cam, Color::yellow() );

}

void Drawable::drawObject3DAxis(Object3D *object, Camera3D *cam, bool drawUp, bool drawRight, bool drawForward)
{
    //if (drawUp)      Drawable::drawVector3D( object->AxisUp(), cam, Color::red() );
    //if (drawRight)   Drawable::drawVector3D( object->AxisRight(), cam, Color::green() );
    //if (drawForward) Drawable::drawVector3D( object->AxisForward(), cam, Color::blue() );
}

void Drawable::drawBillboard(Billboard *B, Camera3D *cam)
{
    B->T1.draw( cam );
    B->T2.draw( cam );

    if (EngineSetup::getInstance()->TRIANGLE_MODE_WIREFRAME) {
        B->T1.drawWireframe();
        B->T2.drawWireframe();
    }
}
