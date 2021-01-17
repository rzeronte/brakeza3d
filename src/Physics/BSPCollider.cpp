#include "BSPCollider.h"
#include "../../headers/ComponentsManager.h"

BSPCollider::BSPCollider()
{
    this->vec3_origin[0] = 0;
    this->vec3_origin[1] = 0;
    this->vec3_origin[2] = 0;

    LoadModelCollisionForWorld();
}

void BSPCollider::LoadModelCollisionForWorld() {
    Logging::getInstance()->Log("LoadModelCollisionForWorld");

    this->worldmodel = getModelCollisionFromBSP(0);
    this->playermodel = new model_collision_t;
    this->playermodel->mins[0] = 0;
    this->playermodel->mins[1] = 0;
    this->playermodel->mins[2] = 0;
    this->playermodel->maxs[0] = 1;
    this->playermodel->maxs[1] = 1;
    this->playermodel->maxs[2] = 1;
}

model_collision_t *BSPCollider::getWorldModel()
{
    return this->worldmodel;
}

model_collision_t *BSPCollider::getPlayerModel()
{
    return this->playermodel;
}

model_collision_t *BSPCollider::getModelCollisionFromBSP(int modelId)
{
    Logging::getInstance()->Log("getModelCollisionFromBSP for modelId: " + std::to_string(modelId));

    BSPMap *bspMap = ComponentsManager::get()->getComponentBSP()->getBSP();

    auto *model = new model_collision_t;           // Destiny
    model_t *bspModel = bspMap->getModel(modelId); // Source

    model->modelindex = modelId;
    model->solid = SOLID_BSP;

    model->planes = (mplane_s *) bspMap->getPlanes();
    model->numplanes = bspMap->getNumPlanes();

    Mod_LoadLeafs(model);
    Mod_LoadNodes(model);

    model->origin[0] = bspModel->origin[0];
    model->origin[1] = bspModel->origin[1];
    model->origin[2] = bspModel->origin[2];

    this->makeHull0(model);
    this->makeHulls(model);

    /*Logging::getInstance()->Log(
        "model_collision_t: \r\n"
        "* modelindex: " + std::to_string(model->modelindex) + "\r\n"
        "* numnodes: " + std::to_string(model->numnodes) + "\r\n"
        "* numplanes: " + std::to_string(model->numplanes) + "\r\n"
        "* nummarksurfaces: " + std::to_string(model->nummarksurfaces) + "\r\n"
        "* hull[0].firstclipnode: " + std::to_string(model->hulls[0].firstclipnode) + "\r\n"
        "* hull[0].lastclipnode: " + std::to_string(model->hulls[0].lastclipnode) + "\r\n"
        "* hull[1].firstclipnode: " + std::to_string(model->hulls[1].firstclipnode) + "\r\n"
        "* hull[1].lastclipnode: " + std::to_string(model->hulls[1].lastclipnode) + "\r\n"
        "* hull[2].firstclipnode: " + std::to_string(model->hulls[2].firstclipnode) + "\r\n"
        "* hull[2].lastclipnode: " + std::to_string(model->hulls[2].lastclipnode)
    );*/

    return model;
}

void BSPCollider::makeHull0(model_collision_t *model) {
    BSPMap *bspMap = ComponentsManager::get()->getComponentBSP()->getBSP();
    int count = model->numnodes;

    auto *out = new clipnode_t[count];
    mnode_t *in = model->nodes;

    //hull0
    model->hulls[0].clipnodes = out;
    model->hulls[0].firstclipnode = 0;
    model->hulls[0].lastclipnode = count - 1;
    model->hulls[0].planes = model->planes;

    mnode_t	*child;
    for (int i = 0 ; i < count ; i++, in++, out++) {
        out->planenum = in->plane - model->planes;
        for (int j = 0 ; j < 2 ; j++)
        {
            child = in->children[j];
            if (child->contents < 0)
                out->children[j] = child->contents;
            else
                out->children[j] = child - model->nodes;
        }
    }
}

void BSPCollider::makeHulls(model_collision_t *model) {
    BSPMap *bspMap = ComponentsManager::get()->getComponentBSP()->getBSP();
    int count = bspMap->getNumClipNodes();

    auto *out = new clipnode_t[count];

    //hull1
    model->hulls[1].clipnodes = out;
    model->hulls[1].firstclipnode = 0;
    model->hulls[1].lastclipnode = count - 1;
    model->hulls[1].planes = (mplane_s *) bspMap->getPlanes();
    model->hulls[1].clip_mins[0] = -16;
    model->hulls[1].clip_mins[1] = -16;
    model->hulls[1].clip_mins[2] = -24;
    model->hulls[1].clip_maxs[0] = 16;
    model->hulls[1].clip_maxs[1] = 16;
    model->hulls[1].clip_maxs[2] = 32;

    //hull2
    model->hulls[2].clipnodes = out;
    model->hulls[2].firstclipnode = 0;
    model->hulls[2].lastclipnode = count - 1;
    model->hulls[2].planes = (mplane_s *) bspMap->getPlanes();
    model->hulls[2].clip_mins[0] = -32;
    model->hulls[2].clip_mins[1] = -32;
    model->hulls[2].clip_mins[2] = -24;
    model->hulls[2].clip_maxs[0] = 32;
    model->hulls[2].clip_maxs[1] = 32;
    model->hulls[2].clip_maxs[2] = 64;

    //hull3
    model->hulls[3].clipnodes = out;
    model->hulls[3].firstclipnode = 0;
    model->hulls[3].lastclipnode = count - 1;
    model->hulls[3].planes = model->planes;

    clipnode_t* in = bspMap->getClipNodes();
    for (int i = 0 ; i < count ; i++, in++, out++) {
        out->planenum = (in->planenum);
        out->children[0] = (in->children[0]);
        out->children[1] = (in->children[1]);
    }
}


