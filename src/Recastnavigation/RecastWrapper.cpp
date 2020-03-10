
#include <cstring>
#include <math.h>
#include <DetourNavMeshBuilder.h>
#include <DetourStatus.h>
#include <DetourNavMesh.h>
#include <DetourCommon.h>
#include "../../headers/Recastnavigation/RecastWrapper.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Brakeza3D.h"

RecastWrapper::RecastWrapper():
        m_keepInterResults(true),
        m_triareas(0),
        m_solid(0),
        m_chf(0),
        m_cset(0),
        m_pmesh(0),
        m_dmesh(0),
        m_startRef(0),
        m_endRef(0)
{
    this->m_geom = new RecastGeometry();
    this->m_ctx = new RecastBuildContext();

    m_partitionType = SAMPLE_PARTITION_WATERSHED;

    m_navQuery = dtAllocNavMeshQuery();

}

bool RecastWrapper::initNavhMesh()
{
    this->cleanup();
    this->resetCommonSettings();

    m_ctx->enableLog(true);

    const float* bmin = m_geom->getNavMeshBoundsMin();
    const float* bmax = m_geom->getNavMeshBoundsMax();
    const float* verts = m_geom->getMesh()->getVerts();
    const int nverts = m_geom->getMesh()->getVertCount();
    const int* tris = m_geom->getMesh()->getTris();
    const int ntris = m_geom->getMesh()->getTriCount();
    //
    // Step 1. Initialize build config.
    //

    // Init build configuration from GUI
    memset(&m_cfg, 0, sizeof(m_cfg));
    m_cfg.cs = m_cellSize;
    m_cfg.ch = m_cellHeight;
    m_cfg.walkableSlopeAngle = m_agentMaxSlope;
    m_cfg.walkableHeight = (int)ceilf(m_agentHeight / m_cfg.ch);
    m_cfg.walkableClimb = (int)floorf(m_agentMaxClimb / m_cfg.ch);
    m_cfg.walkableRadius = (int)ceilf(m_agentRadius / m_cfg.cs);
    m_cfg.maxEdgeLen = (int)(m_edgeMaxLen / m_cellSize);
    m_cfg.maxSimplificationError = m_edgeMaxError;
    m_cfg.minRegionArea = (int)rcSqr(m_regionMinSize);		// Note: area = size*size
    m_cfg.mergeRegionArea = (int)rcSqr(m_regionMergeSize);	// Note: area = size*size
    m_cfg.maxVertsPerPoly = (int)m_vertsPerPoly;
    m_cfg.detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
    m_cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

    // Set the area where the navigation will be build.
    // Here the bounds of the input mesh are used, but the
    // area could be spexcified by an user defined box, etc.
    rcVcopy(m_cfg.bmin, bmin);
    rcVcopy(m_cfg.bmax, bmax);
    rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);

    // Reset build times gathering.
    m_ctx->resetTimers();

    // Start the build process.
    m_ctx->startTimer(RC_TIMER_TOTAL);

    m_ctx->log(RC_LOG_PROGRESS, "Building navMesh navigation:");
    m_ctx->log(RC_LOG_PROGRESS, " -> %d x %d cells", m_cfg.width, m_cfg.height);
    m_ctx->log(RC_LOG_PROGRESS, " -> %.1fK verts, %.1fK tris", nverts/1000.0f, ntris/1000.0f);

    //
    // Step 2. Rasterize input polygon soup.
    //

    // Allocate voxel heightfield where we rasterize our input data to.
    m_solid = rcAllocHeightfield();
    if (!m_solid)
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'.");
        return false;
    }
    if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield.");
        return false;
    }

    // Allocate array that can hold triangle area types.
    // If you have multiple meshes you need to process, allocate
    // and array which can hold the max number of triangles you need to process.
    m_triareas = new unsigned char[ntris];
    if (!m_triareas)
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'm_triareas' (%d).", ntris);
        return false;
    }

    // Find triangles which are walkable based on their slope and rasterize them.
    // If your input data is multiple meshes, you can transform them here, calculate
    // the are type for each of the meshes and rasterize them.
    memset(m_triareas, 0, ntris*sizeof(unsigned char));
    rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
    if (!rcRasterizeTriangles(m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, m_cfg.walkableClimb))
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not rasterize triangles.");
        return false;
    }

    if (!m_keepInterResults)
    {
        delete [] m_triareas;
        m_triareas = 0;
    }

    //
    // Step 3. Filter walkables surfaces.
    //

    // Once all geoemtry is rasterized, we do initial pass of filtering to
    // remove unwanted overhangs caused by the conservative rasterization
    // as well as filter spans where the character cannot possibly stand.
    if (m_filterLowHangingObstacles)
        rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
    if (m_filterLedgeSpans)
        rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
    if (m_filterWalkableLowHeightSpans)
        rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);


    //
    // Step 4. Partition walkable surface to simple regions.
    //

    // Compact the heightfield so that it is faster to handle from now on.
    // This will result more cache coherent data as well as the neighbours
    // between walkable cells will be calculated.
    m_chf = rcAllocCompactHeightfield();
    if (!m_chf)
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'.");
        return false;
    }
    if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf))
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build compact data.");
        return false;
    }

    if (!m_keepInterResults)
    {
        rcFreeHeightField(m_solid);
        m_solid = 0;
    }

    // Erode the walkable area by agent radius.
    if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf))
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not erode.");
        return false;
    }

    // (Optional) Mark areas.
    //const ConvexVolume* vols = m_geom->getConvexVolumes();
    //for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
    //    rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);


    // Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
    // There are 3 martitioning methods, each with some pros and cons:
    // 1) Watershed partitioning
    //   - the classic Recast partitioning
    //   - creates the nicest tessellation
    //   - usually slowest
    //   - partitions the heightfield into nice regions without holes or overlaps
    //   - the are some corner cases where this method creates produces holes and overlaps
    //      - holes may appear when a small obstacles is close to large open area (triangulation can handle this)
    //      - overlaps may occur if you have narrow spiral corridors (i.e stairs), this make triangulation to fail
    //   * generally the best choice if you precompute the nacmesh, use this if you have large open areas
    // 2) Monotone partioning
    //   - fastest
    //   - partitions the heightfield into regions without holes and overlaps (guaranteed)
    //   - creates long thin polygons, which sometimes causes paths with detours
    //   * use this if you want fast navmesh generation
    // 3) Layer partitoining
    //   - quite fast
    //   - partitions the heighfield into non-overlapping regions
    //   - relies on the triangulation code to cope with holes (thus slower than monotone partitioning)
    //   - produces better triangles than monotone partitioning
    //   - does not have the corner cases of watershed partitioning
    //   - can be slow and create a bit ugly tessellation (still better than monotone)
    //     if you have large open areas with small obstacles (not a problem if you use tiles)
    //   * good choice to use for tiled navmesh with medium and small sized tiles

    //if (m_partitionType == SAMPLE_PARTITION_WATERSHED)
    //{
        // Prepare for region partitioning, by calculating distance field along the walkable surface.
        if (!rcBuildDistanceField(m_ctx, *m_chf))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build distance field.");
            return false;
        }

        // Partition the walkable surface into simple regions without holes.
        if (!rcBuildRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build watershed regions.");
            return false;
        }
    //}
    /*else if (m_partitionType == SAMPLE_PARTITION_MONOTONE)
    {
        // Partition the walkable surface into simple regions without holes.
        // Monotone partitioning does not need distancefield.
        if (!rcBuildRegionsMonotone(m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build monotone regions.");
            return false;
        }
    }
    else // SAMPLE_PARTITION_LAYERS
    {
        // Partition the walkable surface into simple regions without holes.
        if (!rcBuildLayerRegions(m_ctx, *m_chf, 0, m_cfg.minRegionArea))
        {
            m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build layer regions.");
            return false;
        }
    }*/

    //
    // Step 5. Trace and simplify region contours.
    //

    // Create contours.
    m_cset = rcAllocContourSet();
    if (!m_cset)
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'.");
        return false;
    }
    if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset))
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not create contours.");
        return false;
    }

    //
    // Step 6. Build polygons mesh from contours.
    //

    // Build polygon navmesh from the contours.
    m_pmesh = rcAllocPolyMesh();
    if (!m_pmesh)
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'.");
        return false;
    }
    if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh))
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not triangulate contours.");
        return false;
    }

    //
    // Step 7. Create detail mesh which allows to access approximate height on each polygon.
    //

    m_dmesh = rcAllocPolyMeshDetail();
    if (!m_dmesh)
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Out of memory 'pmdtl'.");
        return false;
    }

    if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh))
    {
        m_ctx->log(RC_LOG_ERROR, "buildNavigation: Could not build detail mesh.");
        return false;
    }

    if (!m_keepInterResults)
    {
        rcFreeCompactHeightfield(m_chf);
        m_chf = 0;
        rcFreeContourSet(m_cset);
        m_cset = 0;
    }


    // At this point the navigation mesh data is ready, you can access it from m_pmesh.
    // See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

    //
    // (Optional) Step 8. Create Detour data from Recast poly mesh.
    //

    m_ctx->stopTimer(RC_TIMER_TOTAL);
}

