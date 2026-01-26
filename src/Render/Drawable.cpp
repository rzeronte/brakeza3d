#define GL_GLEXT_PROTOTYPES

#include "../../include/Render/Drawable.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Brakeza.h"
#include "ImGuizmo.h"
#include <glm/gtc/type_ptr.hpp>
#include "imgui_internal.h"
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/FileSystemGUI.h"

void Drawable::drawVertex(const Vertex3D &V, Color color)
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

void Drawable::drawVector3D(const Vector3D &V, const Color &color)
{
    Components::get()->Render()->DrawLine(V.vertex1,V.vertex2, color);
}

void Drawable::drawMainAxisOffset(const Vertex3D &offset)
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

    auto axis_x = Vector3D(VXstart, VXend);
    auto axis_y = Vector3D(VYstart, VYend);
    auto axis_z = Vector3D(VZstart, VZend);

    drawVector3D(axis_x, Color::red());
    drawVector3D(axis_y, Color::green());
    drawVector3D(axis_z, Color::blue());
}

void Drawable::drawObject3DAxis(Object3D *object, bool drawUp, bool drawRight, bool drawForward)
{
    if (!Frustum::isVertexInside(object->getPosition())) {
        return;
    }

    const float size = Config::get()->OBJECT_AXIS_SIZE;
    Vertex3D endRight = object->getPosition() + object->right().getScaled(size);
    Vertex3D endUp = object->getPosition() + object->up().getScaled(size);
    Vertex3D endForward = object->getPosition() + object->forward().getScaled(size);

    auto vRight = Vector3D(object->getPosition(), endRight);
    auto vUp = Vector3D(object->getPosition(), endUp);

    auto vForward = Vector3D(object->getPosition(), endForward);
    if (drawRight) drawVector3D(vRight, Color::green());
    if (drawUp) drawVector3D(vUp, Color::red());
    if (drawForward) drawVector3D(vForward, Color::blue());
}

void Drawable::drawLightning(Vertex3D A, Vertex3D B, Color color) {

    float generations = Config::get()->LIGHTNING_GENERATIONS;
    std::vector<Vector3D> segmentList;
    std::vector<Vector3D> tmpList;
    std::vector<Vector3D> newSegments;

    float offsetAmount = Config::get()->LIGHTNING_OFFSET_REDUCTION;
    float multiplier = Config::get()->LIGHTNING_SEGMENT_SHIFT;
    float probabilityBranch = Config::get()->LIGHTNING_PROBABILITY_BRANCH;

    segmentList.emplace_back(A, B);

    for (int i = 0; i < static_cast<int>(generations); i++) {
        tmpList = segmentList;
        int j = 0;
        for (auto ir = tmpList.begin(); ir != tmpList.end(); ++j) {
            Vertex3D midPoint = ir.base()->middlePoint();

            midPoint.x += static_cast<float>(Tools::random(-offsetAmount, offsetAmount)) * multiplier;
            midPoint.y += static_cast<float>(Tools::random(-offsetAmount, offsetAmount)) * multiplier;
            midPoint.z += static_cast<float>(Tools::random(-offsetAmount, offsetAmount)) * multiplier;

            if (Tools::random(1, 10) > 10 - probabilityBranch) {

                Vertex3D splitEnd;
                Vertex3D direction = ir.base()->getComponent().getNormalize().getScaled(offsetAmount * 2);
                splitEnd.x = midPoint.x + direction.x;
                splitEnd.y = midPoint.y + direction.y;
                splitEnd.z = midPoint.z + direction.z;

                auto segmentBranch = Vector3D(midPoint, splitEnd);
                newSegments.push_back(segmentBranch);
            }

            auto segment1 = Vector3D(ir.base()->vertex1, midPoint);
            auto segment2 = Vector3D(midPoint, ir.base()->vertex2);
            newSegments.push_back(segment1);
            newSegments.push_back(segment2);

            ++ir;
        }
        segmentList = newSegments;
        newSegments.clear();

        offsetAmount /= 2;
    }
    for ( auto ir = segmentList.begin(); ir != segmentList.end(); ++ir) {
        drawVector3D(*ir.base(), color);
    }
}

void Drawable::drawAABB(AABB3D *aabb, const Color &color)
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

    auto window = Components::get()->Window();
    Components::get()->Render()->getShaders()->shaderOGLLine3D->renderLines(vectors, window->getForegroundFramebuffer(),color);
}

void Drawable::drawOctreeNode(OctreeNode &node)
{
    if (node.isLeaf()) {
        drawAABB(&node.bounds, Color::orange());
    }

    for (auto & i : node.children) {
        drawOctreeNode(i);
    }
}

void Drawable::drawOctree(Octree *octree)
{
    drawAABB(&octree->root.bounds, Color::yellow());
    drawOctreeNode(octree->root);
}

void Drawable::drawGrid3D(Grid3D *grid)
{

    for (auto & box: grid->getBoxes()) {
        auto aabb = box.box;
        if (Config::get()->DRAW_MESH3D_TEST_PASSED && box.passed) {
            drawAABB(&aabb, Color::blue());
        }

        if (Config::get()->DRAW_MESH3D_TEST_NOT_PASSED && !box.passed) {
            drawAABB(&aabb, Color::gray());
        }
    }

    if (Config::get()->DRAW_MESH3D_GRID_ASTAR) {
        drawGrid3DMakeTravel(grid);
    }
}

void Drawable::drawGrid3DMakeTravel(Grid3D *grid) {

    auto boxes = grid->MakeTravelCubesGrid();
    for (auto &b: boxes) {
        drawAABB(&b.box, Color::magenta());
    }

    auto pf = grid->getPathFinding();

    CubeGrid3D *cubeStart = grid->getCubeFromPosition(pf.from[0],pf.from[1],pf.from[2]);
    if (cubeStart != nullptr) {
        drawAABB(&cubeStart->box, Color::green());
    }

    CubeGrid3D *cubeDest = grid->getCubeFromPosition(pf.to[0],pf.to[1],pf.to[2]);
    if (cubeDest != nullptr) {
        drawAABB(&cubeDest->box, Color::red());
    }
}

void Drawable::WarningMessage(const char *msg)
{
    ImGui::Image(FileSystemGUI::Icon(IconGUI::WARNING), GUIType::Sizes::ICONS_BROWSERS);
    ImGui::SameLine();
    ImGui::TextDisabled("%s", msg);
    ImGui::Spacing();
}

void Drawable::drawObject3DGizmo(
    Object3D *o,
    const glm::mat4 &objectMatrix,
    glm::mat4 viewMatrix,
    glm::mat4 projectionMatrix
) {
    ImGuiIO& io = ImGui::GetIO();
    auto currentOperation  = Components::get()->Window()->getGuiZmoOperation();

    if (!Components::get()->Window()->isWindowMaximized()) {
        ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    } else {
        ImGuizmo::SetRect(
            ImGui::GetWindowPos().x,
            ImGui::GetWindowPos().y,
            ImGui::GetWindowWidth(),
            ImGui::GetWindowHeight()
        );
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

void Drawable::ImGuiLink(const char* label, const char* url)
{
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.6f, 1.0f, 1.0f)); // Color azul
    ImGui::Text("%s", label);
    ImGui::PopStyleColor();

    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        ImGui::SetTooltip("%s", url);
    }

    if (ImGui::IsItemClicked()) {
        #ifdef _WIN32
            std::string command = "start " + std::string(url);
        #elif __linux__
            std::string command = "xdg-open " + std::string(url);
        #elif __APPLE__
            std::string command = "open " + std::string(url);
        #endif
        system(command.c_str());
    }
}