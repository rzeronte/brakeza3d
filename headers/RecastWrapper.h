//
// Created by darkhead on 15/9/19.
//

#ifndef BRAKEDA3D_RECASTWRAPPER_H
#define BRAKEDA3D_RECASTWRAPPER_H


#include <Recast.h>
#include "InputGeom.h"
#include "DetourPathQueue.h"

static const int MAX_CONVEXVOL_PTS = 12;
//static const int DT_VERTS_PER_POLYGON = 6;

struct ConvexVolume
{
    float verts[MAX_CONVEXVOL_PTS*3];
    float hmin, hmax;
    int nverts;
    int area;
};

enum SamplePartitionType
{
    SAMPLE_PARTITION_WATERSHED,
    SAMPLE_PARTITION_MONOTONE,
    SAMPLE_PARTITION_LAYERS,
};

enum SamplePolyAreas
{
    SAMPLE_POLYAREA_GROUND,
    SAMPLE_POLYAREA_WATER,
    SAMPLE_POLYAREA_ROAD,
    SAMPLE_POLYAREA_DOOR,
    SAMPLE_POLYAREA_GRASS,
    SAMPLE_POLYAREA_JUMP,
};

enum SamplePolyFlags
{
    SAMPLE_POLYFLAGS_WALK		= 0x01,		// Ability to walk (ground, grass, road)
    SAMPLE_POLYFLAGS_SWIM		= 0x02,		// Ability to swim (water).
    SAMPLE_POLYFLAGS_DOOR		= 0x04,		// Ability to move through doors.
    SAMPLE_POLYFLAGS_JUMP		= 0x08,		// Ability to jump.
    SAMPLE_POLYFLAGS_DISABLED	= 0x10,		// Disabled polygon
    SAMPLE_POLYFLAGS_ALL		= 0xffff	// All abilities.
};

class RecastWrapper {
public:

    unsigned char* m_triareas;
    rcHeightfield* m_solid;
    rcCompactHeightfield* m_chf;
    rcContourSet* m_cset;
    rcPolyMesh* m_pmesh;
    rcConfig m_cfg;
    rcPolyMeshDetail* m_dmesh;

    class InputGeom* m_geom;

    class dtNavMesh* m_navMesh;
    class dtNavMeshQuery* m_navQuery;

    float m_cellSize;
    float m_cellHeight;
    float m_agentHeight;
    float m_agentRadius;
    float m_agentMaxClimb;
    float m_agentMaxSlope;
    float m_regionMinSize;
    float m_regionMergeSize;
    float m_edgeMaxLen;
    float m_edgeMaxError;
    float m_vertsPerPoly;
    float m_detailSampleDist;
    float m_detailSampleMaxError;
    int m_partitionType;

    rcContext* m_ctx;


    bool m_keepInterResults;

    bool m_filterLowHangingObstacles;
    bool m_filterLedgeSpans;
    bool m_filterWalkableLowHeightSpans;

    bool handleBuild();


};


#endif //BRAKEDA3D_RECASTWRAPPER_H
