//
// Created by darkhead on 15/9/19.
//

#include "../../headers/Recastnavigation/RecastGeometryLoader.h"

RecastGeometryLoader::RecastGeometryLoader() :
        m_scale(1.0f),
        m_verts(0),
        m_tris(0),
        m_normals(0),
        m_vertCount(0),
        m_triCount(0)
{
}

void RecastGeometryLoader::addVertex(float x, float y, float z, int& cap)
{
    if (m_vertCount+1 > cap)
    {
        cap = !cap ? 8 : cap*2;
        float* nv = new float[cap*3];
        if (m_vertCount) {
            int t = m_vertCount*3;
            memcpy(nv, m_verts, t*sizeof(float));
        }
        delete [] m_verts;
        m_verts = nv;
    }

    float* dst = &m_verts[m_vertCount*3];
    *dst++ = x*m_scale;
    *dst++ = y*m_scale;
    *dst++ = z*m_scale;
    m_vertCount++;
}

void RecastGeometryLoader::addTriangle(int a, int b, int c, int& cap)
{
    if (m_triCount+1 > cap)
    {
        cap = !cap ? 8 : cap*2;
        int* nv = new int[cap*3];
        if (m_triCount)
            memcpy(nv, m_tris, m_triCount*3*sizeof(int));
        delete [] m_tris;
        m_tris = nv;
    }

    int* dst = &m_tris[m_triCount*3];
    *dst++ = a;
    *dst++ = b;
    *dst++ = c;
    m_triCount++;
}