void BSPCollider::CrossProduct (vec3_t v1, vec3_t v2, vec3_t cross)
{
    cross[0] = v1[1]*v2[2] - v1[2]*v2[1];
    cross[1] = v1[2]*v2[0] - v1[0]*v2[2];
    cross[2] = v1[0]*v2[1] - v1[1]*v2[0];
}
void BSPCollider::VectorScale (vec3_t in, vec_t scale, vec3_t out)
{
    out[0] = in[0]*scale;
    out[1] = in[1]*scale;
    out[2] = in[2]*scale;
}

void BSPCollider::SV_MoveBounds (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, vec3_t boxmins, vec3_t boxmaxs)
{
    int		i;

    for (i=0 ; i<3 ; i++)
    {
        if (end[i] > start[i])
        {
            boxmins[i] = start[i] + mins[i] - 1;
            boxmaxs[i] = end[i] + maxs[i] + 1;
        }
        else
        {
            boxmins[i] = end[i] + mins[i] - 1;
            boxmaxs[i] = start[i] + maxs[i] + 1;
        }
    }
}

/*
================
SV_HullForEntity

Returns a hull that can be used for testing or clipping an object of mins/maxs
size.
Offset is filled in to contain the adjustment that must be added to the
testing object's origin to get a point to use with the returned hull.
================
*/
hull_t *BSPCollider::SV_HullForEntity (model_collision_t *ent, vec3_t mins, vec3_t maxs, vec3_t offset)
{
    model_collision_t		*model;
    vec3_t		size;
    vec3_t		hullmins, hullmaxs;
    hull_t		*hull;

    // decide which clipping hull to use, based on the size
    // explicit hulls in the BSP model

    model = ent;
    //model = sv.models[ (int)ent->v.modelindex ];

    VectorSubtract (maxs, mins, size);

    if (size[0] < 3)
        hull = &model->hulls[0];
    else if (size[0] <= 32)
        hull = &model->hulls[1];
    else
        hull = &model->hulls[2];

    // calculate an offset value to center the origin
    VectorSubtract (hull->clip_mins, mins, offset);
    VectorAdd (offset, ent->origin, offset);


    return hull;
}

int BSPCollider::SV_HullPointContents (hull_t *hull, int num, vec3_t p)
{
    float		d;
    clipnode_t	*node;
    mplane_t	*plane;

    while (num >= 0) {
        if (num < hull->firstclipnode || num > hull->lastclipnode) {
            Logging::getInstance()->Log ("SV_HullPointContents: bad node number");
            exit(-1);
        }
        node = hull->clipnodes + num;
        plane = hull->planes + node->planenum;

        if (plane->type < 3)
            d = p[plane->type] - plane->dist;
        else
            d = DotProduct (plane->normal, p) - plane->dist;
        if (d < 0)
            num = node->children[1];
        else
            num = node->children[0];
    }

    return num;
}

