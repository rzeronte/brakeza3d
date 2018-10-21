
#ifndef Q3LOADER_H
#define Q3LOADER_H

#pragma pack(push, Q3LOADER_H)
#include <vector>
#pragma pack(pop, Q3LOADER_H)

#pragma pack(push, Q3LOADER_H)
#include <string>
#pragma pack(pop, Q3LOADER_H)

/**
 * Description of a lump.
 * 
 */
struct TLump
{
	int					mOffset;			// Offset to start of lump, relative to beginning of file.  
	int					mLength;			// Length of lump. Always a multiple of 4.  

};

/**
 * Header of the Q3 map.
 */
struct THeader
{
	char				mMagicNumber[4];	//  Magic number. Always "IBSP".  
	int					mVersion;  			//	Version number 0x2e for the BSP files distributed with Quake 3.  
	TLump				mLumpes[17];		//	Lump directory, seventeen entries.
};

/**
 * Entity of the Q3 map.
 * The entities lump stores game-related map information, including information about the map name, weapons, health, armor, triggers, spawn points, lights, and .md3 models to be placed in the map. 
 */
struct TEntity
{
	int					mSize;				// Size of the description.
	char*				mBuffer;			// Entity descriptions, stored as a string. 
};

/**
 * Texture of the Q3 map.
 * The textures lump stores information about surfaces and volumes, which are in turn associated with faces, brushes, and brushsides. 
 */
struct TTexture
{
	char				mName[64];			// Texture name.  
	int					mFlags;				// Surface flags.  
	int					mContents;			// Content flags.  
};

/**
 * Plane of the Q3 map.
 * The planes lump stores a generic set of planes that are in turn referenced by nodes and brushsides. 
 */
struct TPlane
{
	float				mNormal[3];			// Plane normal.  
	float				mDistance;			// Distance from origin to plane along normal.  
};

/**
 * Node of the Q3 map.
 * The nodes lump stores all of the nodes in the map's BSP tree.
 */
struct TNode
{
	int					mPlane;				// Plane index.  
	int					mChildren[2];		// Children indices. Negative numbers are leaf indices: -(leaf+1).  
	int					mMins[3];			// Integer bounding box min coord.  
	int					mMaxs[3];			// Integer bounding box max coord.  
};

/**
 * Leaf of the Q3 map.
 * The leafs lump stores the leaves of the map's BSP tree. 
 */
struct TLeaf
{
	int					mCluster;			// Visdata cluster index.  
	int					mArea;				// Areaportal area.  
	int					mMins[3];			// Integer bounding box min coord.  
	int					mMaxs[3];			// Integer bounding box max coord.  
	int					mLeafFace;			// First leafface for leaf.  
	int					mNbLeafFaces;		// Number of leaffaces for leaf.  
	int					mLeafBrush;			// First leafbrush for leaf.  
	int					mNbLeafBrushes;		// Number of leafbrushes for leaf.  

};

/**
 * LeafFace of the Q3 map.
 * The leaffaces lump stores lists of face indices, with one list per leaf. 
 */
struct TLeafFace
{
	int					mFaceIndex;			// Face index.  
};

/**
 * Leaf Brush of the Q3 map.
 * The leafbrushes lump stores lists of brush indices, with one list per leaf. 
 */
struct TLeafBrush
{
	int					mBrushIndex;		// Brush index.  
};

/**
 * Model of the Q3 map.
 * The models lump describes rigid groups of world geometry. 
 */
struct TModel
{
	float				mMins[3];			// Bounding box min coord.  
	float				mMaxs[3];			// Bounding box max coord.  
	int					mFace;				// First face for model.  
	int					mNbFaces;			// Number of faces for model.  
	int					mBrush;				// First brush for model.  
	int					mNBrushes;			// Number of brushes for model.  

};

/**
 * Brush of the Q3 map.
 * The brushes lump stores a set of brushes, which are in turn used for collision detection.
 */
