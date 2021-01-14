#include "BSPCollider.h"
#include "../../headers/ComponentsManager.h"

BSPCollider::BSPCollider()
{
    this->vec3_origin[0] = 0;
    this->vec3_origin[1] = 0;
    this->vec3_origin[2] = 0;
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

    model = ComponentsManager::get()->getComponentBSP()->getModelCollisionFromBSP(ent->modelindex);
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
    dclipnode_t	*node;
    mplane_t	*plane;

    while (num >= 0)
    {
        if (num < hull->firstclipnode || num > hull->lastclipnode)
            Logging::getInstance()->Log ("SV_HullPointContents: bad node number");

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

bool BSPCollider::SV_RecursiveHullCheck (hull_t *hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace)
{
    dclipnode_t	*node;
    mplane_t	*plane;
    float		t1, t2;
    float		frac;
    int			i;
    vec3_t		mid;
    int			side;
    float		midf;

// check for empty
    if (num < 0)
    {
        if (num != CONTENTS_SOLID)
        {
            trace->allsolid = false;
            if (num == CONTENTS_EMPTY)
                trace->inopen = true;
            else
                trace->inwater = true;
        }
        else
            trace->startsolid = true;
        return true;		// empty
    }

    if (num < hull->firstclipnode || num > hull->lastclipnode)
        Logging::getInstance()->Log("SV_RecursiveHullCheck: bad node number");

//
// find the point distances
//
    node = hull->clipnodes + num;
    plane = hull->planes + node->planenum;

    if (plane->type < 3)
    {
        t1 = p1[plane->type] - plane->dist;
        t2 = p2[plane->type] - plane->dist;
    }
    else
    {
        t1 = DotProduct (plane->normal, p1) - plane->dist;
        t2 = DotProduct (plane->normal, p2) - plane->dist;
    }

    if (t1 >= 0 && t2 >= 0)
        return SV_RecursiveHullCheck (hull, node->children[0], p1f, p2f, p1, p2, trace);
    if (t1 < 0 && t2 < 0)
        return SV_RecursiveHullCheck (hull, node->children[1], p1f, p2f, p1, p2, trace);


// put the crosspoint DIST_EPSILON pixels on the near side
    if (t1 < 0)
        frac = (t1 + DIST_EPSILON)/(t1-t2);
    else
        frac = (t1 - DIST_EPSILON)/(t1-t2);
    if (frac < 0)
        frac = 0;
    if (frac > 1)
        frac = 1;

    midf = p1f + (p2f - p1f)*frac;
    for (i=0 ; i<3 ; i++)
        mid[i] = p1[i] + frac*(p2[i] - p1[i]);

    side = (t1 < 0);

// move up to the node
    if (!SV_RecursiveHullCheck (hull, node->children[side], p1f, midf, p1, mid, trace) )
        return false;


    if (SV_HullPointContents (hull, node->children[side^1], mid)
        != CONTENTS_SOLID)
// go past the node
        return SV_RecursiveHullCheck (hull, node->children[side^1], midf, p2f, mid, p2, trace);

    if (trace->allsolid)
        return false;		// never got out of the solid area

    //==================
    // the other side of the node is solid, this is the impact point
    //==================
    if (!side)
    {
        VectorCopy (plane->normal, trace->plane.normal);
        trace->plane.dist = plane->dist;
    }
    else
    {
        VectorSubtract (vec3_origin, plane->normal, trace->plane.normal);
        trace->plane.dist = -plane->dist;
    }

    while (SV_HullPointContents (hull, hull->firstclipnode, mid)
           == CONTENTS_SOLID)
    { // shouldn't really happen, but does occasionally
        frac -= 0.1;
        if (frac < 0)
        {
            trace->fraction = midf;
            VectorCopy (mid, trace->endpos);
            return false;
        }
        midf = p1f + (p2f - p1f)*frac;
        for (i=0 ; i<3 ; i++)
            mid[i] = p1[i] + frac*(p2[i] - p1[i]);
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

    // clip to world
    model_collision_t *worldmodel = ComponentsManager::get()->getComponentBSP()->getModelCollisionFromBSP( 0 );

    clip.trace = SV_ClipMoveToEntity ( worldmodel, start, mins, maxs, end );

    clip.start = start;
    clip.end = end;
    clip.mins = mins;
    clip.maxs = maxs;
    clip.type = type;

    if (type == MOVE_MISSILE)
    {
        for (i=0 ; i<3 ; i++)
        {
            clip.mins2[i] = -15;
            clip.maxs2[i] = 15;
        }
    }
    else
    {
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
int BSPCollider::SV_FlyMove (edict_t *ent, float time, trace_t *steptrace)
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
    VectorCopy (ent->v.velocity, original_velocity);
    VectorCopy (ent->v.velocity, primal_velocity);
    numplanes = 0;

    time_left = time;

    for (bumpcount=0 ; bumpcount<numbumps ; bumpcount++)
    {
        if (!ent->v.velocity[0] && !ent->v.velocity[1] && !ent->v.velocity[2])
            break;

        for (i=0 ; i<3 ; i++)
            end[i] = ent->v.origin[i] + time_left * ent->v.velocity[i];

        trace = SV_Move (ent->v.origin, ent->v.mins, ent->v.maxs, end, false);

        if (trace.allsolid)
        {	// entity is trapped in another solid
            VectorCopy (vec3_origin, ent->v.velocity);
            return 3;
        }

        if (trace.fraction > 0)
        {	// actually covered some distance
            VectorCopy (trace.endpos, ent->v.origin);
            VectorCopy (ent->v.velocity, original_velocity);
            numplanes = 0;
        }

        if (trace.fraction == 1)
            break;		// moved the entire distance

        if (!trace.ent)
            Logging::getInstance()->Log("SV_FlyMove: !trace.ent");

        if (trace.plane.normal[2] > 0.7)
        {
            blocked |= 1;		// floor
            //if (trace.ent->v.solid == SOLID_BSP)
            //{
                ent->v.flags =	(int)ent->v.flags | FL_ONGROUND;
            //}
        }
        if (!trace.plane.normal[2])
        {
            blocked |= 2;		// step
            if (steptrace)
                *steptrace = trace;	// save for player extrafriction
        }

        //
        // run the impact function
        //
        //SV_Impact (ent, trace.ent);
        if (ent->free)
            break;		// removed by the impact function


        time_left -= time_left * trace.fraction;

        // cliped to another plane
        if (numplanes >= MAX_CLIP_PLANES)
        {	// this shouldn't really happen
            VectorCopy (vec3_origin, ent->v.velocity);
            return 3;
        }

        VectorCopy (trace.plane.normal, planes[numplanes]);
        numplanes++;

        //
        // modify original_velocity so it parallels all of the clip planes
        //
        for (i=0 ; i<numplanes ; i++)
        {
            ClipVelocity (original_velocity, planes[i], new_velocity, 1);
            for (j=0 ; j<numplanes ; j++)
                if (j != i)
                {
                    if (DotProduct (new_velocity, planes[j]) < 0)
                        break;	// not ok
                }
            if (j == numplanes)
                break;
        }

        if (i != numplanes)
        {	// go along this plane
            VectorCopy (new_velocity, ent->v.velocity);
        }
        else
        {	// go along the crease
            if (numplanes != 2)
            {
//				Con_Printf ("clip velocity, numplanes == %i\n",numplanes);
                VectorCopy (vec3_origin, ent->v.velocity);
                return 7;
            }
            CrossProduct (planes[0], planes[1], dir);
            d = DotProduct (dir, ent->v.velocity);
            VectorScale (dir, d, ent->v.velocity);
        }

        //
        // if original velocity is against the original velocity, stop dead
        // to avoid tiny occilations in sloping corners
        //
        if (DotProduct (ent->v.velocity, primal_velocity) <= 0)
        {
            VectorCopy (vec3_origin, ent->v.velocity);
            return blocked;
        }
    }

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

void BSPCollider::SV_UnlinkEdict (edict_t *ent)
{
    if (!ent->area.prev)
        return;		// not linked in anywhere
    RemoveLink (&ent->area);
    ent->area.prev = ent->area.next = NULL;
}

void BSPCollider::SV_FindTouchedLeafs (edict_t *ent, mnode_t *node)
{
    mplane_t	*splitplane;
    bspleaf_t	*leaf;
    int			sides;
    int			leafnum;

    if (node->contents == CONTENTS_SOLID)
        return;

// add an efrag if the node is a leaf

    /*if ( node->contents < 0)
    {
        if (ent->num_leafs == MAX_ENT_LEAFS)
            return;

        model_t *worldmodelleafs = ComponentsManager::get()->getComponentBSP()->getBSP()->getModel(0);

        leaf = (bspleaf_t *)node;
        //leafnum = leaf - sv.worldmodel->leafs - 1;
        leafnum = leaf - worldmodelleafs-> - 1;

        ent->leafnums[ent->num_leafs] = leafnum;
        ent->num_leafs++;
        return;
    }*/

// NODE_MIXED

    splitplane = node->plane;
    sides = BoxOnPlaneSide(ent->v.absmin, ent->v.absmax, splitplane);

// recurse down the contacted sides
    if (sides & 1)
        SV_FindTouchedLeafs (ent, node->children[0]);

    if (sides & 2)
        SV_FindTouchedLeafs (ent, node->children[1]);
}

void BSPCollider::SV_LinkEdict (edict_t *ent, bool touch_triggers)
{
    areanode_t	*node;

    if (ent->area.prev)
        SV_UnlinkEdict (ent);	// unlink from old position

    //if (ent == sv.edicts)
    //    return;		// don't add the world

    if (ent->free)
        return;

// set the abs box

    VectorAdd (ent->v.origin, ent->v.mins, ent->v.absmin);
    VectorAdd (ent->v.origin, ent->v.maxs, ent->v.absmax);

//
// to make items easier to pick up and allow them to be grabbed off
// of shelves, the abs sizes are expanded
//
    if ((int)ent->v.flags & FL_ITEM)
    {
        ent->v.absmin[0] -= 15;
        ent->v.absmin[1] -= 15;
        ent->v.absmax[0] += 15;
        ent->v.absmax[1] += 15;
    }
    else
    {	// because movement is clipped an epsilon away from an actual edge,
        // we must fully check even when bounding boxes don't quite touch
        ent->v.absmin[0] -= 1;
        ent->v.absmin[1] -= 1;
        ent->v.absmin[2] -= 1;
        ent->v.absmax[0] += 1;
        ent->v.absmax[1] += 1;
        ent->v.absmax[2] += 1;
    }

// link to PVS leafs
    ent->num_leafs = 0;
    if (ent->v.modelindex)
        SV_FindTouchedLeafs (ent, ComponentsManager::get()->getComponentBSP()->worldNodes); //sv.worldmodel->nodes

    if (ent->v.solid == SOLID_NOT)
        return;

// find the first node that the ent's box crosses
    node = sv_areanodes;
    while (1)
    {
        if (node->axis == -1)
            break;
        if (ent->v.absmin[node->axis] > node->dist)
            node = node->children[0];
        else if (ent->v.absmax[node->axis] < node->dist)
            node = node->children[1];
        else
            break;		// crosses the node
    }

// link it in

    if (ent->v.solid == SOLID_TRIGGER)
        InsertLinkBefore (&ent->area, &node->trigger_edicts);
    else
        InsertLinkBefore (&ent->area, &node->solid_edicts);

    // if touch_triggers, touch all entities at this node and decend for more
    //if (touch_triggers)
    //    SV_TouchLinks ( ent, sv_areanodes );
}


trace_t BSPCollider::SV_PushEntity (edict_t *ent, vec3_t push)
{
    trace_t	trace;
    vec3_t	end;

    VectorAdd (ent->v.origin, push, end);

    if (ent->v.movetype == MOVETYPE_FLYMISSILE)
        trace = SV_Move (ent->v.origin, ent->v.mins, ent->v.maxs, end, MOVE_MISSILE);
    else if (ent->v.solid == SOLID_TRIGGER || ent->v.solid == SOLID_NOT)
        // only clip against bmodels
        trace = SV_Move (ent->v.origin, ent->v.mins, ent->v.maxs, end, MOVE_NOMONSTERS);
    else
        trace = SV_Move (ent->v.origin, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL);

    VectorCopy (trace.endpos, ent->v.origin);
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

void BSPCollider::SV_WallFriction (edict_t *ent, trace_t *trace)
{
    vec3_t		forward, right, up;
    float		d, i;
    vec3_t		into, side;

    AngleVectors (ent->v.v_angle, forward, right, up);
    d = DotProduct (trace->plane.normal, forward);

    d += 0.5;
    if (d >= 0)
        return;

// cut the tangential velocity
    i = DotProduct (trace->plane.normal, ent->v.velocity);
    VectorScale (trace->plane.normal, i, into);
    VectorSubtract (ent->v.velocity, into, side);

    ent->v.velocity[0] = side[0] * (1 + d);
    ent->v.velocity[1] = side[1] * (1 + d);
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
int BSPCollider::SV_TryUnstick (edict_t *ent, vec3_t oldvel)
{
    int		i;
    vec3_t	oldorg;
    vec3_t	dir;
    int		clip;
    trace_t	steptrace;

    VectorCopy (ent->v.origin, oldorg);
    VectorCopy (vec3_origin, dir);

    for (i=0 ; i<8 ; i++)
    {
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
        ent->v.velocity[0] = oldvel[0];
        ent->v. velocity[1] = oldvel[1];
        ent->v. velocity[2] = 0;
        clip = SV_FlyMove (ent, 0.1, &steptrace);

        if ( fabs(oldorg[1] - ent->v.origin[1]) > 4
             || fabs(oldorg[0] - ent->v.origin[0]) > 4 )
        {
//Con_DPrintf ("unstuck!\n");
            return clip;
        }

// go back to the original pos and try again
        VectorCopy (oldorg, ent->v.origin);
    }

    VectorCopy (vec3_origin, ent->v.velocity);
    return 7;		// still not moving
}

void BSPCollider::SV_WalkMove (edict_t *ent, float deltaTime)
{
    vec3_t		upmove, downmove;
    vec3_t		oldorg, oldvel;
    vec3_t		nosteporg, nostepvel;
    int			clip;
    int			oldonground;
    trace_t		steptrace, downtrace;

    //
    // do a regular slide move unless it looks like you ran into a step
    //

    // Creo que invierte el flag que hubiera para onground
    oldonground = (int)ent->v.flags & FL_ONGROUND;
    ent->v.flags = (int)ent->v.flags & ~FL_ONGROUND;

    // Almacena la posición original y la velocidad de la entidad antes de hacerle ningún cambio
    VectorCopy (ent->v.origin, oldorg);
    VectorCopy (ent->v.velocity, oldvel);

    // Intentamos hacer el movimiento fly completo y almacenamos la traza
    clip = SV_FlyMove (ent, deltaTime, &steptrace); //1 = floor, 2 = wall / step, 4 = dead stop

    // Si el movimiento no se ha bloqueado, nos vamos
    if ( !(clip & 2) ) // Esto se cumple cuando no es 2
        return;		// move didn't block on a step

    // Hemos chcado con 1 floor o 4 dead stop
    if (!oldonground && ent->v.waterlevel == 0)
        return;		// don't stair up while jumping


    VectorCopy (ent->v.origin, nosteporg);
    VectorCopy (ent->v.velocity, nostepvel);

//
// try moving up and forward to go up a step
//
    VectorCopy (oldorg, ent->v.origin);	// back to start pos

    VectorCopy (vec3_origin, upmove);
    VectorCopy (vec3_origin, downmove);
    upmove[2] = STEPSIZE;
    downmove[2] = -STEPSIZE + oldvel[2]*deltaTime;

// move up
    SV_PushEntity (ent, upmove);	// FIXME: don't link?

// move forward
    ent->v.velocity[0] = oldvel[0];
    ent->v. velocity[1] = oldvel[1];
    ent->v. velocity[2] = 0;
    clip = SV_FlyMove (ent, deltaTime, &steptrace);

// check for stuckness, possibly due to the limited precision of floats
// in the clipping hulls
    if (clip)
    {
        if ( fabs(oldorg[1] - ent->v.origin[1]) < 0.03125
             && fabs(oldorg[0] - ent->v.origin[0]) < 0.03125 )
        {	// stepping up didn't make any progress
            clip = SV_TryUnstick (ent, oldvel);
        }
    }

// extra friction based on view angle
    if ( clip & 2 )
        SV_WallFriction (ent, &steptrace);

// move down
    downtrace = SV_PushEntity (ent, downmove);	// FIXME: don't link?

    if (downtrace.plane.normal[2] > 0.7)
    {
        if (ent->v.solid == SOLID_BSP)
        {
            ent->v.flags =	(int)ent->v.flags | FL_ONGROUND;
        }
    }
    else
    {
        // if the push down didn't end up on good ground, use the move without
        // the step up.  This happens near wall / slope combinations, and can
        // cause the player to hop up higher on a slope too steep to climb
        VectorCopy (nosteporg, ent->v.origin);
        VectorCopy (nostepvel, ent->v.velocity);
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