bool BSPCollider::SV_RecursiveHullCheck (hull_t *hull, int hullFirstClipNode, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace)
{
    clipnode_t	*node;
    mplane_t	*plane;
    float		t1, t2;
    float		frac;
    int			i;
    vec3_t		mid;
    int			side;
    float		midf;

    // check for empty
    if (hullFirstClipNode < 0) {
        if (hullFirstClipNode != CONTENTS_SOLID) {
            trace->allsolid = false;
            if (hullFirstClipNode == CONTENTS_EMPTY)
                trace->inopen = true;
            else
                trace->inwater = true;
        } else {
            trace->startsolid = true;
        }
        return true;		// empty
    }

    if (hullFirstClipNode < hull->firstclipnode || hullFirstClipNode > hull->lastclipnode) {
        Logging::getInstance()->Log("SV_RecursiveHullCheck: bad node number (" + std::to_string(hullFirstClipNode)+ ")");
        Logging::getInstance()->Log("SV_RecursiveHullCheck: hull->firstclipnode: " + std::to_string(hull->firstclipnode));
        Logging::getInstance()->Log("SV_RecursiveHullCheck: hull->lastclipnode: " + std::to_string(hull->lastclipnode));
        exit(-1);
    }
    // find the point distances
    node = hull->clipnodes + hullFirstClipNode;
    plane = hull->planes + node->planenum;

    if (plane->type < 3) {
        t1 = p1[plane->type] - plane->dist;
        t2 = p2[plane->type] - plane->dist;
    } else {
        t1 = DotProduct (plane->normal, p1) - plane->dist;
        t2 = DotProduct (plane->normal, p2) - plane->dist;
    }

    if (t1 >= 0 && t2 >= 0) {
        return SV_RecursiveHullCheck(hull, node->children[0], p1f, p2f, p1, p2, trace);
    }

    if (t1 < 0 && t2 < 0) {
        return SV_RecursiveHullCheck (hull, node->children[1], p1f, p2f, p1, p2, trace);
    }

    // put the crosspoint DIST_EPSILON pixels on the near side
    if (t1 < 0) {
        frac = (t1 + DIST_EPSILON)/(t1-t2);
    } else {
        frac = (t1 - DIST_EPSILON)/(t1-t2);
    }

    if (frac < 0) {
        frac = 0;
    }

    if (frac > 1) {
        frac = 1;
    }

    midf = p1f + (p2f - p1f)*frac;
    for (i=0 ; i<3 ; i++) {
        mid[i] = p1[i] + frac*(p2[i] - p1[i]);
    }
    side = (t1 < 0);

    // move up to the node
    if (!SV_RecursiveHullCheck (hull, node->children[side], p1f, midf, p1, mid, trace) ) {
        return false;
    }

    // go past the node
    if (SV_HullPointContents (hull, node->children[side^1], mid) != CONTENTS_SOLID) {
        return SV_RecursiveHullCheck (hull, node->children[side^1], midf, p2f, mid, p2, trace);
    }

    if (trace->allsolid) {
        return false;		// never got out of the solid area
    }

    //==================
    // the other side of the node is solid, this is the impact point
    //==================
    if (!side) {
        VectorCopy (plane->normal, trace->plane.normal);
        trace->plane.dist = plane->dist;
    } else {
        VectorSubtract (vec3_origin, plane->normal, trace->plane.normal);
        trace->plane.dist = -plane->dist;
    }

    // shouldn't really happen, but does occasionally
    while (SV_HullPointContents (hull, hull->firstclipnode, mid) == CONTENTS_SOLID) {
        frac -= 0.1;
        if (frac < 0) {
            trace->fraction = midf;
            VectorCopy (mid, trace->endpos);
            return false;
        }
        midf = p1f + (p2f - p1f)*frac;
        for (i=0 ; i<3 ; i++) {
            mid[i] = p1[i] + frac*(p2[i] - p1[i]);
        }
    }

    trace->fraction = midf;
    VectorCopy (mid, trace->endpos);

    return false;
}

/*
==================
SV_ClipMoveToEntity

Handles selection or creation of a clipping hull, and offseting (and
eventually rotation) of the end points
==================
*/
trace_t BSPCollider::SV_ClipMoveToEntity (model_collision_t *ent, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end)
{
    trace_t		trace;
    vec3_t		offset;
    vec3_t		start_l, end_l;
    hull_t		*hull;

    // fill in a default trace
    memset (&trace, 0, sizeof(trace_t));
    trace.fraction = 1;
    trace.allsolid = true;
    VectorCopy (end, trace.endpos);

    // get the clipping hull
    hull = SV_HullForEntity (ent, mins, maxs, offset);

    VectorSubtract (start, offset, start_l);
    VectorSubtract (end, offset, end_l);

    // trace a line through the apropriate clipping hull
    SV_RecursiveHullCheck (hull, hull->firstclipnode, 0, 1, start_l, end_l, &trace);

    // fix trace up by the offset
    if (trace.fraction != 1)
    VectorAdd (trace.endpos, offset, trace.endpos);

    // did we clip the move?
    if (trace.fraction < 1 || trace.startsolid  )
        trace.ent = ent;

    return trace;
}

trace_t BSPCollider::SV_Move (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int type)
{
    moveclip_t	clip;
    int			i;

    memset ( &clip, 0, sizeof ( moveclip_t ) );

    clip.trace = SV_ClipMoveToEntity ( getWorldModel(), start, mins, maxs, end );

    clip.start = start;
    clip.end = end;
    clip.mins = mins;
    clip.maxs = maxs;
    clip.type = type;

    if (type == MOVE_MISSILE) {
        for (i=0 ; i<3 ; i++) {
            clip.mins2[i] = -15;
            clip.maxs2[i] = 15;
        }
    } else {
        VectorCopy (mins, clip.mins2);
        VectorCopy (maxs, clip.maxs2);
    }

    // create the bounding box of the entire move
    SV_MoveBounds ( start, clip.mins2, clip.maxs2, end, clip.boxmins, clip.boxmaxs );

    // clip to entities
    //SV_ClipToLinks ( sv_areanodes, &clip );

    return clip.trace;
}


