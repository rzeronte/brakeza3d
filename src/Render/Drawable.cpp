
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/EngineBuffers.h"
#include "../../headers/Brakeza3D.h"

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

void Drawable::drawVertex(Vertex3D V, Camera3D *cam, Uint32 color)
{
    Vertex3D A;

    Transforms::cameraSpace(A, V, cam);
    A = Transforms::NDCSpace(A, cam);

    Point2D P1;
    Transforms::screenSpace(P1, A);

    if (Tools::isPixelInWindow((int)P1.x, (int)P1.y)) {
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

void Drawable::drawLine2DZBuffer(Line2D L, Uint32 color, float z_start, float z_increment)
{
    float current_z = z_start;

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
                current_z+=z_increment;
                const int buffer_index = ( y * EngineSetup::getInstance()->screenWidth ) + x;
                if (current_z  >= EngineBuffers::getInstance()->getDepthBuffer( buffer_index ) ) {
                    continue;
                }
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
                current_z+=z_increment;
                const int buffer_index = ( y * EngineSetup::getInstance()->screenWidth ) + x;
                if (current_z >= EngineBuffers::getInstance()->getDepthBuffer( buffer_index ) ) {
                    continue;
                }
                EngineBuffers::getInstance()->setVideoBuffer(x, y, col);
            }
        }
    }
}

void Drawable::drawVector3D(Vector3D V, Camera3D *cam, Uint32 color)
{
    // apply view matrix
    Vertex3D V1, V2;
    Transforms::cameraSpace(V1, V.vertex1, cam);
    Transforms::cameraSpace(V2, V.vertex2, cam);

    V1 = Transforms::NDCSpace(V1, cam);
    V2 = Transforms::NDCSpace(V2, cam);

    // get 2d coordinates
    Point2D P1; Transforms::screenSpace(P1, V1);
    Point2D P2; Transforms::screenSpace(P2, V2);

    Line2D line(P1.x,P1.y, P2.x, P2.y);

    Drawable::drawLine2D(line, color);
}

void Drawable::drawVector3DZBuffer(Vector3D V, Camera3D *cam, Uint32 color)
{
    if ( !cam->frustum->isPointInFrustum(V.vertex1) && !cam->frustum->isPointInFrustum(V.vertex2) ) {
        return;
    }

    // apply view matrix
    Vertex3D V1, V2;
    Transforms::cameraSpace(V1, V.vertex1, cam);
    Transforms::cameraSpace(V2, V.vertex2, cam);

    V1 = Transforms::NDCSpace(V1, cam);
    V2 = Transforms::NDCSpace(V2, cam);

    // get 2d coordinates
    Point2D P1; Transforms::screenSpace(P1, V1);
    Point2D P2; Transforms::screenSpace(P2, V2);

    Line2D line(P1.x,P1.y, P2.x, P2.y);

    float z_diff = abs(V1.z-V2.z);
    float z_increment = z_diff/V.getComponent().getModule();


    Drawable::drawLine2DZBuffer(line, color, V1.z, z_increment);
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
    Point2D fixed_position = Point2D(EngineSetup::getInstance()->screenWidth - 50, 30);
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

    // end points
    Vertex3D VXend( origin.x + (axis_length), origin.y , origin.z );
    Vertex3D VYend( origin.x, origin.y + (axis_length), origin.z );
    Vertex3D VZend( origin.x, origin.y, origin.z + (axis_length) );

    Vector3D axis_x = Vector3D(VXstart, VXend);
    Vector3D axis_y = Vector3D(VYstart, VYend);
    Vector3D axis_z = Vector3D(VZstart, VZend);

    Drawable::drawVector3D( axis_x, cam, Color::red() );
    Drawable::drawVector3D( axis_y, cam, Color::green() );
    Drawable::drawVector3D( axis_z, cam, Color::blue() );

}

void Drawable::drawObject3DAxis(Object3D *object, Camera3D *cam, bool drawUp, bool drawRight, bool drawForward)
{
    if (!cam->frustum->isPointInFrustum(*object->getPosition())) {
        return;
    }

    if (drawRight)   Drawable::drawVector3D( Vector3D(*object->getPosition(), *object->getPosition()+object->AxisRight().getScaled(3)), cam, Color::green() );
    if (drawUp)      Drawable::drawVector3D( Vector3D(*object->getPosition(), *object->getPosition()+object->AxisUp().getScaled(3)), cam, Color::red() );
    if (drawForward) Drawable::drawVector3D( Vector3D(*object->getPosition(), *object->getPosition()+object->AxisForward().getScaled(3)), cam, Color::blue() );
}

