#ifndef BRAKEDA3D_BSPCOLLIDER_H
#define BRAKEDA3D_BSPCOLLIDER_H

#include <cmath>
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/BSPMap.h"

typedef float vec3_t[3];

typedef int	string_t;
typedef float vec_t;
typedef unsigned char byte;

#define MAX_VELOCITY 320;

#define	MAX_ENT_LEAFS	16

#define	DIST_EPSILON	(0.03125)

// edict->flags
#define	FL_FLY					1
#define	FL_SWIM					2
#define	FL_CONVEYOR				4
#define	FL_CLIENT				8
#define	FL_INWATER				16
#define	FL_MONSTER				32
#define	FL_GODMODE				64
#define	FL_NOTARGET				128
#define	FL_ITEM					256
#define	FL_ONGROUND				512
#define	FL_PARTIALGROUND		1024	// not all corners are valid
#define	FL_WATERJUMP			2048	// player jumping out of water
#define	FL_JUMPRELEASED			4096	// for jump debouncing

// edict->solid values
#define	SOLID_NOT				0		// no interaction with other objects
#define	SOLID_TRIGGER			1		// touch on edge, but not blocking
#define	SOLID_BBOX				2		// touch on edge, block
#define	SOLID_SLIDEBOX			3		// touch on edge, but not an onground
#define	SOLID_BSP				4		// bsp clip, touch on edge, block

#define	STEPSIZE	18

// edict->movetype values
#define	MOVETYPE_NONE			0		// never moves
#define	MOVETYPE_ANGLENOCLIP	1
#define	MOVETYPE_ANGLECLIP		2
#define	MOVETYPE_WALK			3		// gravity
#define	MOVETYPE_STEP			4		// gravity, special edge handling
#define	MOVETYPE_FLY			5
#define	MOVETYPE_TOSS			6		// gravity
#define	MOVETYPE_PUSH			7		// no clip to world, push and crush
#define	MOVETYPE_NOCLIP			8
#define	MOVETYPE_FLYMISSILE		9		// extra size to monsters
#define	MOVETYPE_BOUNCE			10

#define	MOVE_NORMAL		0
#define	MOVE_NOMONSTERS	1
#define	MOVE_MISSILE	2

// up / down
#define	PITCH	0
// left / right
#define	YAW		1
// fall over
#define	ROLL	2

#define	MAX_MAP_HULLS		4

#define DotProduct(x,y) (x[0]*y[0]+x[1]*y[1]+x[2]*y[2])
#define VectorSubtract(a,b,c) {c[0]=a[0]-b[0];c[1]=a[1]-b[1];c[2]=a[2]-b[2];}
#define VectorAdd(a,b,c) {c[0]=a[0]+b[0];c[1]=a[1]+b[1];c[2]=a[2]+b[2];}
#define VectorCopy(a,b) {b[0]=a[0];b[1]=a[1];b[2]=a[2];}

#define	CONTENTS_EMPTY		-1
#define	CONTENTS_SOLID		-2
#define	CONTENTS_WATER		-3
#define	CONTENTS_SLIME		-4
#define	CONTENTS_LAVA		-5
#define	CONTENTS_SKY		-6
#define	CONTENTS_ORIGIN		-7		// removed at csg time
#define	CONTENTS_CLIP		-8		// changed to contents_solid

typedef struct mplane_s
{
    vec3_t	normal;
    float	dist;
    byte	type;			// for texture axis selection and fast side tests
    byte	signbits;		// signx + signy<<1 + signz<<1
    byte	pad[2];
} mplane_t;

typedef struct
{
    dclipnode_t 	*clipnodes;
    mplane_t	*planes;
    int			firstclipnode;
    int			lastclipnode;
    vec3_t		clip_mins;
    vec3_t		clip_maxs;
} hull_t;

typedef struct link_s
{
    struct link_s	*prev, *next;
} link_t;

typedef struct mnode_s
{
    // common with leaf
    int			contents = 0 ;		// 0, to differentiate from leafs
    int			visframe;		// node needs to be traversed if current

    short mins[3];		// for bounding box culling
    short maxs[3];

    struct mnode_s	*parent;

    // node specific
    mplane_t	*plane;
    struct mnode_s	*children[2];

    unsigned short		firstsurface;
    unsigned short		numsurfaces;
} mnode_t;