/*
==================
ClipVelocity

Slide off of the impacting object
returns the blocked flags (1 = floor, 2 = step / wall)
==================
*/
#define	STOP_EPSILON	0.1

int BSPCollider::ClipVelocity (vec3_t in, vec3_t normal, vec3_t out, float overbounce)
{
    float	backoff;
    float	change;
    int		i, blocked;

    blocked = 0;
    if (normal[2] > 0)
        blocked |= 1;		// floor
    if (!normal[2])
        blocked |= 2;		// step

    backoff = DotProduct (in, normal) * overbounce;

    for (i=0 ; i<3 ; i++)
    {
        change = normal[i]*backoff;
        out[i] = in[i] - change;
        if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
            out[i] = 0;
    }

    return blocked;
}

/*The basic solid body movement clip that slides along multiple planes
        Returns the clipflags if the velocity was modified (hit something solid)
1 = floor
2 = wall / step
4 = dead stop
If steptrace is not NULL, the trace of any vertical wall hit will be stored
============
*/
#define	MAX_CLIP_PLANES	5
int BSPCollider::SV_FlyMove (model_collision_t *ent, float time, trace_t *steptrace)
{
    int			bumpcount, numbumps;
    vec3_t		dir;
    float		d;
    int			numplanes;
    vec3_t		planes[MAX_CLIP_PLANES];
    vec3_t		primal_velocity, original_velocity, new_velocity;
    int			i, j;
    trace_t		trace;
    vec3_t		end;
    float		time_left;
    int			blocked;

    numbumps = 4;

    blocked = 0;
    VectorCopy (ent->velocity, original_velocity);
    VectorCopy (ent->velocity, primal_velocity);
    numplanes = 0;

    time_left = time;

    for (bumpcount=0 ; bumpcount<numbumps ; bumpcount++) {
        if (!ent->velocity[0] && !ent->velocity[1] && !ent->velocity[2]) {
            Logging::getInstance()->Log("SV_FlyMove: all zero");
            break;
        }

        for (i=0 ; i<3 ; i++) {
            end[i] = ent->origin[i] + time_left * ent->velocity[i];
        }

        trace = SV_Move (ent->origin, ent->mins, ent->maxs, end, false);

        // entity is trapped in another solid
        if (trace.allsolid) {
            Logging::getInstance()->Log("SV_FlyMove: trace.allsolid");
            VectorCopy (vec3_origin, ent->velocity);
            return 3;
        }

        if (trace.fraction > 0) {	// actually covered some distance
            Logging::getInstance()->Log("SV_FlyMove: trace.fraction > 0");
            VectorCopy (trace.endpos, ent->origin);
            VectorCopy (ent->velocity, original_velocity);
            numplanes = 0;
        }

        if (trace.fraction == 1) {
            Logging::getInstance()->Log("SV_FlyMove: trace.fraction == 1");
            break;		// moved the entire distance
        }

        if (!trace.ent) {
            Logging::getInstance()->Log("SV_FlyMove: !trace.ent");
        }

        if (trace.plane.normal[2] > 0.7) {
            blocked |= 1;		// floor
            if (trace.ent->solid == SOLID_BSP) {
                ent->flags =	(int)ent->flags | FL_ONGROUND;
            }
        }

        if (!trace.plane.normal[2]) {
            blocked |= 2;		// step
            if (steptrace) {
                *steptrace = trace;	// save for player extrafriction
            }
        }

        // run the impact function
        //SV_Impact (ent, trace.ent);
        if (ent->free)
            break;		// removed by the impact function


        time_left -= time_left * trace.fraction;

        // cliped to another plane
        if (numplanes >= MAX_CLIP_PLANES) {	// this shouldn't really happen
            VectorCopy (vec3_origin, ent->velocity);
            return 3;
        }

        VectorCopy (trace.plane.normal, planes[numplanes]);
        numplanes++;

        // modify original_velocity so it parallels all of the clip planes
        for (i=0 ; i<numplanes ; i++) {
            ClipVelocity (original_velocity, planes[i], new_velocity, 1);
            for (j=0 ; j<numplanes ; j++)
                if (j != i) {
                    if (DotProduct (new_velocity, planes[j]) < 0)
                        break;	// not ok
                }
            if (j == numplanes)
                break;
        }

        if (i != numplanes) {	// go along this plane
            VectorCopy (new_velocity, ent->velocity);
        } else {	// go along the crease
            if (numplanes != 2) {
//				Con_Printf ("clip velocity, numplanes == %i\n",numplanes);
                VectorCopy (vec3_origin, ent->velocity);
                return 7;
            }
            CrossProduct (planes[0], planes[1], dir);
            d = DotProduct (dir, ent->velocity);
            VectorScale (dir, d, ent->velocity);
        }

        // if original velocity is against the original velocity, stop dead
        // to avoid tiny occilations in sloping corners
        if (DotProduct (ent->velocity, primal_velocity) <= 0) {
            VectorCopy (vec3_origin, ent->velocity);
            return blocked;
        }
    }

    Logging::getInstance()->Log("return SV_FlyMove final código");

    return blocked;
}