struct TBrush
{
	int					mBrushSide;			// First brushside for brush.  
	int					mNbBrushSides;		// Number of brushsides for brush.  
	int					mTextureIndex;		// Texture index.  
};

/**
 * BrushSide of the Q3 map.
 * The brushsides lump stores descriptions of brush bounding surfaces.
 */
struct TBrushSide
{
	int					mPlaneIndex;		// Plane index.  
	int					mTextureIndex;		// Texture index.  
};

/**
 * Vertex of the Q3 map.
 * The vertexes lump stores lists of vertices used to describe faces.
 */
struct TVertex
{
	float				mPosition[3];		// Vertex position.  
	float				mTexCoord[2][2];	// Vertex texture coordinates. 0 = Surface, 1 = Lightmap.  
	float				mNormal[3];			// Vertex normal.  
	unsigned char		mColor[4];			// Vertex color (RGBA).  
};

/**
 * MeshVert of the Q3 map.
 * The meshverts lump stores lists of vertex offsets, used to describe generalized triangle meshes.
 */
struct TMeshVert
{
	int					mMeshVert;			// Vertex index offset, relative to first vertex of corresponding face.
};

/**
 * Effect of the Q3 map.
 * The effects lump stores references to volumetric shaders (typically fog) which affect the rendering of a particular group of faces.
 */
struct TEffect
{
	char				mName[64];			// Effect shader.  
	int					mBrush;				// Brush that generated this effect.  
	int					mUnknown;			// Always 5, except in q3dm8, which has one effect with -1.  
};

/**
 * Face of the Q3 map.
 * The faces lump stores information used to render the surfaces of the map.
 */
struct TFace
{
	int					mTextureIndex;		// Texture index.  
	int					mEffectIndex;		// Index into lump 12 (Effects), or -1.  
	int					mType;				// Face type. 1 = Polygon, 2 = Patch, 3 = Mesh, 4 = Billboard. 
	int					mVertex;			// Index of first vertex.  
	int					mNbVertices;		// Number of vertices.  
	int					mMeshVertex;		// Index of first meshvert.  
	int					mNbMeshVertices;	// Number of meshverts.  
	int					mLightmapIndex;		// Lightmap index.  
	int					mLightmapCorner[2];	// Corner of this face's lightmap image in lightmap.  
	int					mLightmapSize[2];	// Size of this face's lightmap image in lightmap.  
	float				mLightmapOrigin[3];	// World space origin of lightmap.  
	float				mLightmapVecs[2][3];// World space lightmap s and t unit vectors.  
	float				mNormal[3];			// Surface normal.  
	int					mPatchSize[2];		// Patch dimensions.  

};

/**
 * Lightmap of the Q3 map.
 * The lightmaps lump stores the light map textures used make surface lighting look more realistic.
 */
struct TLightMap
{
	unsigned char		mMapData[128][128][3];// Lightmap color data. RGB. 
};

/**
 * Light volume of the Q3 map.
 * The lightvols lump stores a uniform grid of lighting information used to illuminate non-map objects.
 */
struct TLightVol
{
	unsigned char		mAmbient[3];		// Ambient color component. RGB.  
	unsigned char		mDirectional[3];	// Directional color component. RGB.  
	unsigned char		mDir[2];			// Direction to light. 0=phi, 1=theta.  
};

/**
 * The Visibility data of the Q3 map.
 * The visdata lump stores bit vectors that provide cluster-to-cluster visibility information.
 */
struct TVisData
{
	int					mNbClusters;		// The number of clusters
    int					mBytesPerCluster;	// Bytes (8 bits) in the cluster's bitset
    unsigned char*		mBuffer;			// Array of bytes holding the cluster vis.
};

/**
 * The Q3 map definition.
 * The Q3 map is defined by 17 lumps and a header section.
 */
struct TMapQ3
{
	