typedef enum {mod_brush, mod_sprite, mod_alias} modtype_t;
typedef enum {ST_SYNC=0, ST_RAND } synctype_t;

#define	MIPLEVELS	4
typedef struct texture_s
{
    char		name[16];
    unsigned	width, height;
    int			anim_total;				// total tenths in sequence ( 0 = no)
    int			anim_min, anim_max;		// time for this frame min <=time< max
    struct texture_s *anim_next;		// in the animation sequence
    struct texture_s *alternate_anims;	// bmodels in frmae 1 use these
    unsigned	offsets[MIPLEVELS];		// four mip maps stored
} texture_t;

typedef struct
{
    float		vecs[2][4];
    float		mipadjust;
    texture_t	*texture;
    int			flags;
} mtexinfo_t;

#define MAXLIGHTMAPS 4
typedef struct msurface_s
{
    int			visframe;		// should be drawn when node is crossed

    int			dlightframe;
    int			dlightbits;

    mplane_t	*plane;
    int			flags;

    int			firstedge;	// look up in model->surfedges[], negative numbers
    int			numedges;	// are backwards edges

    // surface generation data
    struct surfcache_s	*cachespots[MIPLEVELS];

    short		texturemins[2];
    short		extents[2];

    mtexinfo_t	*texinfo;

    // lighting info
    byte		styles[MAXLIGHTMAPS];
    byte		*samples;		// [numstyles*surfsize]
} msurface_t;

#define	NUM_AMBIENTS			4		// automatic ambient sounds
typedef short svec3_t[3];	// X,Y,Z coordinates of the vertex in shorts

typedef struct
{
    unsigned short	v[2];
    unsigned int	cachededgeoffset;
} medge_t;

typedef struct efrag_s
{
    struct mleaf_s		*leaf;
    struct efrag_s		*leafnext;
    struct entity_s		*entity;
    struct efrag_s		*entnext;
} efrag_t;

typedef struct mleaf_s
{
    // common with node
    int			contents = 0;		// wil be a negative contents number
    int			visframe;		// node needs to be traversed if current

    vec3_t mins;		// for bounding box culling
    vec3_t maxs;

    struct mnode_s	*parent;

    // leaf specific
    byte		*compressed_vis;
    efrag_t		*efrags;

    msurface_t	**firstmarksurface;
    int			nummarksurfaces;
    int			key;			// BSP sequence number for leaf's contents
    byte		ambient_sound_level[NUM_AMBIENTS];
} mleaf_t;

typedef struct model_s
{
    vec3_t      origin;      // brakeza
    vec3_t	    oldorigin;   // brakeza
    int         modelindex;  // brakeza

    modtype_t	type;
    int			numframes;
    synctype_t	synctype;

    int			numclipnodes;
    dclipnode_t	*clipnodes;

    int 		flags;
    // volume occupied by the model
    vec3_t		mins, maxs;
    float		radius;

    int			numplanes;
    mplane_t	*planes;
    hull_t		hulls[MAX_MAP_HULLS];

    int			numnodes;

    vec3_t	absmin;
    vec3_t	absmax;

    mnode_t  *nodes;

    // Extracted from edict
    link_t	area;				// linked to a division node or leaf
    bool 	free;

    int		num_leafs;              // Del edicto
    short	leafnums[MAX_ENT_LEAFS];

    int			numleafs;		// number of visible leafs, not counting 0
    mleaf_t		*leafs;

    float	solid;
    float	movetype;
    vec3_t	velocity;

    float	waterlevel = 0;
    vec3_t	v_angle;
    vec3_t angles;
    vec3_t	punchangle;
    float	fixangle;

    int			nummarksurfaces;
    msurface_t	**marksurfaces;
    byte		*visdata;

    vec3_t	wishdir;
    float	wishspeed;
} model_collision_t;

typedef struct
{
    bool	allsolid;	// if true, plane is not valid
    bool	startsolid;	// if true, the initial point was in a solid area
    bool	inopen, inwater;
    float	fraction;		// time completed, 1.0 = didn't hit anything
    vec3_t	endpos;			// final position
    plane_t	plane;			// surface normal at impact
    model_collision_t	*ent;			// entity the surface is on
} trace_t;