void BSPCollider::InsertLinkBefore (link_t *l, link_t *before)
{
    l->next = before;
    l->prev = before->prev;
    l->prev->next = l;
    l->next->prev = l;
}

void BSPCollider::RemoveLink (link_t *l)
{
    l->next->prev = l->prev;
    l->prev->next = l->next;
}

void BSPCollider::SV_UnlinkEdict (model_collision_t *ent)
{
    if (!ent->area.prev)
        return;		// not linked in anywhere
    RemoveLink (&ent->area);
    ent->area.prev = ent->area.next = NULL;
}

void BSPCollider::SV_FindTouchedLeafs (model_collision_t *ent, mnode_t *node)
{
    mplane_t	*splitplane;
    mleaf_t 	*leaf;
    int			sides;
    int			leafnum;

    if (node->contents == CONTENTS_SOLID)
        return;

    // add an efrag if the node is a leaf

    if ( node->contents < 0)
    {
        if (ent->num_leafs == MAX_ENT_LEAFS)
            return;

        model_collision_t *worldmodelleafs = getModelCollisionFromBSP(0);

        leaf = (mleaf_t *)node;
        leafnum = leaf - worldmodelleafs->leafs - 1;

        ent->leafnums[ent->num_leafs] = leafnum;
        ent->num_leafs++;
        return;
    }

    /*
    // NODE_MIXED

    splitplane = node->plane;
    sides = BoxOnPlaneSide(ent->v.absmin, ent->v.absmax, splitplane);

    // recurse down the contacted sides
    if (sides & 1)
        SV_FindTouchedLeafs (ent, node->children[0]);

    if (sides & 2)
        SV_FindTouchedLeafs (ent, node->children[1]);
    */
}

void BSPCollider::SV_LinkEdict (model_collision_t *ent, bool touch_triggers)
{
    areanode_t	*node;

    if (ent->area.prev)
        SV_UnlinkEdict (ent);	// unlink from old position

    //if (ent == sv.edicts)
    //    return;		// don't add the world

    if (ent->free)
        return;

// set the abs box

    VectorAdd (ent->origin, ent->mins, ent->absmin);
    VectorAdd (ent->origin, ent->maxs, ent->absmax);

//
// to make items easier to pick up and allow them to be grabbed off
// of shelves, the abs sizes are expanded
//
    if ((int)ent->flags & FL_ITEM)
    {
        ent->absmin[0] -= 15;
        ent->absmin[1] -= 15;
        ent->absmax[0] += 15;
        ent->absmax[1] += 15;
    }
    else
    {	// because movement is clipped an epsilon away from an actual edge,
        // we must fully check even when bounding boxes don't quite touch
        ent->absmin[0] -= 1;
        ent->absmin[1] -= 1;
        ent->absmin[2] -= 1;
        ent->absmax[0] += 1;
        ent->absmax[1] += 1;
        ent->absmax[2] += 1;
    }

    // link to PVS leafs
    ent->num_leafs = 0;
    if (ent->modelindex)
        SV_FindTouchedLeafs (ent, getModelCollisionFromBSP(0)->nodes); //sv.worldmodel->nodes

    if (ent->solid == SOLID_NOT)
        return;

    // find the first node that the ent's box crosses
    node = sv_areanodes;
    while (1)
    {
        if (node->axis == -1)
            break;
        if (ent->absmin[node->axis] > node->dist)
            node = node->children[0];
        else if (ent->absmax[node->axis] < node->dist)
            node = node->children[1];
        else
            break;		// crosses the node
    }

    // link it in
    if (ent->solid == SOLID_TRIGGER)
        InsertLinkBefore (&ent->area, &node->trigger_edicts);
    else
        InsertLinkBefore (&ent->area, &node->solid_edicts);

    // if touch_triggers, touch all entities at this node and decend for more
    if (touch_triggers) {
        //SV_TouchLinks ( ent, sv_areanodes );
    }
}


trace_t BSPCollider::SV_PushEntity (model_collision_t *ent, vec3_t push)
{
    trace_t	trace;
    vec3_t	end;

    VectorAdd (ent->origin, push, end);

    if (ent->movetype == MOVETYPE_FLYMISSILE)
        trace = SV_Move (ent->origin, ent->mins, ent->maxs, end, MOVE_MISSILE);
    else if (ent->solid == SOLID_TRIGGER || ent->solid == SOLID_NOT)
        // only clip against bmodels
        trace = SV_Move (ent->origin, ent->mins, ent->maxs, end, MOVE_NOMONSTERS);
    else
        trace = SV_Move (ent->origin, ent->mins, ent->maxs, end, MOVE_NORMAL);

    VectorCopy (trace.endpos, ent->origin);
    SV_LinkEdict (ent, true);

    //if (trace.ent)
    //    SV_Impact (ent, trace.ent);

    return trace;
}


