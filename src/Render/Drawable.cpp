
#include <utility>

#include "../../include/Render/Drawable.h"
#include "../../include/Render/Transforms.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Brakeza3D.h"

void Drawable::drawBox2D(SDL_Rect r) {
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

void Drawable::drawFrustum(Frustum *f, Camera3D *cam, bool drawNP, bool drawFP, bool drawSides) {
    // Center Near Plane
    Drawable::drawVertex( f->fc, cam, Color::red());
    Drawable::drawVertex( f->nc, cam, Color::white());


    if (drawSides) {
        Drawable::drawPlane(f->planes[EngineSetup::get()->LEFT_PLANE], cam, Color::white());
        Drawable::drawPlane(f->planes[EngineSetup::get()->RIGHT_PLANE], cam, Color::white());
        Drawable::drawPlane(f->planes[EngineSetup::get()->TOP_PLANE], cam, Color::white());
        Drawable::drawPlane(f->planes[EngineSetup::get()->BOTTOM_PLANE], cam, Color::white());
    }

    if (drawFP) {
        // draw FAR PLANE
        Drawable::drawVector3D(f->far_top, cam, Color::red());
        Drawable::drawVector3D(f->far_bottom, cam, Color::green());
        Drawable::drawVector3D(f->far_left, cam, Color::blue());
        Drawable::drawVector3D(f->far_right, cam, Color::fuchsia());
    }

    if (drawNP) {
        // draw NEAR PLANE
        Drawable::drawVector3D(f->near_top, cam, Color::red());
        Drawable::drawVector3D(f->near_bottom, cam, Color::green());
        Drawable::drawVector3D(f->near_left, cam, Color::blue());
        Drawable::drawVector3D(f->near_right, cam, Color::fuchsia());
    }
}

void Drawable::drawVertex(Vertex3D V, Camera3D *cam, Color color) {
    Vertex3D A;

    Transforms::cameraSpace(A, V, cam);
    A = Transforms::PerspectiveNDCSpace(A, cam->frustum);

    if (A.z < 0) {
        return;
    }

    Point2D P1;
    Transforms::screenSpace(P1, A);

    if (Tools::isPixelInWindow((int) P1.x, (int) P1.y)) {
        EngineBuffers::getInstance()->setVideoBuffer((int)P1.x, (int)P1.y, color.getColor());
    }
}

void Drawable::drawVertex3D(Vertex3D V, Camera3D *cam, Color color) {
    Vertex3D A;

    Transforms::cameraSpace(A, V, cam);
    A = Transforms::PerspectiveNDCSpace(A, cam->frustum);

    if (A.z < 0) {
        return;
    }

    Point2D P1;
    Transforms::screenSpace(P1, A);

    color = color + Color( 255* A.z);

    if (Tools::isPixelInWindow(P1.x, P1.y)) {
        EngineBuffers::getInstance()->setVideoBuffer(P1.x, P1.y, color.getColor());
    }
}

void Drawable::drawLine2D(Line2D L, Color color) {
    int x1 = L.x1;
    int y1 = L.y1;
    int x2 = L.x2;
    int y2 = L.y2;

    Color col = color;
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

    if (deltaX > deltaY) {
        int p = 2 * deltaY - deltaX;
        int incE = 2 * deltaY;
        int incNE = 2 * (deltaY - deltaX);
        while (x != x2) {
            x = x + pasox;
            if (p < 0) {
                p = p + incE;
            } else {
                y = y + pasoy;
                p = p + incNE;
            }
            if (Tools::isPixelInWindow(x, y)) {
                EngineBuffers::getInstance()->setVideoBuffer(x, y, col.getColor());
            }
        }
    } else {
        int p = 2 * deltaX - deltaY;
        int incE = 2 * deltaX;
        int incNE = 2 * (deltaX - deltaY);
        while (y != y2) {
            y = y + pasoy;
            if (p < 0) {
                p = p + incE;
            } else {
                x = x + pasox;
                p = p + incNE;
            }
            if (Tools::isPixelInWindow(x, y)) {
                EngineBuffers::getInstance()->setVideoBuffer(x, y, col.getColor());
            }
        }
    }
}

void Drawable::drawLine2DZBuffer(Line2D L, Color color, float z_start, float z_increment) {
    float current_z = z_start;

    int x1 = L.x1;
    int y1 = L.y1;
    int x2 = L.x2;
    int y2 = L.y2;

    Color col = color;
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

    if (deltaX > deltaY) {
        int p = 2 * deltaY - deltaX;
        int incE = 2 * deltaY;
        int incNE = 2 * (deltaY - deltaX);
        while (x != x2) {
            x = x + pasox;
            if (p < 0) {
                p = p + incE;
            } else {
                y = y + pasoy;
                p = p + incNE;
            }

            if (Tools::isPixelInWindow(x, y)) {
                current_z += z_increment;
                const int buffer_index = (y * EngineSetup::get()->screenWidth) + x;
                if (current_z >= EngineBuffers::getInstance()->getDepthBuffer(buffer_index)) {
                    continue;
                }
                EngineBuffers::getInstance()->setVideoBuffer(x, y, col.getColor());
            }
        }
    } else {
        int p = 2 * deltaX - deltaY;
        int incE = 2 * deltaX;
        int incNE = 2 * (deltaX - deltaY);
        while (y != y2) {
            y = y + pasoy;
            if (p < 0) {
                p = p + incE;
            } else {
                x = x + pasox;
                p = p + incNE;
            }
            if (Tools::isPixelInWindow(x, y)) {
                current_z += z_increment;
                const int buffer_index = (y * EngineSetup::get()->screenWidth) + x;
                if (current_z >= EngineBuffers::getInstance()->getDepthBuffer(buffer_index)) {
                    continue;
                }
                EngineBuffers::getInstance()->setVideoBuffer(x, y, col.getColor());
            }
        }
    }
}

void Drawable::drawVector3D(Vector3D V, Camera3D *cam, Color color) {
    Vertex3D V1, V2;
    Transforms::cameraSpace(V1, V.vertex1, cam);
    Transforms::cameraSpace(V2, V.vertex2, cam);

    V1 = Transforms::PerspectiveNDCSpace(V1, cam->frustum);
    if (V1.z < 0) {
        return;
    }

    V2 = Transforms::PerspectiveNDCSpace(V2, cam->frustum);
    if (V2.z < 0) {
        return;
    }

    // get 2d coordinates
    Point2D P1, P2;
    Transforms::screenSpace(P1, V1);
    Transforms::screenSpace(P2, V2);

    Line2D line(P1.x, P1.y, P2.x, P2.y);
    Drawable::drawLine2D(line, color);
}

void Drawable::drawVector3DZBuffer(Vector3D V, Camera3D *cam, Color color) {
    if (!cam->frustum->isVertexInside(V.vertex1) && !cam->frustum->isVertexInside(V.vertex2)) {
        return;
    }

    // apply view matrix
    Vertex3D V1, V2;
    Transforms::cameraSpace(V1, V.vertex1, cam);
    Transforms::cameraSpace(V2, V.vertex2, cam);

    V1 = Transforms::PerspectiveNDCSpace(V1, cam->frustum);
    V2 = Transforms::PerspectiveNDCSpace(V2, cam->frustum);

    // get 2d coordinates
    Point2D P1;
    Transforms::screenSpace(P1, V1);
    Point2D P2;
    Transforms::screenSpace(P2, V2);

    Line2D line(P1.x, P1.y, P2.x, P2.y);

    float z_diff = abs(V1.z - V2.z);
    float z_increment = z_diff / V.getComponent().getModule();


    Drawable::drawLine2DZBuffer(line, color, V1.z, z_increment);
}


void Drawable::drawPlane(Plane plane, Camera3D *cam, Color color) {
    Vector3D AB = Vector3D(plane.A, plane.B);
    Vector3D AC = Vector3D(plane.A, plane.C);
    Vector3D BC = Vector3D(plane.B, plane.C);

    Drawable::drawVector3D(AB, cam, color);
    Drawable::drawVector3D(AC, cam, color);
    Drawable::drawVector3D(BC, cam, color);
}

void Drawable::drawMainAxis(Camera3D *cam) {
    Point2D fixedPosition = Point2D(EngineSetup::get()->screenWidth - 50, 30);
    Drawable::drawMainAxisOffset(cam, Transforms::Point2DToWorld(fixedPosition, cam));
}

void Drawable::drawMainAxisOffset(Camera3D *cam, Vertex3D offset) {
    float axis_length = 0.1;
    Vertex3D origin = offset;

    // Creamos unas coordenadas de eje sobre 0, 0, 0
    // start points
    Vertex3D VXstart(origin.x, origin.y, origin.z);
    Vertex3D VYstart(origin.x, origin.y, origin.z);
    Vertex3D VZstart(origin.x, origin.y, origin.z);

    // end points
    Vertex3D VXend(origin.x + (axis_length), origin.y, origin.z);
    Vertex3D VYend(origin.x, origin.y + (axis_length), origin.z);
    Vertex3D VZend(origin.x, origin.y, origin.z + (axis_length));

    Vector3D axis_x = Vector3D(VXstart, VXend);
    Vector3D axis_y = Vector3D(VYstart, VYend);
    Vector3D axis_z = Vector3D(VZstart, VZend);

    Drawable::drawVector3D(axis_x, cam, Color::red());
    Drawable::drawVector3D(axis_y, cam, Color::green());
    Drawable::drawVector3D(axis_z, cam, Color::blue());

}

void Drawable::drawObject3DAxis(Object3D *object, Camera3D *cam, bool drawUp, bool drawRight, bool drawForward) {
    if (!cam->frustum->isVertexInside(object->getPosition())) {
        return;
    }

    float size = EngineSetup::get()->OBJECT_AXIS_SIZE;
    Vertex3D endRight = object->getPosition() + object->AxisRight().getScaled(size);
    Vertex3D endUp = object->getPosition() + object->AxisUp().getScaled(size);
    Vertex3D endForward = object->getPosition() + object->AxisForward().getScaled(size);

    Vector3D vRight = Vector3D(object->getPosition(), endRight);
    Vector3D vUp = Vector3D(object->getPosition(), endUp);
    Vector3D vForward = Vector3D(object->getPosition(), endForward);

    if (drawRight) Drawable::drawVector3D(vRight, cam, Color::green());
    if (drawUp) Drawable::drawVector3D(vUp, cam, Color::red());
    if (drawForward) Drawable::drawVector3D(vForward, cam, Color::blue());
}

void Drawable::drawBillboard(Billboard *B, std::vector<Triangle *> *frameTriangles) {
    //B->T1.draw( frustum );
    frameTriangles->emplace_back(&B->T1);
    frameTriangles->emplace_back(&B->T2);

    if (EngineSetup::get()->TRIANGLE_MODE_WIREFRAME) {
        //Brakeza3D::get()->drawWireframe(&B->T1);
        //Brakeza3D::get()->drawWireframe(&B->T2);
    }
}

void Drawable::drawLightning(Camera3D *cam, Vertex3D A, Vertex3D B) {
    float generations = EngineSetup::get()->LIGHTNING_GENERATIONS;
    std::vector<Vector3D> segmentList;
    std::vector<Vector3D> tmpList;
    std::vector<Vector3D> newSegments;

    float offsetAmount = EngineSetup::get()->LIGHTNING_OFFSET_REDUCTION;
    float multiplier = EngineSetup::get()->LIGHTNING_SEGMENT_SHIFT;
    float probabilityBranch = EngineSetup::get()->LIGHTNING_PROBABILITY_BRANCH;

    segmentList.emplace_back(Vector3D(A, B));

    for (int i = 0; i < generations; i++) {
        tmpList = segmentList;
        int j = 0;
        for (auto ir = tmpList.begin(); ir != tmpList.end(); ++j) {
            Vertex3D midPoint = ir.base()->middlePoint();

            midPoint.x += (float) Tools::random((int) -offsetAmount, (int) offsetAmount) * multiplier;
            midPoint.y += (float) Tools::random((int) -offsetAmount, (int) offsetAmount) * multiplier;
            midPoint.z += (float) Tools::random((int) -offsetAmount, (int) offsetAmount) * multiplier;

            if (Tools::random(1, 10) > 10 - probabilityBranch) {

                Vertex3D splitEnd;
                Vertex3D direction = ir.base()->getComponent().getNormalize().getScaled(offsetAmount * 2);
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

        offsetAmount /= 2;
    }

    for (auto ir = segmentList.begin(); ir != segmentList.end(); ++ir) {
        Drawable::drawVector3DZBuffer(*ir.base(), cam, Color::cyan());
    }
}

void Drawable::drawCrossHair() {

    int x = EngineSetup::get()->screenWidth / 2;
    int y = EngineSetup::get()->screenHeight / 2;

    Uint32 color = EngineSetup::get()->CROSSHAIR_COLOR;
    for (int cw = 1; cw < 3; cw++) {
        EngineBuffers::getInstance()->setVideoBuffer(x + cw, y, color);
        EngineBuffers::getInstance()->setVideoBuffer(x - cw, y, color);
        EngineBuffers::getInstance()->setVideoBuffer(x, y + cw, color);
        EngineBuffers::getInstance()->setVideoBuffer(x, y - cw, color);
    }
}

void Drawable::drawFadeIn() {
    Brakeza3D::get()->currentFadePercent -= 0.01;
    if (Brakeza3D::get()->currentFadePercent < 0) {
        Brakeza3D::get()->currentFadePercent = 0;
    }
    Drawable::drawFacePercent(Brakeza3D::get()->currentFadePercent);
}

void Drawable::drawFadeOut() {
    Brakeza3D::get()->currentFadePercent += 0.01;
    if (Brakeza3D::get()->currentFadePercent > 1) {
        Brakeza3D::get()->currentFadePercent = 1;
    }
    Drawable::drawFacePercent(Brakeza3D::get()->currentFadePercent);
}

void Drawable::drawFacePercent(float percent) {

    EngineBuffers *buffers = EngineBuffers::getInstance();
    for (int i = 0; i < buffers->sizeBuffers; i++) {
        Color bufferColor = Color(buffers->videoBuffer[i]);
        Color mixedColor = Tools::mixColor(bufferColor, Color::black(), percent);
        buffers->videoBuffer[i] = mixedColor.getColor();
    }
}

void Drawable::drawAABB(AABB3D *aabb, Color color) {
    Vector3D v01(aabb->vertices[0], aabb->vertices[2]);
    Vector3D v02(aabb->vertices[0], aabb->vertices[3]);
    Vector3D v03(aabb->vertices[0], aabb->vertices[4]);
    Vector3D v04(aabb->vertices[1], aabb->vertices[6]);
    Vector3D v05(aabb->vertices[1], aabb->vertices[5]);
    Vector3D v06(aabb->vertices[2], aabb->vertices[5]);
    Vector3D v07(aabb->vertices[2], aabb->vertices[6]);
    Vector3D v08(aabb->vertices[3], aabb->vertices[7]);
    Vector3D v09(aabb->vertices[3], aabb->vertices[5]);
    Vector3D v10(aabb->vertices[6], aabb->vertices[4]);
    Vector3D v11(aabb->vertices[7], aabb->vertices[4]);
    Vector3D v12(aabb->vertices[7], aabb->vertices[1]);

    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    Drawable::drawVector3D(v01, camera, color);
    Drawable::drawVector3D(v02, camera, color);
    Drawable::drawVector3D(v03, camera, color);
    Drawable::drawVector3D(v04, camera, color);
    Drawable::drawVector3D(v05, camera, color);
    Drawable::drawVector3D(v06, camera, color);
    Drawable::drawVector3D(v07, camera, color);
    Drawable::drawVector3D(v08, camera, color);
    Drawable::drawVector3D(v09, camera, color);
    Drawable::drawVector3D(v10, camera, color);
    Drawable::drawVector3D(v11, camera, color);
    Drawable::drawVector3D(v12, camera, color);

    Drawable::drawVertex(aabb->max, camera, Color::red());
    Drawable::drawVertex(aabb->min, camera, Color::green());
}

void Drawable::drawOctreeNode(OctreeNode *node, bool onlyWithTriangles)
{
    Color color = Color::white();
    if (node->isLeaf()) {
        Drawable::drawAABB(&node->bounds, color);
    }

    for (auto & i : node->children) {
        if (i != nullptr) {
            Drawable::drawOctreeNode(i, onlyWithTriangles);
        }
    }
}

void Drawable::drawOctree(Octree *octree, bool onlyWithTriangles) {
    Drawable::drawAABB(&octree->root->bounds, Color::yellow());
    Drawable::drawOctreeNode(octree->root, onlyWithTriangles);
}

void Drawable::drawGrid3D(Grid3D *grid) {
    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    for (auto & boxe : grid->boxes) {

        if (boxe->is_empty && EngineSetup::get()->DRAW_MESH3D_GRID_EMPTY) {
            Color c = Color::yellow();
            if (EngineSetup::get()->DRAW_MESH3D_GRID_CUBES) {
                Drawable::drawAABB(boxe->box, c);
            }
            if (EngineSetup::get()->DRAW_MESH3D_GRID_POINTS) {
                Drawable::drawVertex(boxe->box->getCenter(), camera, c);
            }
        }

        if (!boxe->is_empty && EngineSetup::get()->DRAW_MESH3D_GRID_NO_EMPTY) {
            Color c = Color::red();
            if (EngineSetup::get()->DRAW_MESH3D_GRID_CUBES) {
                Drawable::drawAABB(boxe->box, c);
            }
            if (EngineSetup::get()->DRAW_MESH3D_GRID_POINTS) {

                Drawable::drawVertex(boxe->box->getCenter(), camera, c);
            }
        }
    }
}

void Drawable::drawPathInGrid(Grid3D *grid, std::stack<PairData> path) {
    auto *camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    std::vector<Vertex3D> pathVertices = Tools::getVerticesFromPathFinderPath(grid, std::move(path));

    for (auto & pathVertice : pathVertices) {
        Drawable::drawVertex(pathVertice, camera, Color::cyan());
    }
}

void Drawable::drawPathDebugForDevelopment(Grid3D *grid, PathFinder *pathfinder) {
    std::stack<PairData> path;
    PairData src = std::make_pair(EngineSetup::get()->TESTING_INT1,
                                  EngineSetup::get()->TESTING_INT2);
    PairData dest = std::make_pair(EngineSetup::get()->TESTING_INT3,
                                   EngineSetup::get()->TESTING_INT4);

    CubeGrid3D *cubeStart = grid->getFromPosition(src.first, 0, src.second);
    CubeGrid3D *cubeDest = grid->getFromPosition(dest.first, 0, dest.second);

    bool result = pathfinder->AStarSearch(src, dest, path);
    if (result) {
        Drawable::drawPathInGrid(grid, path);
    }

    if (cubeStart != nullptr)
        Drawable::drawAABB(cubeStart->box, Color::green());

    if (cubeDest != nullptr)
        Drawable::drawAABB(cubeDest->box, Color::red());
}

void Drawable::drawLinePoints(Vertex3D from, Vertex3D to, Color color)
{
    Vector3D V = Vector3D(from, to);
    Drawable::drawVector3D(V, ComponentsManager::get()->getComponentCamera()->getCamera(), Color::olive());
}

void Drawable::drawMainDeepMapFromCamera(int pos_x, int pos_y)
{
    int width = EngineSetup::get()->screenWidth;
    int height = EngineSetup::get()->screenHeight;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            auto pixelColor = EngineBuffers::getInstance()->getDepthBuffer(j, i);
            EngineBuffers::getInstance()->setVideoBuffer(j + pos_x, i + pos_y, Color(pixelColor/10, pixelColor/10, pixelColor/10).getColor());
        }
    }
}
