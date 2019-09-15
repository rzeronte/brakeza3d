#ifndef BRAKEDA3D_RCMESHLOADEROBJ_H
#define BRAKEDA3D_RCMESHLOADEROBJ_H

class rcMeshLoaderObj {
public:
    rcMeshLoaderObj();
    ~rcMeshLoaderObj();

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
};


#endif //BRAKEDA3D_RCMESHLOADEROBJ_H