typedef struct
{
    vec3_t		boxmins, boxmaxs;// enclose the test object along entire move
    float		*mins, *maxs;	// size of the moving object
    vec3_t		mins2, maxs2;	// size when clipping against mosnters
    float		*start, *end;
    trace_t		trace;
    int			type;
    model_collision_t *passedict;
} moveclip_t;

typedef struct
{
    int		fileofs, filelen;
} lump_t;

class BSPCollider {
public:

    model_collision_t *worldmodel;
    model_collision_t *playermodel;
    vec3_t vec3_origin;
    float scale;

    BSPCollider();
    void LoadModelCollisionForWorld();

    void CrossProduct (vec3_t v1, vec3_t v2, vec3_t cross);
    void VectorScale (vec3_t in, vec_t scale, vec3_t out);
    void AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);

    void SV_MoveBounds (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, vec3_t boxmins, vec3_t boxmaxs);

    hull_t *SV_HullForEntity (model_collision_t *ent, vec3_t mins, vec3_t maxs, vec3_t offset);
    int SV_HullPointContents (hull_t *hull, int num, vec3_t p);
    bool SV_RecursiveHullCheck (hull_t *hull, int hullFirstClipNode, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace);
    int SV_FlyMove (model_collision_t *ent, float time, trace_t *steptrace);

    trace_t SV_ClipMoveToEntity (model_collision_t *ent, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end);
    trace_t SV_Move (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int type);
    void SV_FindTouchedLeafs (model_collision_t *ent, mnode_t *node);
    trace_t SV_PushEntity (model_collision_t *ent, vec3_t push);

    void SV_WallFriction (model_collision_t *ent, trace_t *trace);
    int SV_TryUnstick (model_collision_t *ent, vec3_t oldvel);
    void SV_WalkMove (model_collision_t *ent, float deltaTime);

    int ClipVelocity (vec3_t in, vec3_t normal, vec3_t out, float overbounce);

    model_collision_t *getModelCollisionFromBSP(int modelId);

    static int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, mplane_t *p);

    model_collision_t *getWorldModel();
    model_collision_t *getPlayerModel();

    void SV_AddGravity (model_collision_t *ent, float deltaTime);
    void SV_CheckStuck (model_collision_t *ent);
    model_collision_t *SV_TestEntityPosition (model_collision_t *ent);

    void drawHullAABB(model_collision_t *model, int indexHull);

    void Mod_LoadLeafs_BSP29(model_collision_t *brushmodel, dheader_t *header);
    void Mod_LoadNodes_BSP29(model_collision_t *brushmodel, dheader_t *header);
    void Mod_LoadClipnodes_BSP29(model_collision_t *brushmodel, dheader_t *header);
    void Mod_MakeClipHulls(model_collision_t *brushmodel);
    void Mod_MakeDrawHull(model_collision_t *brushmodel);
    void SV_ClientThink (model_collision_t *model, float deltaTime);

    void DropPunchAngle (model_collision_t *model, float deltaTime);
    float V_CalcRoll (vec3_t angles, vec3_t velocity);
    float V_CalcBob (float time, vec3_t velocity);

    void ApplyBob(model_collision_t *model, float deltaTime);

    void SV_CheckVelocity (model_collision_t *ent);
    void SV_AirMove (model_collision_t *model, float deltaTime);
    void SV_UserFriction (model_collision_t *model, float deltaTime);
    void SV_AirAccelerate (model_collision_t *model, vec3_t wishveloc, float deltaTime);
    void SV_Accelerate (model_collision_t *model, float deltaTime);

    float VectorNormalize (vec3_t v);
    void consoleTrace(trace_t *t);
    static void Vertex3DToVec3(Vertex3D v, vec3_t &dest);
    static Vertex3D QuakeToVertex3D(vec3_t &v);

    void checkTrace(Vertex3D start, Vertex3D finish, vec3_t mins, vec3_t maxs);
    vec_t VectorLength(vec3_t v);

};

#endif //BRAKEDA3D_BSPCOLLIDER_H
