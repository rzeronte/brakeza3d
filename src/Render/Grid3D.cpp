
#include "../../include/Render/Grid3D.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Render/Color.h"
#include "../../include/Config.h"
#include "../../include/Misc/ToolsMaths.h"
#include "../../include/Components/Components.h"
#include "../../include/OpenGL/ShaderOGLLine3D.h"
#include <SDL2/SDL_image.h>

static void emitAABBEdges(std::vector<Vector3D>& out, AABB3D& box)
{
    auto& v = box.vertices;
    out.emplace_back(v[0], v[2]);
    out.emplace_back(v[0], v[3]);
    out.emplace_back(v[0], v[4]);
    out.emplace_back(v[1], v[6]);
    out.emplace_back(v[1], v[5]);
    out.emplace_back(v[2], v[5]);
    out.emplace_back(v[2], v[6]);
    out.emplace_back(v[3], v[7]);
    out.emplace_back(v[3], v[5]);
    out.emplace_back(v[6], v[4]);
    out.emplace_back(v[7], v[4]);
    out.emplace_back(v[7], v[1]);
}

Grid3D::Grid3D(AABB3D bounds, int sizeX, int sizeY, int sizeZ)
:
    numberCubesX(sizeX),
    numberCubesY(sizeY),
    numberCubesZ(sizeZ),
    bounds(bounds),
    pathFinding(PathFinding(sizeX, sizeY, sizeZ))
{
    MakeCells();
}

void Grid3D::MakeCells()
{
    Vertex3D dimensions = bounds.max - bounds.min;

    float cubeSizeX = dimensions.x / numberCubesX;
    float cubeSizeY = dimensions.y / numberCubesY;
    float cubeSizeZ = dimensions.z / numberCubesZ;

    Vertex3D offsetPosition = Vertex3D::zero();
    for (int x = 0; x < numberCubesX; x++) {
        for (int y = 0; y < numberCubesY; y++) {
            for (int z = 0; z < numberCubesZ; z++) {
                auto cubeGrid = CubeGrid3D();
                cubeGrid.posX = x;
                cubeGrid.posY = y;
                cubeGrid.posZ = z;

                cubeGrid.box = AABB3D();
                cubeGrid.box.min = offsetPosition + Vertex3D::zero();
                cubeGrid.box.max = offsetPosition + Vertex3D(cubeSizeX, cubeSizeY, cubeSizeZ);
                cubeGrid.box.updateVertices();

                cubeGrid.box.min = cubeGrid.box.min + bounds.min;
                cubeGrid.box.max = cubeGrid.box.max + bounds.min;
                cubeGrid.box.updateVertices();
                cubeGrid.passed = true;

                boxes.push_back(cubeGrid);
                offsetPosition.z += cubeSizeZ;
            }
            offsetPosition.z = 0;
            offsetPosition.y += cubeSizeY;
        }
        offsetPosition.y = 0;
        offsetPosition.x += cubeSizeX;
    }
}

void Grid3D::doTestForNonEmptyGeometry(std::vector<Triangle *> &triangles)
{
    for (int i = 0; i < (int)this->boxes.size(); i++) {
        if (!this->isEmpty(this->boxes[i], triangles)) {
            this->boxes[i].passed = true;
        } else {
            this->boxes[i].passed = false;
        }
    }
    LoadPathFindingBlocksFromGrid();
}

bool Grid3D::isEmpty(CubeGrid3D &cube, std::vector<Triangle *> &triangles)
{
    std::vector<Plane> planes = cube.box.getPlanes();

    for (int i = 0; i < (int)triangles.size(); i++) {
        auto t = triangles[i];
        t->updateObjectSpace();

        bool r1 = Plane::isVertex3DClosedByPlanes(t->Ao, planes);
        if (r1) return false;

        bool r2 = Plane::isVertex3DClosedByPlanes(t->Bo, planes);
        if (r2) return false;

        bool r3 = Plane::isVertex3DClosedByPlanes(t->Co, planes);
        if (r3) return false;

        if (ToolsMaths::isTriangleIntersectingAABB(*t, cube.box)) {
            return false;
        }
    }

    return true;
}

CubeGrid3D *Grid3D::getCubeFromPosition(int x, int y, int z)
{
    for (unsigned int i = 0; i < boxes.size(); i++) {
        if (boxes[i].posX == x && boxes[i].posY == y && boxes[i].posZ == z) {
            return &boxes[i];
        }
    }

    return nullptr;
}

Vertex3D Grid3D::getClosestPoint(Vertex3D v, std::vector<Vertex3D> path, int &indexVertex)
{
    float min_distance = 9999999999;
    int index = 0;
    for (int i = 0; i < (int)path.size(); i++) {
        float d = ToolsMaths::distanceBetweenVertices(v, path[i]);
        if (d < min_distance) {
            min_distance = d;
            index = i;
        }
    }

    indexVertex = index;
    return path[index];
}