bool RecastWrapper::initNavQuery()
{
    // The GUI may allow more max points per polygon than Detour can handle.
    // Only build the detour navmesh if we do not exceed the limit.
    if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
    {
        unsigned char* navData = 0;
        int navDataSize = 0;

        Logging::getInstance()->Log("navmesh npolys: " + std::to_string(m_pmesh->npolys));
        // Update poly flags from areas.
        for (int i = 0; i < m_pmesh->npolys; ++i)
        {
            if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
                m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

            if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
                m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
                m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
            {
                m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
            }
            else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
            {
                m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
            }
            else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
            {
                m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
            }
        }

        dtNavMeshCreateParams params = dtNavMeshCreateParams();
        memset(&params, 0, sizeof(params));
        params.verts = m_pmesh->verts;
        params.vertCount = m_pmesh->nverts;
        params.polys = m_pmesh->polys;
        params.polyAreas = m_pmesh->areas;
        params.polyFlags = m_pmesh->flags;
        params.polyCount = m_pmesh->npolys;
        params.nvp = m_pmesh->nvp;
        params.detailMeshes = m_dmesh->meshes;
        params.detailVerts = m_dmesh->verts;
        params.detailVertsCount = m_dmesh->nverts;
        params.detailTris = m_dmesh->tris;
        params.detailTriCount = m_dmesh->ntris;
        //params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
        //params.offMeshConRad = m_geom->getOffMeshConnectionRads();
        //params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
        //params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
        //params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
        //params.offMeshConUserID = m_geom->getOffMeshConnectionId();
        //params.offMeshConCount = m_geom->getOffMeshConnectionCount();
        params.walkableHeight = m_agentHeight;
        params.walkableRadius = m_agentRadius;
        params.walkableClimb = m_agentMaxClimb;
        rcVcopy(params.bmin, m_pmesh->bmin);
        rcVcopy(params.bmax, m_pmesh->bmax);
        params.cs = m_cfg.cs;
        params.ch = m_cfg.ch;
        params.buildBvTree = true;


        if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
        {
            m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
            return false;
        }

        m_navMesh = dtAllocNavMesh();
        if (!m_navMesh)
        {
            dtFree(navData);
            m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
            return false;
        }

        dtStatus status;

        status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
        if (dtStatusFailed(status))
        {
            dtFree(navData);
            m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
            return false;
        }

        status = m_navQuery->init(m_navMesh, 2048);
        if (dtStatusFailed(status))
        {
            m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
            return false;
        }
    }

    return true;
}