void Drawable::drawBillboard(Billboard *B, std::vector<Triangle*> *frameTriangles)
{
    //B->T1.draw( cam );
    frameTriangles->emplace_back(&B->T1);
    frameTriangles->emplace_back(&B->T2);

    if (EngineSetup::getInstance()->TRIANGLE_MODE_WIREFRAME) {
        Brakeza3D::get()->drawWireframe(&B->T1);
        Brakeza3D::get()->drawWireframe(&B->T2);
    }
}

void Drawable::drawLightning(Camera3D *cam, Vertex3D A, Vertex3D B)
{
    float generations = EngineSetup::getInstance()->LIGHTNING_GENERATIONS;
    std::vector<Vector3D> segmentList;
    std::vector<Vector3D> tmpList;
    std::vector<Vector3D> newSegments;

    float offsetAmount = EngineSetup::getInstance()->LIGHTNING_OFFSET_REDUCTION;
    float multiplier = EngineSetup::getInstance()->LIGHTNING_SEGMENT_SHIFT;
    float probabilityBranch = EngineSetup::getInstance()->LIGHTNING_PROBABILITY_BRANCH;

    segmentList.emplace_back(Vector3D(A, B));

    for (int i = 0; i < generations; i++) {
        tmpList = segmentList;
        int j = 0;
        for (auto ir = tmpList.begin(); ir != tmpList.end(); ++j) {
            Vertex3D midPoint = ir.base()->middlePoint();

            midPoint.x += (float) Tools::random((int) -offsetAmount, (int) offsetAmount) * multiplier;
            midPoint.y += (float) Tools::random((int) -offsetAmount, (int) offsetAmount) * multiplier;
            midPoint.z += (float) Tools::random((int) -offsetAmount, (int) offsetAmount) * multiplier;

            if ( Tools::random(1, 10) > 10-probabilityBranch) {

                Vertex3D splitEnd;
                Vertex3D direction = ir.base()->getComponent().getNormalize().getScaled( offsetAmount * 2 );
                splitEnd.x = midPoint.x + direction.x;
                splitEnd.y = midPoint.y + direction.y;
                splitEnd.z = midPoint.z + direction.z;

                Vector3D segmentBranch = Vector3D(midPoint, splitEnd);
                newSegments.push_back(segmentBranch);
            }

            Vector3D segment1 = Vector3D(ir.base()->vertex1, midPoint);
            Vector3D segment2 = Vector3D(midPoint, ir.base()->vertex2);
            newSegments.push_back(segment1);
            newSegments.push_back(segment2);

            ++ir;
        }
        segmentList = newSegments;
        newSegments.clear();

        offsetAmount/=2;
    }

    for (auto ir = segmentList.begin() ; ir != segmentList.end(); ++ir) {
        Drawable::drawVector3DZBuffer(*ir.base(), cam, Color::cyan());
    }
}

void Drawable::drawCrossHair()
{
    int x, y;

    x = EngineSetup::getInstance()->screenWidth / 2;
    y = EngineSetup::getInstance()->screenHeight / 2;

    Uint32 color = EngineSetup::getInstance()->CROSSHAIR_COLOR;
    for (int cw = 1; cw < 3; cw++) {
        EngineBuffers::getInstance()->setVideoBuffer(x+cw, y, color);
        EngineBuffers::getInstance()->setVideoBuffer(x-cw, y, color);
        EngineBuffers::getInstance()->setVideoBuffer(x, y+cw, color);
        EngineBuffers::getInstance()->setVideoBuffer(x, y-cw, color);
    }
}

