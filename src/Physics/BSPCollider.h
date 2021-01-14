/*
SV_Physics_Client ->
   SV_CheckVelocity
   SV_AddGravity
   SV_CheckStuck
   SV_WalkMove
      - (Move slide)   SV_FlyMove
                       - SV_Move
      - (Move up)      SV_PushEntity
      - (Move Forward) SV_FlyMove
 */

#ifndef BRAKEDA3D_BSPCOLLIDER_H
#define BRAKEDA3D_BSPCOLLIDER_H


#include <cmath>
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/BSPMap.h"


typedef float vec3_t[3];

typedef int	string_t;
typedef int	func_t;
typedef float vec_t;

typedef unsigned char byte;

typedef struct
{
    vec3_t	origin;
    vec3_t	angles;
    int		modelindex;
    int		frame;
    int		colormap;
    int		skin;
    int		effects;
} entity_state_t;

typedef struct
{
    int			planenum;
    short		children[2];	// negative numbers are contents
} dclipnode_t;


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

#define	MAX_ENT_LEAFS	16

// edict->flags
#define	FL_FLY					1
#define	FL_SWIM					2
//#define	FL_GLIMPSE				4
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

#define	NEXT_EDICT(e) ((edict_t *)( (byte *)e + pr_edict_size))

#define	EDICT_TO_PROG(e) ((byte *)e - (byte *)sv.edicts)
#define PROG_TO_EDICT(e) ((edict_t *)((byte *)sv.edicts + e))

#define	CONTENTS_EMPTY		-1
#define	CONTENTS_SOLID		-2
#define	CONTENTS_WATER		-3
#define	CONTENTS_SLIME		-4
#define	CONTENTS_LAVA		-5
#define	CONTENTS_SKY		-6
#define	CONTENTS_ORIGIN		-7		// removed at csg time
#define	CONTENTS_CLIP		-8		// changed to contents_solid

#define	DIST_EPSILON	(0.03125)

typedef struct
{
    float	modelindex;
    vec3_t	absmin;
    vec3_t	absmax;
    float	ltime;
    float	movetype;
    float	solid;
    vec3_t	origin;
    vec3_t	oldorigin;
    vec3_t	velocity;
    vec3_t	angles;
    vec3_t	avelocity;
    vec3_t	punchangle;
    string_t	classname;
    string_t	model;
    float	frame;
    float	skin;
    float	effects;
    vec3_t	mins;
    vec3_t	maxs;
    vec3_t	size;
    func_t	touch;
    func_t	use;
    func_t	think;
    func_t	blocked;
    float	nextthink;
    int	groundentity;
    float	health;
    float	frags;
    float	weapon;
    string_t	weaponmodel;
    float	weaponframe;
    float	currentammo;
    float	ammo_shells;
    float	ammo_nails;
    float	ammo_rockets;
    float	ammo_cells;
    float	items;
    float	takedamage;
    int	chain;
    float	deadflag;
    vec3_t	view_ofs;
    float	button0;
    float	button1;
    float	button2;
    float	impulse;
    float	fixangle;
    vec3_t	v_angle;
    float	idealpitch;
    string_t	netname;
    int	enemy;
    float	flags;
    float	colormap;
    float	team;
    float	max_health;
    float	teleport_time;
    float	armortype;
    float	armorvalue;
    float	waterlevel;
    float	watertype;
    float	ideal_yaw;
    float	yaw_speed;
    int	aiment;
    int	goalentity;
    float	spawnflags;
    string_t	target;
    string_t	targetname;
    float	dmg_take;
    float	dmg_save;
    int	dmg_inflictor;
    int	owner;
    vec3_t	movedir;
    string_t	message;
    float	sounds;
    string_t	noise;
    string_t	noise1;
    string_t	noise2;
    string_t	noise3;
} entvars_t;

typedef struct link_s
{
    struct link_s	*prev, *next;
} link_t;

typedef struct edict_s
{
    bool 	   free;
    link_t		area;				// linked to a division node or leaf

    int			num_leafs;
    short		leafnums[MAX_ENT_LEAFS];

    entity_state_t	baseline;

    float		freetime;			// sv.time when the object was freed
    entvars_t	v;					// C exported fields from progs
// other fields from progs come immediately after
} edict_t;




typedef struct areanode_s
{
    int		axis;		// -1 = leaf node
    float	dist;
    struct areanode_s	*children[2];
    link_t	trigger_edicts;
    link_t	solid_edicts;
} areanode_t;