void RecastWrapper::getPathBetween(Vertex3D startV, Vertex3D endV, std::vector<Vertex3D> &points)
{
    points.clear();

    startV = Transforms::objectToLocal(startV, Brakeza3D::get()->getComponentsManager()->getComponentBSP()->getBSP());
    endV   = Transforms::objectToLocal(endV, Brakeza3D::get()->getComponentsManager()->getComponentBSP()->getBSP());

    startV.saveToFloat3(m_spos);
    endV.saveToFloat3(m_epos);

    m_polyPickExt[0] = 4;
    m_polyPickExt[1] = 8;
    m_polyPickExt[2] = 4;

    //m_filter.setIncludeFlags(0xFFFF) ;
    //m_filter.setExcludeFlags(0) ;
    m_filter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f) ;

    dtStatus status;

    // calc m_startRef and m_endRef
    m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);
    m_navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, &m_endRef, 0);

    status = m_navQuery->findPath(m_startRef, m_endRef, m_spos, m_epos, &m_filter, m_polys, &m_npolys, MAX_POLYS);
    if (dtStatusSucceed(status)) {
        //Logging::get()->Log("findPath Success: Num npolys: " + std::to_string(m_npolys));
    }

    // Iterate over the path to find smooth path on the detail mesh surface.
    if (m_npolys) {
        // Iterate over the path to find smooth path on the detail mesh surface.
        dtPolyRef polys[MAX_POLYS];
        memcpy(polys, m_polys, sizeof(dtPolyRef)*m_npolys);
        int npolys = m_npolys;

        float iterPos[3], targetPos[3];
        m_navQuery->closestPointOnPoly(m_startRef, m_spos, iterPos, 0);
        m_navQuery->closestPointOnPoly(polys[npolys-1], m_epos, targetPos, 0);

        static const float STEP_SIZE = 0.5f;
        static const float SLOP = 0.01f;

        m_nsmoothPath = 0;

        dtVcopy(&m_smoothPath[m_nsmoothPath*3], iterPos);
        m_nsmoothPath++;

        // Move towards target a small advancement at a time until target reached or
        // when ran out of memory to store the path.
        while (npolys && m_nsmoothPath < MAX_SMOOTH) {
            // Find location to steer towards.
            float steerPos[3];
            unsigned char steerPosFlag;
            dtPolyRef steerPosRef;

            if (!getSteerTarget(m_navQuery, iterPos, targetPos, SLOP,
                                polys, npolys, steerPos, steerPosFlag, steerPosRef))
                break;

            bool endOfPath = (steerPosFlag & DT_STRAIGHTPATH_END) ? true : false;
            bool offMeshConnection = (steerPosFlag & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ? true : false;

            // Find movement delta.
            float delta[3], len;
            dtVsub(delta, steerPos, iterPos);
            len = dtMathSqrtf(dtVdot(delta, delta));
            // If the steer target is end of path or off-mesh link, do not move past the location.
            if ((endOfPath || offMeshConnection) && len < STEP_SIZE)
                len = 1;
            else
                len = STEP_SIZE / len;
            float moveTgt[3];
            dtVmad(moveTgt, iterPos, delta, len);

            // Move
            float result[3];
            dtPolyRef visited[16];
            int nvisited = 0;
            m_navQuery->moveAlongSurface(polys[0], iterPos, moveTgt, &m_filter,
                                         result, visited, &nvisited, 16);

            npolys = fixupCorridor(polys, npolys, MAX_POLYS, visited, nvisited);
            npolys = fixupShortcuts(polys, npolys, m_navQuery);

            float h = 0;
            m_navQuery->getPolyHeight(polys[0], result, &h);
            result[1] = h;
            dtVcopy(iterPos, result);

            // Handle end of path and off-mesh links when close enough.
            if (endOfPath && inRange(iterPos, steerPos, SLOP, 1.0f))
            {
                // Reached end of path.
                dtVcopy(iterPos, targetPos);
                if (m_nsmoothPath < MAX_SMOOTH)
                {
                    dtVcopy(&m_smoothPath[m_nsmoothPath*3], iterPos);
                    m_nsmoothPath++;
                }
                break;
            }
            else if (offMeshConnection && inRange(iterPos, steerPos, SLOP, 1.0f))
            {
                // Reached off-mesh connection.
                float startPos[3], endPos[3];

                // Advance the path up to and over the off-mesh connection.
                dtPolyRef prevRef = 0, polyRef = polys[0];
                int npos = 0;
                while (npos < npolys && polyRef != steerPosRef)
                {
                    prevRef = polyRef;
                    polyRef = polys[npos];
                    npos++;
                }
                for (int i = npos; i < npolys; ++i)
                    polys[i-npos] = polys[i];
                npolys -= npos;

                // Handle the connection.
                dtStatus status = m_navMesh->getOffMeshConnectionPolyEndPoints(prevRef, polyRef, startPos, endPos);
                if (dtStatusSucceed(status))
                {
                    if (m_nsmoothPath < MAX_SMOOTH)
                    {
                        dtVcopy(&m_smoothPath[m_nsmoothPath*3], startPos);
                        m_nsmoothPath++;
                        // Hack to make the dotted path not visible during off-mesh connection.
                        if (m_nsmoothPath & 1)
                        {
                            dtVcopy(&m_smoothPath[m_nsmoothPath*3], startPos);
                            m_nsmoothPath++;
                        }
                    }
                    // Move position at the other side of the off-mesh link.
                    dtVcopy(iterPos, endPos);
                    float eh = 0.0f;
                    m_navQuery->getPolyHeight(polys[0], iterPos, &eh);
                    iterPos[1] = eh;
                }
            }

            // Store results.
            if (m_nsmoothPath < MAX_SMOOTH)
            {
                Vertex3D nV = Vertex3D(iterPos);
                Transforms::objectSpace( nV, nV, Brakeza3D::get()->getComponentsManager()->getComponentBSP()->getBSP() );
                points.push_back(nV);

                dtVcopy(&m_smoothPath[m_nsmoothPath*3], iterPos);
                m_nsmoothPath++;
            }
        }
    }
}