	THeader					mHeader;		// Header of the file.
	TEntity					mEntity;		// Array of the leaves.
	std::vector<TTexture>	mTextures;		// Array of the textures.
	std::vector<TPlane>		mPlanes;		// Array of the planes.
	std::vector<TNode>		mNodes;			// Array of the nodes.
	std::vector<TLeaf>		mLeaves;		// Array of the leaves.
	std::vector<TLeafFace>	mLeafFaces;		// Array of the leaf faces.
	std::vector<TLeafBrush>	mLeafBrushes;	// Array of the leaf brushes.
	std::vector<TModel>		mModels;		// Array of the models.
	std::vector<TBrush>		mBrushes;		// Array of the brushes.
	std::vector<TBrushSide>	mBrushSides;	// Array of the brush sides.
	std::vector<TVertex>	mVertices;		// Array of the vertices.
	std::vector<TMeshVert>	mMeshVertices;	// Array of the mesh vertices.
	std::vector<TEffect>	mEffects;		// Array of the effects.
	std::vector<TFace>		mFaces;			// Array of the faces.
	std::vector<TLightMap>	mLightMaps;		// Array of the light maps.
	std::vector<TLightVol>	mLightVols;		// Array of the light volumes.
	TVisData				mVisData;		// The visibility datas.
};

/**
 * Constant for the ID Software Magic number.
 */
const std::string			cMagicNumber	= "IBSP";
							
/**							
 * Constant for the Q3 Map version.
 */							
const int					cVersion		= 0x2E;
							
/**							
 * Constant identifier for all the lumps.
 */							
const int					cEntityLump		= 0x00; // Entities : Game-related object descriptions.  
const int					cTextureLump	= 0x01; // Textures : Surface descriptions.  
const int					cPlaneLump		= 0x02; // Planes : Planes used by map geometry.  
const int					cNodeLump		= 0x03; // Nodes : BSP tree nodes.  
const int					cLeafLump		= 0x04; // Leafs : BSP tree leaves.  
const int					cLeafFaceLump	= 0x05; // LeafFaces : Lists of face indices, one list per leaf.  
const int					cLeafBrushLump	= 0x06; // LeafBrushes  Lists of brush indices, one list per leaf.  
const int					cModelLump		= 0x07; // Models  Descriptions of rigid world geometry in map.  
const int					cBrushLump		= 0x08; // Brushes  Convex polyhedra used to describe solid space.  
const int					cBrushSideLump	= 0x09; // Brushsides  Brush surfaces.  
const int					cVertexLump		= 0x0A; // Vertexes  Vertices used to describe faces.  
const int					cMeshVertLump	= 0x0B; // MeshVerts  Lists of offsets, one list per mesh.  
const int					cEffectLump		= 0x0C; // Effects  List of special map effects.  
const int					cFaceLump		= 0x0D; // Faces  Surface geometry.  
const int					cLightMapLump	= 0x0E; // LightMaps  Packed lightmap data.  
const int					cLightVolLump	= 0x0F; // LightVols  Local illumination data.  
const int					cVisDataLump	= 0x10; // Visdata  Cluster-cluster visibility data.

/**
 * Dump all the Q3 map in a text file.
 * Must be used only for debug purpose.
 *
 * @param pFile	The file to dump the informations.
 * @param pMap	The Q3 map to dump in string.
 */
void debugInformations(const TMapQ3& pMap, FILE* pFile);

/**
 * Read the map.
 *
 * @param pFilename	The filename of the map to read.
 *
 * @return true if the loading successed, false otherwise.
 */
bool readMap(const std::string& pFilename, TMapQ3& pMap);

/**
 * Free all the datas of the map.
 *
 * @param pMap	The Q3 map to free.
 */
void freeMap(TMapQ3& pMap);

/**
 * Check if the header of the map is valid.
 *
 * @param pMap	The map to test.
 *
 * @return True if the map is valid, false otherwise.
 */
bool isValid(const TMapQ3& pMap);

#endif