void BSPCollider::AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up)
{
    float		angle;
    float		sr, sp, sy, cr, cp, cy;

    angle = angles[YAW] * (M_PI*2 / 360);
    sy = sin(angle);
    cy = cos(angle);
    angle = angles[PITCH] * (M_PI*2 / 360);
    sp = sin(angle);
    cp = cos(angle);
    angle = angles[ROLL] * (M_PI*2 / 360);
    sr = sin(angle);
    cr = cos(angle);

    forward[0] = cp*cy;
    forward[1] = cp*sy;
    forward[2] = -sp;
    right[0] = (-1*sr*sp*cy+-1*cr*-sy);
    right[1] = (-1*sr*sp*sy+-1*cr*cy);
    right[2] = -1*sr*cp;
    up[0] = (cr*sp*cy+-sr*-sy);
    up[1] = (cr*sp*sy+-sr*cy);
    up[2] = cr*cp;
}

void BSPCollider::SV_WallFriction (model_collision_t *ent, trace_t *trace)
{
    vec3_t		forward, right, up;
    float		d, i;
    vec3_t		into, side;

    AngleVectors (ent->v_angle, forward, right, up);
    d = DotProduct (trace->plane.normal, forward);

    d += 0.5;
    if (d >= 0)
        return;

    // cut the tangential velocity
    i = DotProduct (trace->plane.normal, ent->velocity);
    VectorScale (trace->plane.normal, i, into);
    VectorSubtract (ent->velocity, into, side);

    ent->velocity[0] = side[0] * (1 + d);
    ent->velocity[1] = side[1] * (1 + d);
}

/*
=====================
SV_TryUnstick

Player has come to a dead stop, possibly due to the problem with limited
float precision at some angle joins in the BSP hull.

Try fixing by pushing one pixel in each direction.

This is a hack, but in the interest of good gameplay...
======================
*/
int BSPCollider::SV_TryUnstick (model_collision_t *ent, vec3_t oldvel)
{
    int		i;
    vec3_t	oldorg;
    vec3_t	dir;
    int		clip;
    trace_t	steptrace;

    VectorCopy (ent->origin, oldorg);
    VectorCopy (vec3_origin, dir);

    for (i=0 ; i<8 ; i++) {
    // try pushing a little in an axial direction
        switch (i)
        {
            case 0:	dir[0] = 2; dir[1] = 0; break;
            case 1:	dir[0] = 0; dir[1] = 2; break;
            case 2:	dir[0] = -2; dir[1] = 0; break;
            case 3:	dir[0] = 0; dir[1] = -2; break;
            case 4:	dir[0] = 2; dir[1] = 2; break;
            case 5:	dir[0] = -2; dir[1] = 2; break;
            case 6:	dir[0] = 2; dir[1] = -2; break;
            case 7:	dir[0] = -2; dir[1] = -2; break;
        }

        SV_PushEntity (ent, dir);

        // retry the original move
        ent->velocity[0] = oldvel[0];
        ent->velocity[1] = oldvel[1];
        ent->velocity[2] = 0;
        clip = SV_FlyMove (ent, 0.1, &steptrace);

        if ( fabs(oldorg[1] - ent->origin[1]) > 4
             || fabs(oldorg[0] - ent->origin[0]) > 4 )
        {
        //Con_DPrintf ("unstuck!\n");
            return clip;
        }

    // go back to the original pos and try again
        VectorCopy (oldorg, ent->origin);
    }

    VectorCopy (vec3_origin, ent->velocity);
    return 7;		// still not moving
}