bool RecastWrapper::rayCasting(Vertex3D A, Vertex3D B)
{
    Vertex3D startV = Transforms::objectToLocal(A, Brakeza3D::get()->getComponentsManager()->getComponentBSP()->getBSP());
    Vertex3D endV   = Transforms::objectToLocal(B, Brakeza3D::get()->getComponentsManager()->getComponentBSP()->getBSP());

    startV.saveToFloat3(m_spos);
    endV.saveToFloat3(m_epos);

    m_polyPickExt[0] = 2;
    m_polyPickExt[1] = 4;
    m_polyPickExt[2] = 2;

    m_filter.setIncludeFlags(0xFFFF) ;
    m_filter.setExcludeFlags(0) ;
    m_filter.setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f) ;

    // calc m_startRef and m_endRef
    m_navQuery->findNearestPoly(m_spos, m_polyPickExt, &m_filter, &m_startRef, 0);
    m_navQuery->findNearestPoly(m_epos, m_polyPickExt, &m_filter, &m_endRef, 0);

    float t = 0;
    m_navQuery->raycast(m_startRef, m_spos, m_epos, &m_filter, &t, m_hitNormal, m_polys, &m_npolys, MAX_POLYS);

    return t > 1;
}

void RecastWrapper::cleanup()
{
    delete [] m_triareas;
    m_triareas = 0;
    //rcFreeHeightField(m_solid);
    m_solid = 0;
    //rcFreeCompactHeightfield(m_chf);
    m_chf = 0;
    //rcFreeContourSet(m_cset);
    m_cset = 0;
    //rcFreePolyMesh(m_pmesh);
    m_pmesh = 0;
    //rcFreePolyMeshDetail(m_dmesh);
    m_dmesh = 0;
    //dtFreeNavMesh(m_navMesh);
    m_navMesh = 0;
}


