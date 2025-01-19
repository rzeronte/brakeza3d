#define GL_GLEXT_PROTOTYPES


#include <utility>

#include "../../include/Render/Drawable.h"
#include "../../include/Render/Transforms.h"
#include "../../include/LUAManager.h"
#include "../../include/Brakeza3D.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>

void Drawable::drawVertex(Vertex3D V, Camera3D *cam, Color color)
{
    Point2D P1 = Transforms::WorldToPoint(V);

    if (Tools::isPixelInWindow(P1.x, P1.y)) {
    }
}

void Drawable::drawVector3D(Vector3D V, Color color)
{
    glm::mat4 ViewMatrix = ComponentsManager::get()->getComponentCamera()->getCamera()->getViewMatrix();
    glm::mat4 ProjectionMatrix = Camera3D::getProjectionMatrix();

    glm::vec4 position1 = ProjectionMatrix * ViewMatrix * glm::vec4(V.vertex1.x, V.vertex1.y, V.vertex1.z, 1.0);
    position1 /= position1.w;

    glm::vec4 position2 = ProjectionMatrix * ViewMatrix * glm::vec4(V.vertex2.x, V.vertex2.y, V.vertex2.z, 1.0);
    position2 /= position2.w;

    const auto windowWidth = EngineSetup::get()->screenWidth;
    const auto windowHeight = EngineSetup::get()->screenHeight;

    Point2D P1((int)((position1.x + 1.0) * 0.5 * windowWidth), (int)((1.0 - position1.y) * 0.5 * windowHeight));
    Point2D P2((int)((position2.x + 1.0) * 0.5 * windowWidth), (int)((1.0 - position2.y) * 0.5 * windowHeight));

    ComponentsManager::get()->getComponentWindow()->getShaderOGLLine()->render(
        P1,
        P2,
        color,
        0.0001f,
        ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer()
    );
}

void Drawable::drawMainAxis()
{
    Point2D fixedPosition = Point2D(EngineSetup::get()->screenWidth - 50, 30);
    Drawable::drawMainAxisOffset(Transforms::Point2DToWorld(fixedPosition));
}

void Drawable::drawMainAxisOffset(Vertex3D offset)
{
    float axis_length = EngineSetup::get()->OBJECT_AXIS_SIZE;
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

    Drawable::drawVector3D(axis_x, Color::red());
    Drawable::drawVector3D(axis_y, Color::green());
    Drawable::drawVector3D(axis_z, Color::blue());

}

void Drawable::drawObject3DAxis(Object3D *object, bool drawUp, bool drawRight, bool drawForward)
{
    if (!Frustum::isVertexInside(object->getPosition())) {
        return;
    }

    const float size = EngineSetup::get()->OBJECT_AXIS_SIZE;
    Vertex3D endRight = object->getPosition() + object->AxisRight().getScaled(size);
    Vertex3D endUp = object->getPosition() + object->AxisUp().getScaled(size);
    Vertex3D endForward = object->getPosition() + object->AxisForward().getScaled(size);

    Vector3D vRight = Vector3D(object->getPosition(), endRight);
    Vector3D vUp = Vector3D(object->getPosition(), endUp);
    Vector3D vForward = Vector3D(object->getPosition(), endForward);

    if (drawRight) Drawable::drawVector3D(vRight, Color::green());
    if (drawUp) Drawable::drawVector3D(vUp, Color::red());
    if (drawForward) Drawable::drawVector3D(vForward, Color::blue());
}

void Drawable::drawLightning(Vertex3D A, Vertex3D B, Color color) {

    float generations = EngineSetup::get()->LIGHTNING_GENERATIONS;
    std::vector<Vector3D> segmentList;
    std::vector<Vector3D> tmpList;
    std::vector<Vector3D> newSegments;

    float offsetAmount = EngineSetup::get()->LIGHTNING_OFFSET_REDUCTION;
    float multiplier = EngineSetup::get()->LIGHTNING_SEGMENT_SHIFT;
    float probabilityBranch = EngineSetup::get()->LIGHTNING_PROBABILITY_BRANCH;

    segmentList.emplace_back(A, B);

    for (int i = 0; i < (int) generations; i++) {
        tmpList = segmentList;
        int j = 0;
        for (auto ir = tmpList.begin(); ir != tmpList.end(); ++j) {
            Vertex3D midPoint = ir.base()->middlePoint();

            midPoint.x += (float) Tools::random((int) -offsetAmount, (int) offsetAmount) * multiplier;
            midPoint.y += (float) Tools::random((int) -offsetAmount, (int) offsetAmount) * multiplier;
            midPoint.z += (float) Tools::random((int) -offsetAmount, (int) offsetAmount) * multiplier;

            if ((float) Tools::random(1, 10) > 10 - probabilityBranch) {

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
    for ( auto ir = segmentList.begin(); ir != segmentList.end(); ++ir) {
        Drawable::drawVector3D(*ir.base(), color);
    }
}

void Drawable::drawAABB(AABB3D *aabb, Color color)
{
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

    Drawable::drawVector3D(v01, color);
    Drawable::drawVector3D(v02, color);
    Drawable::drawVector3D(v03, color);
    Drawable::drawVector3D(v04, color);
    Drawable::drawVector3D(v05, color);
    Drawable::drawVector3D(v06, color);
    Drawable::drawVector3D(v07, color);
    Drawable::drawVector3D(v08, color);
    Drawable::drawVector3D(v09, color);
    Drawable::drawVector3D(v10, color);
    Drawable::drawVector3D(v11, color);
    Drawable::drawVector3D(v12, color);

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

void Drawable::drawObject3DGizmo(
    Object3D *o,
    ImGuizmo::OPERATION currentOperation,
    glm::mat4 objectMatrix,
    glm::mat4 viewMatrix,
    glm::mat4 projectionMatrix
) {
    ImGuiIO& io = ImGui::GetIO();

    if (!ComponentsManager::get()->getComponentWindow()->isWindowMaximized()) {
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    } else {
        auto windowWidth = (float) ImGui::GetWindowWidth();
        auto windowHeight = (float) ImGui::GetWindowHeight();

        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
    }

    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist(ImGui::GetCurrentWindow()->DrawList);

    auto objectMatrixManipulated = objectMatrix;

    ImGuizmo::Manipulate(
        glm::value_ptr(viewMatrix),
        glm::value_ptr(projectionMatrix),
        currentOperation,
        ImGuizmo::LOCAL,
        glm::value_ptr(objectMatrixManipulated),
        nullptr,
        nullptr
    );

    if (ImGuizmo::IsOver()) {
    }

    if (ImGuizmo::IsUsing()) {
        if (currentOperation == ImGuizmo::OPERATION::TRANSLATE) {
            auto position = glm::vec3(objectMatrixManipulated[3]);
            o->setPosition(Vertex3D(position[0], position[1], position[2]));
        }

        if (currentOperation == ImGuizmo::OPERATION::ROTATE) {
            o->setRotation(M3::fromMat3GLM(objectMatrixManipulated).getTranspose());
        }

        if (currentOperation == ImGuizmo::OPERATION::SCALE) {
            o->setScale((float) glm::length(glm::vec3(objectMatrixManipulated[0])));
        }
    }
}