typedef struct mnode_s
{
    // common with leaf
    int			contents;		// 0, to differentiate from leafs
    int			visframe;		// node needs to be traversed if current

    short		minmaxs[6];		// for bounding box culling

    struct mnode_s	*parent;

// node specific
    mplane_t	*plane;
    struct mnode_s	*children[2];

    unsigned short		firstsurface;
    unsigned short		numsurfaces;
} mnode_t;

typedef enum {mod_brush, mod_sprite, mod_alias} modtype_t;
typedef enum {ST_SYNC=0, ST_RAND } synctype_t;

typedef struct efrag_s
{
    struct mleaf_s		*leaf;
    struct efrag_s		*leafnext;
    struct entity_s		*entity;
    struct efrag_s		*entnext;
} efrag_t;

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
typedef struct surfcache_s
{
    struct surfcache_s	*next;
    struct surfcache_s 	**owner;		// NULL is an empty chunk of memory
    int					lightadj[MAXLIGHTMAPS]; // checked for strobe flush
    int					dlight;
    int					size;		// including header
    unsigned			width;
    unsigned			height;		// DEBUG only needed for debug
    float				mipscale;
    struct texture_s	*texture;	// checked for animating textures
    byte				data[4];	// width*height elements
} surfcache_t;

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
    float		mins[3], maxs[3];
    float		origin[3];
    int			headnode[MAX_MAP_HULLS];
    int			visleafs;		// not including the solid leaf 0
    int			firstface, numfaces;
} dmodel_t;

typedef struct
{
    vec3_t		position;
} mvertex_t;

typedef struct
{
    unsigned short	v[2];
    unsigned int	cachededgeoffset;
} medge_t;

typedef struct cache_user_s
{
    void	*data;
} cache_user_t;

#define	MAX_QPATH		64			// max length of a quake game pathname
typedef struct model_s
{
    modtype_t	type;
    int			numframes;
    synctype_t	synctype;

    vec3_t      origin;      // brakeza
    int         modelindex;  // brakeza

    int			flags;
    // volume occupied by the model
    vec3_t		mins, maxs;
    float		radius;

    int			numplanes;
    mplane_t	*planes;

    int			numleafs;		// number of visible leafs, not counting 0
    bspleaf_t	*leafs;

    int			numclipnodes;
    dclipnode_t	*clipnodes;

    hull_t		hulls[MAX_MAP_HULLS];
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
    edict_t		*passedict;
} moveclip_t;

#define	AREA_NODES	32


class BSPCollider {
public:

    areanode_t	sv_areanodes[AREA_NODES];
    vec3_t vec3_origin;

    BSPCollider();

    void CrossProduct (vec3_t v1, vec3_t v2, vec3_t cross);
    void VectorScale (vec3_t in, vec_t scale, vec3_t out);
    void SV_MoveBounds (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, vec3_t boxmins, vec3_t boxmaxs);
    hull_t *SV_HullForEntity (model_collision_t *ent, vec3_t mins, vec3_t maxs, vec3_t offset);
    int SV_HullPointContents (hull_t *hull, int num, vec3_t p);
    bool SV_RecursiveHullCheck (hull_t *hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace);
    trace_t SV_ClipMoveToEntity (model_collision_t *ent, vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end);
    trace_t SV_Move (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int type);
    int ClipVelocity (vec3_t in, vec3_t normal, vec3_t out, float overbounce);
    int SV_FlyMove (edict_t *ent, float time, trace_t *steptrace);
    void InsertLinkBefore (link_t *l, link_t *before);
    void RemoveLink (link_t *l);
    void SV_UnlinkEdict (edict_t *ent);
    void SV_FindTouchedLeafs (edict_t *ent, mnode_t *node);
    void SV_LinkEdict (edict_t *ent, bool touch_triggers);
    trace_t SV_PushEntity (edict_t *ent, vec3_t push);
    void AngleVectors (vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
    void SV_WallFriction (edict_t *ent, trace_t *trace);
    int SV_TryUnstick (edict_t *ent, vec3_t oldvel);
    void SV_WalkMove (edict_t *ent, float deltaTime);
    static int BoxOnPlaneSide (vec3_t emins, vec3_t emaxs, mplane_t *p);
};

#endif //BRAKEDA3D_BSPCOLLIDER_H