void RecastWrapper::resetCommonSettings()
{
    m_cellSize = 0.25f;
    m_cellHeight = 0.25f;
    m_agentHeight = 0.1f;
    m_agentRadius = 0.2f;
    m_agentMaxClimb = 2.f;
    m_agentMaxSlope = 45.0f;
    m_regionMinSize = 8;
    m_regionMergeSize = 20;
    m_edgeMaxLen = 12.0f;
    m_edgeMaxError = 1.3f;
    m_vertsPerPoly = 3.0f;
    m_detailSampleDist = 6.0f;
    m_detailSampleMaxError = 1.0f;
    m_partitionType = SAMPLE_PARTITION_WATERSHED;
}

void RecastWrapper::drawNavMeshPoints()
{
    const int nvp = m_pmesh->nvp;
    const float cs = m_pmesh->cs;
    const float ch = m_pmesh->ch;
    const float* orig = m_pmesh->bmin;

    for (int i = 0; i < m_pmesh->nverts; ++i) {
        const unsigned short* v = &m_pmesh->verts[i*3];
        const float x = (orig[0] + v[0]*cs);
        const float y = (orig[1] + v[1]*ch);
        const float z = (orig[2] + v[2]*cs);

        Vertex3D tmpV = Vertex3D(x, y, z);
        Transforms::objectSpace(tmpV, tmpV, Brakeza3D::get()->getComponentsManager()->getComponentBSP()->getBSP());
        Drawable::drawVertex(tmpV, Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera(), Color::red());
    }
}