void BSPCollider:: SV_WalkMove (model_collision_t *ent, float deltaTime)
{
    Logging::getInstance()->Log("SV_WalkMove start ");

    vec3_t		upmove, downmove;
    vec3_t		oldorg, oldvel;
    vec3_t		nosteporg, nostepvel;
    int			clip;
    int			oldonground;
    trace_t		steptrace, downtrace;

    // do a regular slide move unless it looks like you ran into a step

    // Creo que invierte el flag que hubiera para onground
    oldonground = (int)ent->flags & FL_ONGROUND;
    ent->flags = (int)ent->flags & ~FL_ONGROUND;

    // Almacena la posición original y la velocidad de la entidad antes de hacerle ningún cambio
    VectorCopy (ent->origin, oldorg);
    VectorCopy (ent->velocity, oldvel);

    // Intentamos hacer el movimiento fly completo y almacenamos la traza
    // 1 = floor, 2 = wall / step, 4 = dead stop
    clip = SV_FlyMove (ent, deltaTime, &steptrace);
    Logging::getInstance()->Log("SV_FlyMove: clip -> " + std::to_string(clip));

    // Si el movimiento no se ha bloqueado, nos vamos
    if ( !(clip & 2) )  { // Esto se cumple cuando no es 2
        Logging::getInstance()->Log("Salimos WalkMove: clip -> " + std::to_string(clip));
        return;		// move didn't block on a step
    }
    // Hemos chocado con 1 floor o 4 dead stop
    if (!oldonground && ent->waterlevel == 0) {
        Logging::getInstance()->Log("Salimos WalkMove: clip !oldonground && waterlevel -> " + std::to_string(clip));
        return;		// don't stair up while jumping
    }

    VectorCopy (ent->origin, nosteporg);
    VectorCopy (ent->velocity, nostepvel);

    // try moving up and forward to go up a step
    VectorCopy (oldorg, ent->origin);	// back to start pos

    VectorCopy (vec3_origin, upmove);
    VectorCopy (vec3_origin, downmove);
    upmove[2] = STEPSIZE;
    downmove[2] = -STEPSIZE + oldvel[2]*deltaTime;

    // move up
    SV_PushEntity (ent, upmove);	// FIXME: don't link?

    // move forward
    ent->velocity[0] = oldvel[0];
    ent->velocity[1] = oldvel[1];
    ent->velocity[2] = 0;
    clip = SV_FlyMove (ent, deltaTime, &steptrace);

    // check for stuckness, possibly due to the limited precision of floats
    // in the clipping hulls
    if (clip) {
        if ( fabs(oldorg[1] - ent->origin[1]) < 0.03125
             && fabs(oldorg[0] - ent->origin[0]) < 0.03125 )
        {	// stepping up didn't make any progress
            clip = SV_TryUnstick (ent, oldvel);
        }
    }

    // extra friction based on view angle
    if ( clip & 2 ) {
        SV_WallFriction (ent, &steptrace);
    }

    // move down
    downtrace = SV_PushEntity (ent, downmove);	// FIXME: don't link?

    if (downtrace.plane.normal[2] > 0.7) {
        if (ent->solid == SOLID_BSP)
        {
            ent->flags =	(int)ent->flags | FL_ONGROUND;
        }
    } else {
        // if the push down didn't end up on good ground, use the move without
        // the step up.  This happens near wall / slope combinations, and can
        // cause the player to hop up higher on a slope too steep to climb
        VectorCopy (nosteporg, ent->origin);
        VectorCopy (nostepvel, ent->velocity);
    }
}

