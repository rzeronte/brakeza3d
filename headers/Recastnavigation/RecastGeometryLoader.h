#ifndef BRAKEDA3D_RCMESHLOADEROBJ_H
#define BRAKEDA3D_RCMESHLOADEROBJ_H

#include <stdio.h>
#include <string.h>

class RecastGeometryLoader {
public:
    RecastGeometryLoader();
    ~RecastGeometryLoader();

    float m_scale;
    float* m_verts;
    int* m_tris;
    float* m_normals;
    int m_vertCount;
    int m_triCount;

    const float* getVerts() const { return m_verts; }
    const float* getNormals() const { return m_normals; }
    const int* getTris() const { return m_tris; }
    int getVertCount() const { return m_vertCount; }
    int getTriCount() const { return m_triCount; }

    void addVertex(float x, float y, float z, int& cap);
    void addTriangle(int a, int b, int c, int& cap);
};


#endif //BRAKEDA3D_RCMESHLOADEROBJ_H
