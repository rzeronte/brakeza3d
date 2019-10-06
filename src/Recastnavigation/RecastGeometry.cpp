#include "../../headers/Recastnavigation/RecastGeometry.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Engine.h"

extern Engine *brakeza3D;

RecastGeometry::RecastGeometry() {
    m_mesh = new RecastGeometryLoader;

}

void RecastGeometry::loadBSPMapTriangles(Triangle *model_triangles, int n_triangles )
{
    int cap = 0;
    int tcap = 0;

    for (int i = 0; i < n_triangles; i++) {
        int baseIndex = i*3;

        Vertex3D A = model_triangles[i].A;
        Vertex3D B = model_triangles[i].B;
        Vertex3D C = model_triangles[i].C;

        m_mesh->addVertex(A.x, A.y, A.z, cap);
        m_mesh->addVertex(B.x, B.y, B.z, cap);
        m_mesh->addVertex(C.x, C.y, C.z, cap);

        m_mesh->addTriangle(baseIndex, baseIndex+1, baseIndex+2, tcap);
    }

    rcCalcBounds(m_mesh->getVerts(), m_mesh->getVertCount(), m_meshBMin, m_meshBMax);

    Logging::getInstance()->Log("loadFromMesh3D: getVerts: " + std::to_string(m_mesh->getVertCount()) + ", getVerts: " + std::to_string(m_mesh->getTriCount()));
}