int Grid3D::getNumberCubesX() const {
    return numberCubesX;
}

int Grid3D::getNumberCubesY() const {
    return numberCubesY;
}

int Grid3D::getNumberCubesZ() const {
    return numberCubesZ;
}

const AABB3D &Grid3D::getBounds() const {
    return bounds;
}

const std::vector<CubeGrid3D> &Grid3D::getBoxes() const {
    return boxes;
}

void Grid3D::DrawImGuiProperties()
{
    if (ImGui::TreeNode("A* setup cells")) {
        if (ImGui::TreeNode("From")) {
            ImGui::SliderInt("X", &pathFinding.from[0], 0, getNumberCubesX() - 1);
            ImGui::SliderInt("Y", &pathFinding.from[1], 0, getNumberCubesY() - 1);
            ImGui::SliderInt("Z", &pathFinding.from[2], 0, getNumberCubesZ() - 1);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("To")) {
            ImGui::SliderInt("X", &pathFinding.to[0], 0, getNumberCubesX() - 1);
            ImGui::SliderInt("Y", &pathFinding.to[1], 0, getNumberCubesY() - 1);
            ImGui::SliderInt("Z", &pathFinding.to[2], 0, getNumberCubesZ() - 1);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    ImGui::Separator();

    static int sizeX = getNumberCubesX();
    static int sizeY = getNumberCubesY();
    static int sizeZ = getNumberCubesZ();
    ImGui::SliderInt("Size X", &sizeX, 1, 10);
    ImGui::SliderInt("Size Y", &sizeY, 1, 10);
    ImGui::SliderInt("Size Z", &sizeZ, 1, 10);

    if (ImGui::Button("Update Grid3D")) {
        Reset(sizeX, sizeY, sizeZ);
    }
}

void Grid3D::Reset(int x, int y, int z)
{
    numberCubesX = x;
    numberCubesY = y;
    numberCubesZ = z;
    boxes.clear();
    MakeCells();
    pathFinding.reset(x, y, z);
}

PathFinding Grid3D::getPathFinding()
{
    return pathFinding;
}

void Grid3D::LoadPathFindingBlocksFromGrid()
{
    for (int x = 0; x < getNumberCubesX() - 1; x++) {
        for (int y = 0; y < getNumberCubesY() - 1; y++) {
            for (int z = 0; z < getNumberCubesZ() - 1; z++) {
                CubeGrid3D *c = getCubeFromPosition(x, y, z);
                if (c->passed) {
                    pathFinding.setObstacle(x, y, z);
                }
            }
        }
    }
}

cJSON *Grid3D::getJSON()
{
    cJSON *gridJSON = cJSON_CreateObject();

    cJSON_AddNumberToObject(gridJSON, "x", getNumberCubesX());
    cJSON_AddNumberToObject(gridJSON, "y", getNumberCubesY());
    cJSON_AddNumberToObject(gridJSON, "z", getNumberCubesZ());

    return gridJSON;
}

std::vector<CubeGrid3D> Grid3D::MakeTravelCubesGrid()
{
    auto path = getPathFinding().makeTravelIndexes();

    if (path.empty()) return {};
    std::vector<CubeGrid3D> output;
    for (const auto& step : path) {
        int x = std::get<0>(step);
        int y = std::get<1>(step);
        int z = std::get<2>(step);

        auto cube = getCubeFromPosition(x, y, z);
        if (cube!= nullptr) {
            output.push_back(*cube);
        }
    }

    return output;
}

bool Grid3D::isCellWalkable(int x, int z) const
{
    if (x < 0 || x >= numberCubesX || z < 0 || z >= numberCubesZ) return false;
    int idx = x * numberCubesY * numberCubesZ + z;
    return !boxes[idx].passed;
}

std::tuple<int,int> Grid3D::snapToWalkable(int gx, int gz, int maxRadius) const
{
    if (isCellWalkable(gx, gz)) return {gx, gz};
    for (int r = 1; r <= maxRadius; r++) {
        for (int dx = -r; dx <= r; dx++) {
            for (int dz = -r; dz <= r; dz++) {
                if (std::abs(dx) == r || std::abs(dz) == r) {
                    int nx = std::max(0, std::min(numberCubesX - 1, gx + dx));
                    int nz = std::max(0, std::min(numberCubesZ - 1, gz + dz));
                    if (isCellWalkable(nx, nz)) return {nx, nz};
                }
            }
        }
    }
    return {gx, gz};
}

void Grid3D::setTravel(int x1, int y1, int z1, int x2, int y2, int z2)
{
    pathFinding.setTravel(x1, y1, z1, x2, y2, z2);
}

void Grid3D::drawDebug(Color color)
{
    if (boxes.empty()) return;

    auto* shader = Components::get()->Render()->getShaders()->shaderOGLLine3D;
    auto fbo = Components::get()->Window()->getForegroundFramebuffer();

    std::vector<Vector3D> greenLines;
    greenLines.reserve(boxes.size() * 12);
    for (auto& box : boxes) {
        if (!box.passed) {
            emitAABBEdges(greenLines, box.box);
        }
    }
    if (!greenLines.empty()) {
        shader->renderLines(greenLines, fbo, color);
    }

    if (Config::get()->DRAW_MESH3D_GRID_ASTAR) {
        auto pathBoxes = MakeTravelCubesGrid();
        std::vector<Vector3D> magentaLines;
        magentaLines.reserve(pathBoxes.size() * 12);
        for (auto& b : pathBoxes) {
            emitAABBEdges(magentaLines, b.box);
        }
        if (!magentaLines.empty()) {
            shader->renderLines(magentaLines, fbo, Color::magenta());
        }

        CubeGrid3D* cubeStart = getCubeFromPosition(pathFinding.from[0], pathFinding.from[1], pathFinding.from[2]);
        if (cubeStart != nullptr) {
            std::vector<Vector3D> startLines;
            emitAABBEdges(startLines, cubeStart->box);
            shader->renderLines(startLines, fbo, Color::green());
        }

        CubeGrid3D* cubeDest = getCubeFromPosition(pathFinding.to[0], pathFinding.to[1], pathFinding.to[2]);
        if (cubeDest != nullptr) {
            std::vector<Vector3D> destLines;
            emitAABBEdges(destLines, cubeDest->box);
            shader->renderLines(destLines, fbo, Color::red());
        }
    }
}

std::vector<CubeGrid3D> Grid3D::computePath(int gx1, int gz1, int gx2, int gz2)
{
    PathFinding pf = pathFinding;  // copia local — boxes/obstáculos son read-only tras init
    pf.setTravel(gx1, 0, gz1, gx2, 0, gz2);
    auto path = pf.makeTravelIndexes();

    std::vector<CubeGrid3D> output;
    for (const auto& step : path) {
        auto cube = getCubeFromPosition(std::get<0>(step), std::get<1>(step), std::get<2>(step));
        if (cube != nullptr) output.push_back(*cube);
    }
    return output;
}

void Grid3D::fillGrid3DFromImage(const std::string& imagePath, int threshold, bool flipZ, bool flipX)
{
    SDL_Surface* raw = IMG_Load(imagePath.c_str());
    if (!raw) {
        printf("[Grid3D] FillGrid3DFromImage: cannot load '%s': %s\n",
               imagePath.c_str(), IMG_GetError());
        return;
    }

    SDL_Surface* img = SDL_ConvertSurfaceFormat(raw, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(raw);

    if (!img) return;

    SDL_LockSurface(img);

    const int imgW = img->w;
    const int imgH = img->h;

    const Uint8* pixels = static_cast<const Uint8*>(img->pixels);
    const int bpp = img->format->BytesPerPixel;
    const int pitch = img->pitch;

    auto isObstacle = [&](int px, int py) -> bool
    {
        int offset = py * pitch + px * bpp;

        Uint8 r = pixels[offset + 0];
        Uint8 g = pixels[offset + 1];
        Uint8 b = pixels[offset + 2];

        return (r < threshold && g < threshold && b < threshold);
    };

    for (int x = 0; x < numberCubesX; x++) {
        for (int z = 0; z < numberCubesZ; z++) {

            float u = (x + 0.5f) / (float)numberCubesX;
            float v = (z + 0.5f) / (float)numberCubesZ;
            if (flipX) u = 1.0f - u;
            if (flipZ) v = 1.0f - v;

            int px = std::clamp((int)(u * imgW), 0, imgW - 1);
            int pz = std::clamp((int)(v * imgH), 0, imgH - 1);

            bool obstacle = isObstacle(px, pz);

            for (int y = 0; y < numberCubesY; y++) {
                int idx = x * numberCubesY * numberCubesZ
                        + y * numberCubesZ
                        + z;

                boxes[idx].passed = obstacle;

                if (obstacle) {
                    pathFinding.setObstacle(x, y, z);
                }
            }
        }
    }

    SDL_UnlockSurface(img);
    SDL_FreeSurface(img);

    int obstacleCount = 0;
    for (const auto& b : boxes)
        if (!b.passed) obstacleCount++;

    printf("[Grid3D] FillGrid3DFromImage: '%s' (%dx%d) grid %dx%dx%d obstacles=%d walkable=%d\n",
           imagePath.c_str(), imgW, imgH,
           numberCubesX, numberCubesY, numberCubesZ,
           obstacleCount, (int)boxes.size() - obstacleCount);
}