void RecastWrapper::drawPathSegments(std::vector<Vertex3D> &points)
{
    Vertex3D startV;
    int count = 0;
    for ( auto it = points.begin(); it != points.end(); ++it ) {
        Vertex3D v = *(it);
        v.consoleInfo("v", false);
        if (count > 0 ) {
            Vector3D line = Vector3D(startV, v);
            Drawable::drawVector3D(line, Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera(), Color::yellow());
            //Drawable::drawVector3DZBuffer(line, brakeza3D->camera, Color::blue());
        }

        startV = v;
        count++;
        Drawable::drawVertex(v, Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera(), Color::green());
    }
}

bool RecastWrapper::getSteerTarget(dtNavMeshQuery* navQuery, const float* startPos, const float* endPos,
                           const float minTargetDist,
                           const dtPolyRef* path, const int pathSize,
                           float* steerPos, unsigned char& steerPosFlag, dtPolyRef& steerPosRef,
                           float* outPoints, int* outPointCount)
{
    // Find steer target.
    static const int MAX_STEER_POINTS = 3;
    float steerPath[MAX_STEER_POINTS*3];
    unsigned char steerPathFlags[MAX_STEER_POINTS];
    dtPolyRef steerPathPolys[MAX_STEER_POINTS];
    int nsteerPath = 0;
    navQuery->findStraightPath(startPos, endPos, path, pathSize,
                               steerPath, steerPathFlags, steerPathPolys, &nsteerPath, MAX_STEER_POINTS);
    if (!nsteerPath)
        return false;

    if (outPoints && outPointCount)
    {
        *outPointCount = nsteerPath;
        for (int i = 0; i < nsteerPath; ++i)
            dtVcopy(&outPoints[i*3], &steerPath[i*3]);
    }


    // Find vertex far enough to steer to.
    int ns = 0;
    while (ns < nsteerPath)
    {
        // Stop at Off-Mesh link or when point is further than slop away.
        if ((steerPathFlags[ns] & DT_STRAIGHTPATH_OFFMESH_CONNECTION) ||
            !inRange(&steerPath[ns*3], startPos, minTargetDist, 1000.0f))
            break;
        ns++;
    }
    // Failed to find good point to steer to.
    if (ns >= nsteerPath)
        return false;

    dtVcopy(steerPos, &steerPath[ns*3]);
    steerPos[1] = startPos[1];
    steerPosFlag = steerPathFlags[ns];
    steerPosRef = steerPathPolys[ns];

    return true;
}

