#define GL_GLEXT_PROTOTYPES

#include "../../include/Render/Drawable.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Brakeza3D.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>

void Drawable::drawVertex(Vertex3D V, Color color)
{
    Point2D P1 = Transforms::WorldToPoint(V);
    glDisable(GL_BLEND);

    if (Tools::isPixelInWindow(P1.x, P1.y)) {
        glBegin(GL_POINTS);
            glVertex3i(V.x, V.y, V.z);
        glEnd();
    }
    glEnable(GL_BLEND);
}

void Drawable::drawVector3D(Vector3D V, Color color)
{
    ComponentsManager::get()->getComponentRender()->drawLine(V.vertex1,V.vertex2,color);
}

void Drawable::drawMainAxis()
{
    Point2D fixedPosition = Point2D(EngineSetup::get()->screenWidth - 80, 50);
    Drawable::drawMainAxisOffset(Transforms::Point2DToWorld(fixedPosition));
}

void Drawable::drawMainAxisOffset(Vertex3D offset)
{
    float axis_length = 0.075f;
    Vertex3D origin = offset;

    // Creamos unas coordenadas de eje sobre 0, 0, 0
    // start points
    Vertex3D VXstart(origin.x, origin.y, origin.z);
    Vertex3D VYstart(origin.x, origin.y, origin.z);
    Vertex3D VZstart(origin.x, origin.y, origin.z);

    // end points
    Vertex3D VXend(origin.x + axis_length, origin.y, origin.z);
    Vertex3D VYend(origin.x, origin.y + axis_length, origin.z);
    Vertex3D VZend(origin.x, origin.y, origin.z + axis_length);

    Vector3D axis_x = Vector3D(VXstart, VXend);
    Vector3D axis_y = Vector3D(VYstart, VYend);
    Vector3D axis_z = Vector3D(VZstart, VZend);

    drawVector3D(axis_x, Color::red());
    drawVector3D(axis_y, Color::green());
    drawVector3D(axis_z, Color::blue());
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
    std::vector<Vector3D> vectors;
    vectors.emplace_back(aabb->vertices[0], aabb->vertices[2]);
    vectors.emplace_back(aabb->vertices[0], aabb->vertices[3]);
    vectors.emplace_back(aabb->vertices[0], aabb->vertices[4]);
    vectors.emplace_back(aabb->vertices[1], aabb->vertices[6]);
    vectors.emplace_back(aabb->vertices[1], aabb->vertices[5]);
    vectors.emplace_back(aabb->vertices[2], aabb->vertices[5]);
    vectors.emplace_back(aabb->vertices[2], aabb->vertices[6]);
    vectors.emplace_back(aabb->vertices[3], aabb->vertices[7]);
    vectors.emplace_back(aabb->vertices[3], aabb->vertices[5]);
    vectors.emplace_back(aabb->vertices[6], aabb->vertices[4]);
    vectors.emplace_back(aabb->vertices[7], aabb->vertices[4]);
    vectors.emplace_back(aabb->vertices[7], aabb->vertices[1]);

    auto window = ComponentsManager::get()->getComponentWindow();
    ComponentsManager::get()->getComponentRender()->getShaderOGLLine3D()->renderLines(vectors, window->getForegroundFramebuffer(),color);
}

void Drawable::drawOctreeNode(OctreeNode &node)
{
    if (node.isLeaf()) {
        Drawable::drawAABB(&node.bounds, Color::orange());
    }

    for (auto & i : node.children) {
        Drawable::drawOctreeNode(i);
    }
}

void Drawable::drawOctree(Octree *octree) {
    Drawable::drawAABB(&octree->root.bounds, Color::yellow());
    Drawable::drawOctreeNode(octree->root);
}

void Drawable::drawGrid3D(Grid3D *grid) {

    for (auto & box: grid->getBoxes()) {
        auto aabb = box.box;
        if (EngineSetup::get()->DRAW_MESH3D_TEST_PASSED && box.passed) {
            Drawable::drawAABB(&aabb, Color::blue());
        }

        if (EngineSetup::get()->DRAW_MESH3D_TEST_NOT_PASSED && !box.passed) {
            Drawable::drawAABB(&aabb, Color::gray());
        }
    }

    if (EngineSetup::get()->DRAW_MESH3D_GRID_ASTAR) {
        Drawable::drawGrid3DMakeTravel(grid);
    }
}

void Drawable::drawGrid3DMakeTravel(Grid3D *grid) {

    auto boxes = grid->makeTravelCubesGrid();
    for (auto &b: boxes) {
        Drawable::drawAABB(&b.box, Color::magenta());
    }

    auto pf = grid->getPathFinding();

    CubeGrid3D *cubeStart = grid->getCubeFromPosition(pf.from[0],pf.from[1],pf.from[2]);
    if (cubeStart != nullptr) {
        Drawable::drawAABB(&cubeStart->box, Color::green());
    }

    CubeGrid3D *cubeDest = grid->getCubeFromPosition(pf.to[0],pf.to[1],pf.to[2]);
    if (cubeDest != nullptr) {
        Drawable::drawAABB(&cubeDest->box, Color::red());
    }
}

void Drawable::drawObject3DGizmo(
    Object3D *o,
    glm::mat4 objectMatrix,
    glm::mat4 viewMatrix,
    glm::mat4 projectionMatrix
) {
    ImGuiIO& io = ImGui::GetIO();
    auto currentOperation  = ComponentsManager::get()->getComponentWindow()->getGuizmoOperation();

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

    if (ImGuizmo::IsUsing()) {
        if (currentOperation == ImGuizmo::OPERATION::TRANSLATE) {
            auto position = glm::vec3(objectMatrixManipulated[3]);
            o->setPosition(Vertex3D(position[0], position[1], position[2]));
        }

        if (currentOperation == ImGuizmo::OPERATION::ROTATE) {
            auto m = M3::fromMat3GLM(objectMatrixManipulated).getTranspose();
            M3::normalize(m);
            o->setRotation(m);
        }

        if (currentOperation == ImGuizmo::OPERATION::SCALE_X) {
            o->setScale(glm::length(glm::vec3(objectMatrixManipulated[0])));
        }
    }
}