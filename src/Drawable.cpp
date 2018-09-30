
#include "../headers/Drawable.h"
#include "../headers/Line2D.h"
#include "../headers/Point2D.h"
#include "../headers/Tools.h"
#include "../headers/Render.h"
#include "../headers/EngineBuffers.h"
#include "../headers/Billboard.h"

void Drawable::drawBox2D(SDL_Surface *surface, SDL_Rect r)
{
    Line2D l1, l2, l3, l4;

    int x = r.x;
    int y = r.y;

    // top
    l1.setup(x, y, x + r.w, y);

    //left
    l2.setup(x, y, x, y + r.h);

    //bottom
    l3.setup(x, y + r.h, x + r.w, y + r.h);

    //right
    l4.setup(x + r.w, y, x + r.w, y + r.h);

    l1.draw(surface);
    l2.draw(surface);
    l3.draw(surface);
    l4.draw(surface);
}

void Drawable::drawFrustum(SDL_Surface *screen, Frustum *f, Camera *cam, bool drawNP, bool drawFP, bool drawSides)
{
    // Center Near Plane
    Drawable::drawVertex(screen, f->fc, cam, Color::red());
    Drawable::drawVertex(screen, f->nc, cam, Color::white());

    //Drawable::drawObject3DAxis(screen, f, cam, false, false, true);

    if (drawSides) {
        Drawable::drawPlane(screen, f->planes[EngineSetup::getInstance()->LEFT_PLANE], cam, Color::white() );
        Drawable::drawPlane(screen, f->planes[EngineSetup::getInstance()->RIGHT_PLANE], cam, Color::white() );
        Drawable::drawPlane(screen, f->planes[EngineSetup::getInstance()->TOP_PLANE], cam, Color::white() );
        Drawable::drawPlane(screen, f->planes[EngineSetup::getInstance()->BOTTOM_PLANE], cam, Color::white() );
    }

    if (drawFP) {
        // draw FAR PLANE
        Drawable::drawVector3D(screen, f->far_top, cam, Color::red() );
        Drawable::drawVector3D(screen, f->far_bottom, cam, Color::red() );
        Drawable::drawVector3D(screen, f->far_left, cam, Color::red() );
        Drawable::drawVector3D(screen, f->far_right, cam, Color::red() );
    }

    if (drawNP) {
        // draw NEAR PLANE
        Drawable::drawVector3D(screen, f->near_top, cam, Color::pink() );
        Drawable::drawVector3D(screen, f->near_bottom, cam, Color::green() );
        Drawable::drawVector3D(screen, f->near_left, cam, Color::blue() );
        Drawable::drawVector3D(screen, f->near_right, cam, Color::yellow() );
    }
}

void Drawable::drawPlaneNormalVector(SDL_Surface *screen, Plane *plane, Camera *cam, Uint32 color)
{
    Vertex normal = plane->getNormalVector();

    normal.addVertex(plane->A);

    Vector3D vectorNormal = Vector3D (plane->A, normal);

    Drawable::drawVector3D(screen, vectorNormal, cam, color );
}


void Drawable::drawVertex(SDL_Surface *screen, Vertex V, Camera *cam, Uint32 color) {

    Vertex A = Render::cameraSpace(V, cam);
    A = Render::homogeneousClipSpace(A, cam);

    Point2D P1 = Render::screenSpace(A, cam);

    if (Tools::isPixelInWindow(P1.x, P1.y)) {
        EngineBuffers::getInstance()->setVideoBuffer(P1.x, P1.y, color);
    }
}

void Drawable::drawVector3D(SDL_Surface *screen, Vector3D V, Camera *cam, Uint32 color)
{
    if (!cam->frustum->isPointInFrustum(V.vertex1) && !cam->frustum->isPointInFrustum(V.vertex2)) {
        return;
    }

    // apply view matrix
    Vertex V1 = Render::cameraSpace(V.vertex1, cam);
    Vertex V2 = Render::cameraSpace(V.vertex2, cam);

    V1 = Render::homogeneousClipSpace(V1, cam);
    V2 = Render::homogeneousClipSpace(V2, cam);

    // get 2d coordinates
    Point2D P1 = Render::screenSpace(V1, cam);
    Point2D P2 = Render::screenSpace(V2, cam);

    Line2D line(P1.x,P1.y, P2.x, P2.y);

    line.draw(screen, color);
}


void Drawable::drawPlane(SDL_Surface *screen, Plane plane, Camera *cam, Uint32 color)
{
    Vector3D AB = Vector3D( plane.A, plane.B );
    Vector3D AC = Vector3D( plane.A, plane.C );
    Vector3D BC = Vector3D( plane.B, plane.C );

    Drawable::drawVector3D(screen, AB, cam, color);
    Drawable::drawVector3D(screen, AC, cam, color);
    Drawable::drawVector3D(screen, BC, cam, color);
}


void Drawable::drawMainAxis(SDL_Surface *screen, Camera *cam)
{
    float axis_length = 0.1;
    Vertex origin(0, 0, 0);

    // Creamos unas coordenadas de eje sobre 0, 0, 0
    // start points
    Vertex VXstart( origin.x, origin.y, origin.z );
    Vertex VYstart( origin.x, origin.y, origin.z );
    Vertex VZstart( origin.x, origin.y, origin.z );

    // end points
    Vertex VXend( origin.x + (axis_length), origin.y , origin.z );
    Vertex VYend( origin.x, origin.y + (axis_length), origin.z );
    Vertex VZend( origin.x, origin.y, origin.z + (axis_length) );

    Vector3D axis_x = Vector3D(VXstart, VXend);
    Vector3D axis_y = Vector3D(VYstart, VYend);
    Vector3D axis_z = Vector3D(VZstart, VZend);

    Drawable::drawVector3D( screen, axis_x, cam, Color::red() );
    Drawable::drawVector3D( screen, axis_y, cam, Color::green() );
    Drawable::drawVector3D( screen, axis_z, cam, Color::blue() );
}


void Drawable::drawObject3DAxis(SDL_Surface *screen, Object3D *object, Camera *cam, bool drawUp, bool drawRight, bool drawForward)
{
    object->updateAxis();

    if (drawUp)      Drawable::drawVector3D(screen, object->up, cam, Color::red() );
    if (drawRight)   Drawable::drawVector3D(screen, object->right, cam, Color::green() );
    if (drawForward) Drawable::drawVector3D(screen, object->forward, cam, Color::blue() );

}

void Drawable::drawBillboard(SDL_Surface *screen, Billboard *B, Camera *cam)
{
    B->T1.draw(screen, cam);
    B->T2.draw(screen, cam);

    if (EngineSetup::getInstance()->TRIANGLE_MODE_WIREFRAME) {
        B->T1.drawWireframe(screen, cam);
        B->T2.drawWireframe(screen, cam);
    }

}
