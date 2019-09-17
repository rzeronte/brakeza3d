
#ifndef BRAKEDA3D_INPUTGEOM_H
#define BRAKEDA3D_INPUTGEOM_H


#include "RecastWrapper.h"
#include "InputGeom.h"
#include "rcMeshLoaderObj.h"
#include "Objects/Mesh3D.h"


struct BuildSettings
{
    // Cell size in world units
    float cellSize;
    // Cell height in world units
    float cellHeight;
    // Agent height in world units
    float agentHeight;
    // Agent radius in world units
    float agentRadius;
    // Agent max climb in world units
    float agentMaxClimb;
    // Agent max slope in degrees
    float agentMaxSlope;
    // Region minimum size in voxels.
    // regionMinSize = sqrt(regionMinArea)
    float regionMinSize;
    // Region merge size in voxels.
    // regionMergeSize = sqrt(regionMergeArea)
    float regionMergeSize;
    // Edge max length in world units
    float edgeMaxLen;
    // Edge max error in voxels
    float edgeMaxError;
    float vertsPerPoly;
    // Detail sample distance in voxels
    float detailSampleDist;
    // Detail sample max error in voxel heights.
    float detailSampleMaxError;
    // Partition type, see SamplePartitionType
    int partitionType;
    // Bounds of the area to mesh
    float navMeshBMin[3];
    float navMeshBMax[3];
    // Size of the tiles in voxels
    float tileSize;
};

class InputGeom {
public:
    InputGeom();

    rcMeshLoaderObj* m_mesh;
    float m_meshBMin[3], m_meshBMax[3];
    BuildSettings m_buildSettings;
    bool m_hasBuildSettings;

    void loadFromMesh3D(Triangle *model_triangles, int n_triangles = 0);

    /// @name Off-Mesh connections.
    ///@{
    static const int MAX_OFFMESH_CONNECTIONS = 256;
    float m_offMeshConVerts[MAX_OFFMESH_CONNECTIONS*3*2];
    float m_offMeshConRads[MAX_OFFMESH_CONNECTIONS];
    unsigned char m_offMeshConDirs[MAX_OFFMESH_CONNECTIONS];
    unsigned char m_offMeshConAreas[MAX_OFFMESH_CONNECTIONS];
    unsigned short m_offMeshConFlags[MAX_OFFMESH_CONNECTIONS];
    unsigned int m_offMeshConId[MAX_OFFMESH_CONNECTIONS];
    int m_offMeshConCount;
    ///@}

    /// Method to return static mesh data.
    const rcMeshLoaderObj* getMesh() const { return m_mesh; }
    const float* getMeshBoundsMin() const { return m_meshBMin; }
    const float* getMeshBoundsMax() const { return m_meshBMax; }
    const float* getNavMeshBoundsMin() const { return m_hasBuildSettings ? m_buildSettings.navMeshBMin : m_meshBMin; }
    const float* getNavMeshBoundsMax() const { return m_hasBuildSettings ? m_buildSettings.navMeshBMax : m_meshBMax; }
    const BuildSettings* getBuildSettings() const { return m_hasBuildSettings ? &m_buildSettings : 0; }
    bool raycastMesh(float* src, float* dst, float& tmin);

    /// @name Off-Mesh connections.
    ///@{
    int getOffMeshConnectionCount() const { return m_offMeshConCount; }
    const float* getOffMeshConnectionVerts() const { return m_offMeshConVerts; }
    const float* getOffMeshConnectionRads() const { return m_offMeshConRads; }
    const unsigned char* getOffMeshConnectionDirs() const { return m_offMeshConDirs; }
    const unsigned char* getOffMeshConnectionAreas() const { return m_offMeshConAreas; }
    const unsigned short* getOffMeshConnectionFlags() const { return m_offMeshConFlags; }
    const unsigned int* getOffMeshConnectionId() const { return m_offMeshConId; }

};


#endif //BRAKEDA3D_INPUTGEOM_H