void Drawable::drawFireShader()
{
    // Set whole screen to 0 (color: 0x07,0x07,0x07)
    int FIRE_WIDTH = EngineSetup::getInstance()->FIRE_WIDTH;
    int FIRE_HEIGHT = EngineSetup::getInstance()->FIRE_HEIGHT;

    for(int x = 0 ; x < FIRE_WIDTH ; x++) {
        for (int y = 1; y < FIRE_HEIGHT ; y++) {
            int src = (y * FIRE_WIDTH + x);

            int pixel = EngineBuffers::getInstance()->firePixelsBuffer[src];

            if ( pixel == 0) {
                EngineBuffers::getInstance()->firePixelsBuffer[src - FIRE_WIDTH] = 0;
            } else {
                int randIdx = Tools::random(0, 3) & 3;
                int dst = src - randIdx + 1;
                EngineBuffers::getInstance()->firePixelsBuffer[dst - FIRE_WIDTH ] = pixel - (randIdx & 1);
            }
        }
    }

    for (int y = 1; y < FIRE_HEIGHT; y++) {
        for(int x = 0 ; x < FIRE_WIDTH ; x++) {
            int index = y * FIRE_WIDTH + x;
            int fireIndex = EngineBuffers::getInstance()->firePixelsBuffer[index];

            if (fireIndex != 0) {
                Uint32 fireColor = EngineBuffers::getInstance()->fireColors[fireIndex];
                EngineBuffers::getInstance()->videoBuffer[ index ] = Color::black(); //fireColor; //::black();
            }
        }
    }
}

void Drawable::drawFadeIn()
{
    Brakeza3D::get()->currentFadePercent -= 0.01;
    if (Brakeza3D::get()->currentFadePercent < 0) {
        Brakeza3D::get()->currentFadePercent = 0;
        EngineSetup::getInstance()->FADEIN = false;
    }
    Drawable::drawFacePercent(Brakeza3D::get()->currentFadePercent);
}

void Drawable::drawFadeOut()
{
    Brakeza3D::get()->currentFadePercent += 0.01;
    if (Brakeza3D::get()->currentFadePercent > 1) {
        Brakeza3D::get()->currentFadePercent = 1;
    }
    Drawable::drawFacePercent(Brakeza3D::get()->currentFadePercent);
}

void Drawable::drawFacePercent(float percent) {

    EngineBuffers *buffers = EngineBuffers::getInstance();
    for (int i = 0 ; i < buffers->sizeBuffers; i++) {
        buffers->videoBuffer[i] = Tools::mixColor(buffers->videoBuffer[i], Color::black(), percent);
    }
}

void Drawable::waterShader(int type)
{
    //BSP LAVA EFFECT
    float LAVA_CLOSENESS = 2.35;
    float LAVA_INTENSITY = 0.45;
    float LAVA_SPEED = 2.55;
    float LAVA_SCALE = 2.35;

    // Default config is used in menu mode
    float intensity_r = 1;
    float intensity_g = 1;
    float intensity_b = 1;

    //water = -3 |mud = -4 | lava = -5
    switch( type ) {
        default:
        case -3:
            break;
        case -4:
            intensity_r = 0.5;
            intensity_g = 1;
            intensity_b = 0.5;
            break;
        case -5:
            intensity_r = 1;
            intensity_g = 0.5;
            intensity_b = 0.5;
            break;
    }

    Uint32 *newVideoBuffer = new Uint32[EngineBuffers::getInstance()->sizeBuffers];

    for (int y = 0; y < EngineSetup::getInstance()->screenHeight; y++) {
        for (int x = 0; x < EngineSetup::getInstance()->screenWidth; x++) {
            Uint32 currentPixelColor = EngineBuffers::getInstance()->getVideoBuffer(x, y);

            int r_light = (int) (Tools::getRedValueFromColor(currentPixelColor)   * intensity_r);
            int g_light = (int) (Tools::getGreenValueFromColor(currentPixelColor) * intensity_g);
            int b_light = (int) (Tools::getBlueValueFromColor(currentPixelColor)  * intensity_b);

            currentPixelColor = Tools::createRGB( r_light, g_light, b_light );

            float cache1 = x / LAVA_CLOSENESS;
            float cache2 = y / LAVA_CLOSENESS;

            int nx = (cache1 + LAVA_INTENSITY * sin(LAVA_SPEED * Brakeza3D::get()->executionTime + cache2) ) * LAVA_SCALE;
            int ny = (cache2 + LAVA_INTENSITY * sin(LAVA_SPEED * Brakeza3D::get()->executionTime + cache1) ) * LAVA_SCALE;

            int bufferIndex = nx + ny * EngineSetup::getInstance()->screenWidth;

            if(Tools::isPixelInWindow(nx, ny)) {
                newVideoBuffer[bufferIndex] = currentPixelColor;
            }
        }
    }

    memcpy (&EngineBuffers::getInstance()->videoBuffer, &newVideoBuffer, sizeof(newVideoBuffer));
}