int RecastWrapper::fixupCorridor(dtPolyRef* path, const int npath, const int maxPath,
                  const dtPolyRef* visited, const int nvisited)
{
    int furthestPath = -1;
    int furthestVisited = -1;

    // Find furthest common polygon.
    for (int i = npath-1; i >= 0; --i)
    {
        bool found = false;
        for (int j = nvisited-1; j >= 0; --j)
        {
            if (path[i] == visited[j])
            {
                furthestPath = i;
                furthestVisited = j;
                found = true;
            }
        }
        if (found)
            break;
    }

    // If no intersection found just return current path.
    if (furthestPath == -1 || furthestVisited == -1)
        return npath;

    // Concatenate paths.

    // Adjust beginning of the buffer to include the visited.
    const int req = nvisited - furthestVisited;
    const int orig = rcMin(furthestPath+1, npath);
    int size = rcMax(0, npath-orig);
    if (req+size > maxPath)
        size = maxPath-req;
    if (size)
        memmove(path+req, path+orig, size*sizeof(dtPolyRef));

    // Store visited
    for (int i = 0; i < req; ++i)
        path[i] = visited[(nvisited-1)-i];

    return req+size;
}

// This function checks if the path has a small U-turn, that is,
// a polygon further in the path is adjacent to the first polygon
// in the path. If that happens, a shortcut is taken.
// This can happen if the target (T) location is at tile boundary,
// and we're (S) approaching it parallel to the tile edge.
// The choice at the vertex can be arbitrary,
//  +---+---+
//  |:::|:::|
//  +-S-+-T-+
//  |:::|   | <-- the step can end up in here, resulting U-turn path.
//  +---+---+
int RecastWrapper::fixupShortcuts(dtPolyRef* path, int npath, dtNavMeshQuery* navQuery)
{
    if (npath < 3)
        return npath;

    // Get connected polygons
    static const int maxNeis = 16;
    dtPolyRef neis[maxNeis];
    int nneis = 0;

    const dtMeshTile* tile = 0;
    const dtPoly* poly = 0;
    if (dtStatusFailed(navQuery->getAttachedNavMesh()->getTileAndPolyByRef(path[0], &tile, &poly)))
        return npath;

    for (unsigned int k = poly->firstLink; k != DT_NULL_LINK; k = tile->links[k].next)
    {
        const dtLink* link = &tile->links[k];
        if (link->ref != 0)
        {
            if (nneis < maxNeis)
                neis[nneis++] = link->ref;
        }
    }

    // If any of the neighbour polygons is within the next few polygons
    // in the path, short cut to that polygon directly.
    static const int maxLookAhead = 6;
    int cut = 0;
    for (int i = dtMin(maxLookAhead, npath) - 1; i > 1 && cut == 0; i--) {
        for (int j = 0; j < nneis; j++)
        {
            if (path[i] == neis[j]) {
                cut = i;
                break;
            }
        }
    }
    if (cut > 1)
    {
        int offset = cut-1;
        npath -= offset;
        for (int i = 1; i < npath; i++)
            path[i] = path[i+offset];
    }

    return npath;
}

inline bool RecastWrapper::inRange(const float* v1, const float* v2, const float r, const float h)
{
    const float dx = v2[0] - v1[0];
    const float dy = v2[1] - v1[1];
    const float dz = v2[2] - v1[2];
    return (dx*dx + dz*dz) < r*r && fabsf(dy) < h;
}