/*
BoxOnPlaneSide
Returns 1, 2, or 1 + 2
*/
int BSPCollider::BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, mplane_t *p)
{
    if ((p)->type < 3) {
        if ((p)->dist <= (emins)[(p)->type]) {
            return 1;
        } else {
            if ((p)->dist >= (emaxs)[(p)->type])
                return 2;
            else
                return 3;
        }
    } else {
            float	dist1, dist2;
            int		sides;

            // general case
            switch (p->signbits)
            {
                case 0:
                    dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
                    dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
                    break;
                case 1:
                    dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
                    dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
                    break;
                case 2:
                    dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
                    dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
                    break;
                case 3:
                    dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
                    dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
                    break;
                case 4:
                    dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
                    dist2 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
                    break;
                case 5:
                    dist1 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emins[2];
                    dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emaxs[2];
                    break;
                case 6:
                    dist1 = p->normal[0]*emaxs[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
                    dist2 = p->normal[0]*emins[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
                    break;
                case 7:
                    dist1 = p->normal[0]*emins[0] + p->normal[1]*emins[1] + p->normal[2]*emins[2];
                    dist2 = p->normal[0]*emaxs[0] + p->normal[1]*emaxs[1] + p->normal[2]*emaxs[2];
                    break;
                default:
                    dist1 = dist2 = 0;		// shut up compiler
                    break;
            }

            sides = 0;
            if (dist1 >= p->dist)
                sides = 1;
            if (dist2 < p->dist)
                sides |= 2;

            return sides;
    }
}

void BSPCollider::Mod_LoadLeafs (model_collision_t *loadmodel)
{
    Logging::getInstance()->Log("Mod_LoadLeafs for loadmodel: " + std::to_string(loadmodel->modelindex));

    bspleaf_t *in;
    int	i, j, count, p;

    in = ComponentsManager::get()->getComponentBSP()->getBSP()->getLeaves();

    count = ComponentsManager::get()->getComponentBSP()->getBSP()->getNumLeaves();
    auto *out = new mleaf_t [count];

    loadmodel->leafs = out;
    loadmodel->numleafs = count;

    for ( i = 0 ; i < count ; i++, in++, out++)
    {
        for (j=0 ; j<3 ; j++)
        {
            out->minmaxs[j] =  (in->sbox.min[j]);
            out->minmaxs[3+j] =  (in->sbox.max[j]);
        }

        p = (in->type);
        out->contents = p;

        out->firstmarksurface = loadmodel->marksurfaces + (in->firstsurf);
        out->nummarksurfaces = (in->numsurf);

        p = (in->vislist);
        if (p == -1)
            out->compressed_vis = NULL;
        else
            out->compressed_vis = loadmodel->visdata + p;

        // loadmodel->leafs->efrags = NULL; ???

        for (j=0 ; j<4 ; j++)
            out->ambient_sound_level[j] = in->ambientsnd[j];
    }
}

void BSPCollider::Mod_LoadNodes (model_collision_t *loadmodel)
{

    bspnode_t *in = ComponentsManager::get()->getComponentBSP()->getBSP()->getNodes();
    int count = ComponentsManager::get()->getComponentBSP()->getBSP()->getNumNodes();
    auto *out = new mnode_t[count];

    loadmodel->nodes = out;
    loadmodel->numnodes = count;

    for ( int i = 0 ; i < count ; i++, in++, out++) {
        out->plane = new mplane_t;
        for (int j = 0 ; j < 3 ; j++) {
            out->minmaxs[j] = (in->sbox.min[j]);
            out->minmaxs[3+j] = (in->sbox.max[j]);
        }

        int p = (in->planenum);
        out->plane = loadmodel->planes + p;

        out->firstsurface = (in->firstsurf);
        out->numsurfaces = (in->numsurf);

        for (int j = 0 ; j < 2 ; j++) {
            short children[2];
            children[0] = in->front;
            children[1] = in->back;
            p = (children[j]);
            if (p >= 0)
                out->children[j] = loadmodel->nodes + p;
            else
                out->children[j] = (mnode_t *)(loadmodel->leafs + (-1 - p));
        }
    }

    Mod_SetParent (loadmodel->nodes, NULL);	// sets nodes and leafs
}

void BSPCollider::Mod_SetParent (mnode_t *node, mnode_t *parent)
{
    node->parent = parent;
    if (node->contents < 0)
        return;
    Mod_SetParent (node->children[0], node);
    Mod_SetParent (node->children[1], node);
}


void BSPCollider::SV_AddGravity (model_collision_t *ent, float deltaTime)
{
    float ent_gravity = 1.0f;
    float gravity = 9.8;
    ent->velocity[2] -= ent_gravity * gravity * deltaTime;
}

void BSPCollider::SV_CheckStuck (model_collision_t *ent)
{
    int		i, j;
    int		z;
    vec3_t	org;

    if (!SV_TestEntityPosition(ent))
    {
        VectorCopy (ent->origin, ent->oldorigin);
        return;
    }

    VectorCopy (ent->origin, org);
    VectorCopy (ent->oldorigin, ent->origin);
    if (!SV_TestEntityPosition(ent))
    {
        Logging::getInstance()->Log ("Unstuck.\n");
        SV_LinkEdict (ent, true);
        return;
    }

    for (z=0 ; z< 18 ; z++)
        for (i=-1 ; i <= 1 ; i++)
            for (j=-1 ; j <= 1 ; j++)
            {
                ent->origin[0] = org[0] + i;
                ent->origin[1] = org[1] + j;
                ent->origin[2] = org[2] + z;
                if (!SV_TestEntityPosition(ent))
                {
                    Logging::getInstance()->Log("Unstuck.\n");
                    SV_LinkEdict (ent, true);
                    return;
                }
            }

    VectorCopy (org, ent->origin);
    Logging::getInstance()->Log ("player is stuck.\n");
}

model_collision_t *BSPCollider::SV_TestEntityPosition (model_collision_t *ent)
{
    trace_t	trace;

    trace = SV_Move (ent->origin, ent->mins, ent->maxs, ent->origin, 0);

    if (trace.startsolid)
        return getWorldModel();

    return NULL;
}

void BSPCollider::drawHull(model_collision_t *model, int indexHull)
{
    hull_t hull = model->hulls[indexHull];
    BSPMap *bspMap = ComponentsManager::get()->getComponentBSP()->getBSP();

    for (int i = 0; i < model->numnodes; i++) {
        mnode_t node = model->nodes[i];
        for (int j = node.firstsurface; j < node.firstsurface + node.numsurfaces; j++) {
            //bspMap->DrawSurfaceTriangles( j );
        }
        AABB3D a;
        a.min.x = node.minmaxs[0];
        a.min.y = node.minmaxs[1];
        a.min.z = node.minmaxs[2];
        a.max.x = node.minmaxs[3];
        a.max.y = node.minmaxs[4];
        a.max.z = node.minmaxs[5];

        a.vertices[0] = a.max;
        a.vertices[1] = a.min;
        a.vertices[2] = Vertex3D(a.max.x, a.max.y, a.min.z);
        a.vertices[3] = Vertex3D(a.max.x, a.min.y, a.max.z);
        a.vertices[4] = Vertex3D(a.min.x, a.max.y, a.max.z);
        a.vertices[5] = Vertex3D(a.max.x, a.min.y, a.min.z);
        a.vertices[6] = Vertex3D(a.min.x, a.max.y, a.min.z);
        a.vertices[7] = Vertex3D(a.min.x, a.min.y, a.max.z);
        Object3D *p = new Object3D();
        Drawable::drawAABB(&a,p);
    }
}
