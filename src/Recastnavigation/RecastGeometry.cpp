#include "../../headers/Recastnavigation/RecastGeometry.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Engine.h"

RecastGeometry::RecastGeometry() {
    m_mesh = new RecastGeometryLoader;

}

void RecastGeometry::loadBSPMapTriangles(std::vector<Triangle*> &triangles )
{


    int cap = 0;
    int tcap = 0;

    for (int i = 0; i < triangles.size(); i++) {
        int baseIndex = i*3;

        Vertex3D A = triangles[i]->A;
        Vertex3D B = triangles[i]->B;
        Vertex3D C = triangles[i]->C;

        m_mesh->addVertex(A.x, A.y, A.z, cap);
        m_mesh->addVertex(B.x, B.y, B.z, cap);
        m_mesh->addVertex(C.x, C.y, C.z, cap);

        m_mesh->addTriangle(baseIndex, baseIndex+1, baseIndex+2, tcap);
    }

    rcCalcBounds(m_mesh->getVerts(), m_mesh->getVertCount(), m_meshBMin, m_meshBMax);

    Logging::getInstance()->Log("loadFromMesh3D: getVerts: " + std::to_string(m_mesh->getVertCount()) + ", getVerts: " + std::to_string(m_mesh->getTriCount()));
}