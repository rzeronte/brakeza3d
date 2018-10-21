/**
 * Simple Q3 Map loader.
 * 
 * For comments or bug reports, contact : nicolas.baudrey@wanadoo.fr
 */

#include "../../headers/Q3MapLoader/Q3Loader.h"

#pragma pack(push, Q3LOADER_H)
#include <cstdio>
#pragma pack(pop, Q3LOADER_H)

/**
 * Check if the header of the map is valid.
 *
 * @param pMap	The map to test.
 *
 * @return True if the map is valid, false otherwise.
 */
bool isValid(const TMapQ3& pMap)
{
	// Check if the header is equal to ID Software Magic Number.
	if
		(strncmp(pMap.mHeader.mMagicNumber, cMagicNumber.c_str(), 4) != 0)
	{
		return false;
	}

	// Check if the version number is equal to the Q3 map.
	if
		(pMap.mHeader.mVersion != cVersion)
	{
		return false;
	}

	return true;
}

/**
 * Read the header of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
bool readHeader(FILE* pFile, TMapQ3& pMap)
{
	fread(&pMap.mHeader, 1, sizeof(THeader), pFile);

	return isValid(pMap);
}

/**
 * Read the texture lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readTexture(FILE* pFile, TMapQ3& pMap)
{
	int	lNbTextures = pMap.mHeader.mLumpes[cTextureLump].mLength / sizeof(TTexture);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cTextureLump].mOffset, SEEK_SET);

	for
		(int lTextureCounter = 0; lTextureCounter < lNbTextures; ++lTextureCounter)
	{
		TTexture lTexture;
		
		fread(&lTexture, 1, sizeof(TTexture), pFile);

		pMap.mTextures.push_back(lTexture);
	}
}

/**
 * Read the entity lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readEntity(FILE* pFile, TMapQ3& pMap)
{
	// Set the entity size.
	pMap.mEntity.mSize = pMap.mHeader.mLumpes[cEntityLump].mLength;
	
	// Allocate the entity buffer.
	pMap.mEntity.mBuffer =  new char[pMap.mEntity.mSize];

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cEntityLump].mOffset, SEEK_SET);
	
	// Read the buffer.
	fread(pMap.mEntity.mBuffer, pMap.mEntity.mSize, sizeof(char), pFile);
};

/**
 * Read the plane lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readPlane(FILE* pFile, TMapQ3& pMap)
{
	int	lNbPlanes = pMap.mHeader.mLumpes[cPlaneLump].mLength / sizeof(TPlane);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cPlaneLump].mOffset, SEEK_SET);

	for
		(int lPlaneCounter = 0; lPlaneCounter < lNbPlanes; ++lPlaneCounter)
	{
		TPlane lPlane;
		
		fread(&lPlane, 1, sizeof(TPlane), pFile);

		pMap.mPlanes.push_back(lPlane);
	}
}

/**
 * Read the node lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readNode(FILE* pFile, TMapQ3& pMap)
{
	int	lNbNodes = pMap.mHeader.mLumpes[cNodeLump].mLength / sizeof(TNode);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cNodeLump].mOffset, SEEK_SET);

	for
		(int lNodeCounter = 0; lNodeCounter < lNbNodes; ++lNodeCounter)
	{
		TNode lNode;
		
		fread(&lNode, 1, sizeof(TNode), pFile);

		pMap.mNodes.push_back(lNode);
	}	
}

/**
 * Read the leaf lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readLeaf(FILE* pFile, TMapQ3& pMap)
{
	int	lNbLeaves = pMap.mHeader.mLumpes[cLeafLump].mLength / sizeof(TLeaf);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cLeafLump].mOffset, SEEK_SET);

	for
		(int lLeafCounter = 0; lLeafCounter < lNbLeaves; ++lLeafCounter)
	{
		TLeaf lLeaf;
		
		fread(&lLeaf, 1, sizeof(TLeaf), pFile);

		pMap.mLeaves.push_back(lLeaf);
	}	
}

/**
 * Read the leafface lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readLeafFace(FILE* pFile, TMapQ3& pMap)
{
	int	lNbLeafFaces = pMap.mHeader.mLumpes[cLeafFaceLump].mLength / sizeof(TLeafFace);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cLeafFaceLump].mOffset, SEEK_SET);

	for
		(int lLeafFaceCounter = 0; lLeafFaceCounter < lNbLeafFaces; ++lLeafFaceCounter)
	{
		TLeafFace lLeafFace;
		
		fread(&lLeafFace, 1, sizeof(TLeafFace), pFile);

		pMap.mLeafFaces.push_back(lLeafFace);
	}			
}

/**
 * Read the leafbrush lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readLeafBrush(FILE* pFile, TMapQ3& pMap)
{
	int	lNbLeafBrushes = pMap.mHeader.mLumpes[cLeafBrushLump].mLength / sizeof(TLeafBrush);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cLeafBrushLump].mOffset, SEEK_SET);

	for
		(int lLeafBrusheCounter = 0; lLeafBrusheCounter < lNbLeafBrushes; ++lLeafBrusheCounter)
	{
		TLeafBrush lLeafBrush;
		
		fread(&lLeafBrush, 1, sizeof(TLeafBrush), pFile);

		pMap.mLeafBrushes.push_back(lLeafBrush);
	}	
}

/**
 * Read the model lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readModel(FILE* pFile, TMapQ3& pMap)
{
	int	lNbModels = pMap.mHeader.mLumpes[cModelLump].mLength / sizeof(TModel);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cModelLump].mOffset, SEEK_SET);

	for
		(int lModelCounter = 0; lModelCounter < lNbModels; ++lModelCounter)
	{
		TModel lModel;
		
		fread(&lModel, 1, sizeof(TModel), pFile);

		pMap.mModels.push_back(lModel);
	}
}

/**
 * Read the brush lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readBrush(FILE* pFile, TMapQ3& pMap)
{
	int	lNbBrushes = pMap.mHeader.mLumpes[cBrushLump].mLength / sizeof(TBrush);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cBrushLump].mOffset, SEEK_SET);

	for
		(int lBrusheCounter = 0; lBrusheCounter < lNbBrushes; ++lBrusheCounter)
	{
		TBrush lBrush;
		
		fread(&lBrush, 1, sizeof(TBrush), pFile);

		pMap.mBrushes.push_back(lBrush);
	}
}

/**
 * Read the brush side lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readBrushSide(FILE* pFile, TMapQ3& pMap)
{
	int	lNbBrushSides = pMap.mHeader.mLumpes[cBrushSideLump].mLength / sizeof(TBrushSide);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cBrushSideLump].mOffset, SEEK_SET);

	for
		(int lBrushSideCounter = 0; lBrushSideCounter < lNbBrushSides; ++lBrushSideCounter)
	{
		TBrushSide lBrushSide;
		
		fread(&lBrushSide, 1, sizeof(TBrushSide), pFile);

		pMap.mBrushSides.push_back(lBrushSide);
	}
}

/**
 * Read the vertex lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readVertex(FILE* pFile, TMapQ3& pMap)
{
	int	lNbVertices = pMap.mHeader.mLumpes[cVertexLump].mLength / sizeof(TVertex);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cVertexLump].mOffset, SEEK_SET);

	for
		(int lVerticeCounter = 0; lVerticeCounter < lNbVertices; ++lVerticeCounter)
	{
		TVertex lVertex;
		
		fread(&lVertex, 1, sizeof(TVertex), pFile);

		pMap.mVertices.push_back(lVertex);
	}
}

/**
 * Read the meshvert lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readMeshVert(FILE* pFile, TMapQ3& pMap)
{
	int	lNbMeshVertices = pMap.mHeader.mLumpes[cMeshVertLump].mLength / sizeof(TMeshVert);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cMeshVertLump].mOffset, SEEK_SET);

	for
		(int lVerticeCounter = 0; lVerticeCounter < lNbMeshVertices; ++lVerticeCounter)
	{
		TMeshVert lMeshVertex;
		
		fread(&lMeshVertex, 1, sizeof(TMeshVert), pFile);

		pMap.mMeshVertices.push_back(lMeshVertex);
	}
}

/**
 * Read the effect lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readEffect(FILE* pFile, TMapQ3& pMap)
{
	int	lNbEffects = pMap.mHeader.mLumpes[cEffectLump].mLength / sizeof(TEffect);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cEffectLump].mOffset, SEEK_SET);

	for
		(int lEffectCounter = 0; lEffectCounter < lNbEffects; ++lEffectCounter)
	{
		TEffect lEffect;
		
		fread(&lEffect, 1, sizeof(TEffect), pFile);

		pMap.mEffects.push_back(lEffect);
	}	
}

/**
 * Read the face lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readFace(FILE* pFile, TMapQ3& pMap)
{
	int	lNbFaces = pMap.mHeader.mLumpes[cFaceLump].mLength / sizeof(TFace);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cFaceLump].mOffset, SEEK_SET);

	for
		(int lFaceCounter = 0; lFaceCounter < lNbFaces; ++lFaceCounter)
	{
		TFace lFace;
		
		fread(&lFace, 1, sizeof(TFace), pFile);

		pMap.mFaces.push_back(lFace);
	}
}

/**
 * Read the effect lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readLightMap(FILE* pFile, TMapQ3& pMap)
{
	int	lNbLightMaps = pMap.mHeader.mLumpes[cLightMapLump].mLength / sizeof(TLightMap);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cLightMapLump].mOffset, SEEK_SET);

	for
		(int lLightMapCounter = 0; lLightMapCounter < lNbLightMaps; ++lLightMapCounter)
	{
		TLightMap lLightMap;
		
		fread(&lLightMap, 1, sizeof(TLightMap), pFile);

		pMap.mLightMaps.push_back(lLightMap);
	}
}

/**
 * Read the effect lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readLightVol(FILE* pFile, TMapQ3& pMap)
{
	int	lNbLightVols = pMap.mHeader.mLumpes[cLightVolLump].mLength / sizeof(TLightVol);

	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cLightVolLump].mOffset, SEEK_SET);

	for
		(int lLightVolCounter = 0; lLightVolCounter < lNbLightVols; ++lLightVolCounter)
	{
		TLightVol lLightVol;
		
		fread(&lLightVol, 1, sizeof(TLightVol), pFile);

		pMap.mLightVols.push_back(lLightVol);
	}
}

/**
 * Read the effect lump of the Q3 map.
 *
 * @param pFile	The stream on the Q3 file data.
 * @param pMap	The map structure to fill.
 */
void readVisData(FILE* pFile, TMapQ3& pMap)
{
	// Go to the start of the chunk.
	fseek(pFile, pMap.mHeader.mLumpes[cVisDataLump].mOffset, SEEK_SET);

	fread(&pMap.mVisData.mNbClusters, 1, sizeof(int), pFile);
	fread(&pMap.mVisData.mBytesPerCluster, 1, sizeof(int), pFile);

	// Allocate the buffer.
	int lBufferSize = pMap.mVisData.mNbClusters * pMap.mVisData.mBytesPerCluster;
	pMap.mVisData.mBuffer = new unsigned char[lBufferSize];

	fread(pMap.mVisData.mBuffer, lBufferSize, sizeof(unsigned char), pFile);
}


/**
 * Dump all the Q3 map in a text file.
 * Must be used only for debug purpose.
 *
 * @param pFile	The file to dump the informations.
 * @param pMap	The Q3 map to dump in string.
 */
void debugInformations(const TMapQ3& pMap, FILE* pFile)
{
	// Check if the given stream is valid.
	if
		(! pFile)
	{
		printf("debugInformations :: Invalid stream handle.\n");
		return ;
	}

	// Check if the map is valid.
	if
		(! isValid(pMap))
	{
		printf("debugInformations :: Invalid Q3 map header.\n");
		return;
	}

	fprintf(pFile, "********* Header *********\n");
	fprintf(pFile, "Magic Number : %s\n", pMap.mHeader.mMagicNumber);
	fprintf(pFile, "Version : %d\n", pMap.mHeader.mVersion);
	for
		(int lLumpCounter = 0; lLumpCounter < 17; ++lLumpCounter)
	{
		fprintf(pFile, "Lump %d\n", lLumpCounter);
		fprintf(pFile, "\tOffset : %d\n", pMap.mHeader.mLumpes[lLumpCounter].mOffset);
		fprintf(pFile, "\tLength : %d\n", pMap.mHeader.mLumpes[lLumpCounter].mLength);
	}
	fprintf(pFile, "\n");
	
	fprintf(pFile, "********* Entity Lump *********\n");
	fprintf(pFile, "Size : %d\n", pMap.mEntity.mSize);
	if
		(pMap.mEntity.mSize != 0)
	{
		fprintf(pFile, "Buffer : %s\n", pMap.mEntity.mBuffer);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* Texture Lump *********\n");
	for
		(int lTextureCounter = 0; lTextureCounter < pMap.mTextures.size(); ++lTextureCounter)
	{
		fprintf(pFile, "Texture %d\n", lTextureCounter);
		fprintf(pFile, "\tName : %s\n", pMap.mTextures[lTextureCounter].mName);
		fprintf(pFile, "\tFlags : %d\n", pMap.mTextures[lTextureCounter].mFlags);
		fprintf(pFile, "\tContents : %d\n", pMap.mTextures[lTextureCounter].mContents);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* Plane Lump *********\n");
	for
		(int lPlaneCounter = 0; lPlaneCounter < pMap.mPlanes.size(); ++lPlaneCounter)
	{
		fprintf(pFile, "Plane %d\n", lPlaneCounter);
		fprintf(pFile, "\tNormal : %f %f %f\n", pMap.mPlanes[lPlaneCounter].mNormal[0], pMap.mPlanes[lPlaneCounter].mNormal[1], pMap.mPlanes[lPlaneCounter].mNormal[2]);
		fprintf(pFile, "\tDistance : %f\n", pMap.mPlanes[lPlaneCounter].mDistance);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* Node Lump *********\n");
	for
		(int lNodeCounter = 0; lNodeCounter < pMap.mNodes.size(); ++lNodeCounter)
	{
		fprintf(pFile, "Node %d\n", lNodeCounter);
		fprintf(pFile, "\tPlane index : %d\n", pMap.mNodes[lNodeCounter].mPlane);
		fprintf(pFile, "\tChildren index : %d %d\n", pMap.mNodes[lNodeCounter].mChildren[0], pMap.mNodes[lNodeCounter].mChildren[1]);
		fprintf(pFile, "\tMin Bounding Box : %d %d %d\n", pMap.mNodes[lNodeCounter].mMins[0], pMap.mNodes[lNodeCounter].mMins[1], pMap.mNodes[lNodeCounter].mMins[2]);
		fprintf(pFile, "\tMax Bounding Box : %d %d %d\n", pMap.mNodes[lNodeCounter].mMaxs[0], pMap.mNodes[lNodeCounter].mMaxs[1], pMap.mNodes[lNodeCounter].mMaxs[2]);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* Leaf Lump *********\n");
	for
		(int lLeafCounter = 0; lLeafCounter < pMap.mLeaves.size(); ++lLeafCounter)
	{
		fprintf(pFile, "Leaf %d\n", lLeafCounter);
		fprintf(pFile, "\tCluster %d\n", pMap.mLeaves[lLeafCounter].mCluster);
		fprintf(pFile, "\tMin Bounding Box : %d %d %d\n", pMap.mLeaves[lLeafCounter].mMins[0], pMap.mLeaves[lLeafCounter].mMins[1], pMap.mLeaves[lLeafCounter].mMins[2]);
		fprintf(pFile, "\tMax Bounding Box : %d %d %d\n", pMap.mLeaves[lLeafCounter].mMaxs[0], pMap.mLeaves[lLeafCounter].mMaxs[1], pMap.mLeaves[lLeafCounter].mMaxs[2]);
		fprintf(pFile, "\tLeafFace %d\n", pMap.mLeaves[lLeafCounter].mLeafFace);
		fprintf(pFile, "\tNb LeafFace %d\n", pMap.mLeaves[lLeafCounter].mNbLeafFaces);
		fprintf(pFile, "\tLeafBrush %d\n", pMap.mLeaves[lLeafCounter].mLeafBrush);
		fprintf(pFile, "\tNb LeafBrushes %d\n", pMap.mLeaves[lLeafCounter].mNbLeafBrushes);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* LeafFace Lump *********\n");
	for
		(int lLeafFaceCounter = 0; lLeafFaceCounter < pMap.mLeafFaces.size(); ++lLeafFaceCounter)
	{
		fprintf(pFile, "LeafFace %d\n", lLeafFaceCounter);
		fprintf(pFile, "\tFaceIndex %d\n", pMap.mLeafFaces[lLeafFaceCounter].mFaceIndex);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* LeafBrush Lump *********\n");
	for
		(int lLeafBrushCounter = 0; lLeafBrushCounter < pMap.mLeafBrushes.size(); ++lLeafBrushCounter)
	{
		fprintf(pFile, "LeafBrush %d\n", lLeafBrushCounter);
		fprintf(pFile, "\tBrushIndex %d\n", pMap.mLeafBrushes[lLeafBrushCounter].mBrushIndex);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* Model Lump *********\n");
	for
		(int lModelCounter = 0; lModelCounter < pMap.mModels.size(); ++lModelCounter)
	{
		fprintf(pFile, "Model %d\n", lModelCounter);
		fprintf(pFile, "\tMin Bounding Box : %d %d %d\n", pMap.mModels[lModelCounter].mMins[0], pMap.mModels[lModelCounter].mMins[1], pMap.mModels[lModelCounter].mMins[2]);
		fprintf(pFile, "\tMax Bounding Box : %d %d %d\n", pMap.mModels[lModelCounter].mMaxs[0], pMap.mModels[lModelCounter].mMaxs[1], pMap.mModels[lModelCounter].mMaxs[2]);
		fprintf(pFile, "\tFace %d\n", pMap.mModels[lModelCounter].mFace);
		fprintf(pFile, "\tNbFaces %d\n", pMap.mModels[lModelCounter].mNbFaces);
		fprintf(pFile, "\tBrush %d\n", pMap.mModels[lModelCounter].mBrush);
		fprintf(pFile, "\tNbBrushes %d\n", pMap.mModels[lModelCounter].mNBrushes);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* Brush Lump *********\n");
	for
		(int lBrushCounter = 0; lBrushCounter < pMap.mBrushes.size(); ++lBrushCounter)
	{
		fprintf(pFile, "Brush %d\n", lBrushCounter);
		fprintf(pFile, "\tBrushSide %d\n", pMap.mBrushes[lBrushCounter].mBrushSide);
		fprintf(pFile, "\tNbBrushSides %d\n", pMap.mBrushes[lBrushCounter].mNbBrushSides);
		fprintf(pFile, "\tTextureIndex %d\n", pMap.mBrushes[lBrushCounter].mTextureIndex);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* BrushSide Lump *********\n");
	for
		(int lBrushSideCounter = 0; lBrushSideCounter < pMap.mBrushSides.size(); ++lBrushSideCounter)
	{
		fprintf(pFile, "BrushSide %d\n", lBrushSideCounter);
		fprintf(pFile, "\tPlaneIndex %d\n", pMap.mBrushSides[lBrushSideCounter].mPlaneIndex);
		fprintf(pFile, "\tTextureIndex %d\n", pMap.mBrushSides[lBrushSideCounter].mTextureIndex);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* Vertex Lump *********\n");
	for
		(int lVertexCounter = 0; lVertexCounter < pMap.mVertices.size(); ++lVertexCounter)
	{
		fprintf(pFile, "Vertex %d\n", lVertexCounter);
		fprintf(pFile, "\tPosition : %f %f %f\n", pMap.mVertices[lVertexCounter].mPosition[0], pMap.mVertices[lVertexCounter].mPosition[1], pMap.mVertices[lVertexCounter].mPosition[2]);
		fprintf(pFile, "\tTexCoord0 : %f %f\n", pMap.mVertices[lVertexCounter].mTexCoord[0], pMap.mVertices[lVertexCounter].mTexCoord[1]);
		fprintf(pFile, "\tTexCoord0 : %f %f\n", pMap.mVertices[lVertexCounter].mTexCoord[2], pMap.mVertices[lVertexCounter].mTexCoord[3]);
		fprintf(pFile, "\tNormal : %f %f %f\n", pMap.mVertices[lVertexCounter].mNormal[0], pMap.mVertices[lVertexCounter].mNormal[1], pMap.mVertices[lVertexCounter].mNormal[2]);
		fprintf(pFile, "\tColor : %d %d %d %d\n", pMap.mVertices[lVertexCounter].mColor[0], pMap.mVertices[lVertexCounter].mColor[1], pMap.mVertices[lVertexCounter].mColor[2], pMap.mVertices[lVertexCounter].mColor[3]);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* MeshVert Lump *********\n");
	for
		(int lMeshVertCounter = 0; lMeshVertCounter < pMap.mMeshVertices.size(); ++lMeshVertCounter)
	{
		fprintf(pFile, "MeshVert %d\n", lMeshVertCounter);
		fprintf(pFile, "\tVertex Index : %d\n", pMap.mMeshVertices[lMeshVertCounter].mMeshVert);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* Effect Lump *********\n");
	for
		(int lEffectCounter = 0; lEffectCounter < pMap.mEffects.size(); ++lEffectCounter)
	{
		fprintf(pFile, "Effect %d\n", lEffectCounter);
		fprintf(pFile, "\tName : %s\n", pMap.mEffects[lEffectCounter].mName);
		fprintf(pFile, "\tBrush : %d\n", pMap.mEffects[lEffectCounter].mBrush);
		fprintf(pFile, "\tUnknown : %d\n", pMap.mEffects[lEffectCounter].mUnknown);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* Face Lump *********\n");
	for
		(int lFaceCounter = 0; lFaceCounter < pMap.mFaces.size(); ++lFaceCounter)
	{
		fprintf(pFile, "Face %d\n", lFaceCounter);
		fprintf(pFile, "\tTextureIndex : %d\n", pMap.mFaces[lFaceCounter].mTextureIndex);
		fprintf(pFile, "\tEffectIndex : %d\n", pMap.mFaces[lFaceCounter].mEffectIndex);
		fprintf(pFile, "\tType : %d\n", pMap.mFaces[lFaceCounter].mType);
		fprintf(pFile, "\tVertex : %d\n", pMap.mFaces[lFaceCounter].mVertex);
		fprintf(pFile, "\tNbVertices : %d\n", pMap.mFaces[lFaceCounter].mNbVertices);
		fprintf(pFile, "\tMeshVertex : %d\n", pMap.mFaces[lFaceCounter].mMeshVertex);
		fprintf(pFile, "\tNbMeshVertices : %d\n", pMap.mFaces[lFaceCounter].mNbMeshVertices);
		fprintf(pFile, "\tLightMapIndex : %d\n", pMap.mFaces[lFaceCounter].mLightmapIndex);
		fprintf(pFile, "\tLightMapCorner : %d %d\n", pMap.mFaces[lFaceCounter].mLightmapCorner[0], pMap.mFaces[lFaceCounter].mLightmapCorner[1]);
		fprintf(pFile, "\tLightmapSize : %d %d\n", pMap.mFaces[lFaceCounter].mLightmapSize[0], pMap.mFaces[lFaceCounter].mLightmapSize[1]);
		fprintf(pFile, "\tLightmapOrigin : %f %f %f\n", pMap.mFaces[lFaceCounter].mLightmapOrigin[0], pMap.mFaces[lFaceCounter].mLightmapOrigin[1], pMap.mFaces[lFaceCounter].mLightmapOrigin[2]);
		fprintf(pFile, "\tLightmapVecs S : %f %f %f\n", pMap.mFaces[lFaceCounter].mLightmapVecs[0][0], pMap.mFaces[lFaceCounter].mLightmapVecs[0][1], pMap.mFaces[lFaceCounter].mLightmapVecs[0][2]);
		fprintf(pFile, "\tLightmapVecs T : %f %f %f\n", pMap.mFaces[lFaceCounter].mLightmapVecs[1][0], pMap.mFaces[lFaceCounter].mLightmapVecs[1][1], pMap.mFaces[lFaceCounter].mLightmapVecs[1][2]);
		fprintf(pFile, "\tNormal : %f %f %f\n", pMap.mFaces[lFaceCounter].mNormal[0], pMap.mFaces[lFaceCounter].mNormal[1], pMap.mFaces[lFaceCounter].mNormal[2]);
		fprintf(pFile, "\tPatchSize : %d %d\n", pMap.mFaces[lFaceCounter].mPatchSize[0], pMap.mFaces[lFaceCounter].mPatchSize[1]);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* LightMap Lump *********\n");
	fprintf(pFile, "NbLightMaps %d\n", pMap.mLightMaps.size());
	fprintf(pFile, "\n");

	fprintf(pFile, "********* LightVol Lump *********\n");
	for
		(int lLightVolCounter = 0; lLightVolCounter < pMap.mLightVols.size(); ++lLightVolCounter)
	{
		fprintf(pFile, "LightVol %d\n", lLightVolCounter);
		fprintf(pFile, "\tAmbient : %d %d %d\n", pMap.mLightVols[lLightVolCounter].mAmbient[0], pMap.mLightVols[lLightVolCounter].mAmbient[1], pMap.mLightVols[lLightVolCounter].mAmbient[2]);
		fprintf(pFile, "\tDirectional : %d %d %d\n", pMap.mLightVols[lLightVolCounter].mDirectional[0], pMap.mLightVols[lLightVolCounter].mDirectional[1], pMap.mLightVols[lLightVolCounter].mDirectional[2]);
		fprintf(pFile, "\tDir : %d %d\n", pMap.mLightVols[lLightVolCounter].mDir[0], pMap.mLightVols[lLightVolCounter].mDir[1]);
	}
	fprintf(pFile, "\n");

	fprintf(pFile, "********* VisData Lump *********\n");
	fprintf(pFile, "\tNbCluster %d\n", pMap.mVisData.mNbClusters);
	fprintf(pFile, "\tBytePerCluster %d\n", pMap.mVisData.mBytesPerCluster);
	fprintf(pFile, "\n");
}

/**
 * Read the map.
 *
 * @param pFilename	The filename of the map to read.
 *
 * @return true if the loading successed, false otherwise.
 */
bool readMap(const std::string& pFilename, TMapQ3& pMap)
{
	// Open the file.
	FILE* lFile = fopen(pFilename.c_str(), "r+b");

	// Check if the file exists.
	if
		(! lFile)
	{
		printf("readMap :: Invalid stream handle.\n");
		return false;
	}

	// Read the header.
	if
		(readHeader(lFile, pMap) == false)
	{
		printf("readMap :: Invalid Q3 map header.\n");
		return false;
	}

	// Read the entity lump.
	readEntity(lFile, pMap);

	// Read the texture lump.
	readTexture(lFile, pMap);

	// Read the plane lump.
	readPlane(lFile, pMap);

	// Read the node lump.
	readNode(lFile, pMap);

	// Read the leaf lump.
	readLeaf(lFile, pMap);

	// Read the leaf face lump.
	readLeafFace(lFile, pMap);

	// Read the leaf brush lump.
	readLeafBrush(lFile, pMap);

	// Read the model lump.
	readModel(lFile, pMap);

	// Read the brush lump.
	readBrush(lFile, pMap);

	// Read the brushside lump.
	readBrushSide(lFile, pMap);

	// Read the vertex lump.
	readVertex(lFile, pMap);

	// Read the meshvert lump.
	readMeshVert(lFile, pMap);

	// Read the effect lump.
	readEffect(lFile, pMap);

	// Read the face lump.
	readFace(lFile, pMap);

	// Read the lightmap lump.
	readLightMap(lFile, pMap);

	// Read the lightvol lump.
	readLightVol(lFile, pMap);

	// read the visdata lump.
	readVisData(lFile, pMap);
	
	// Close the file.
	fclose(lFile);

	return true;
};

/**
 * Free all the datas of the map.
 *tLightmapVecs
 * @param pMap	The Q3 map to free.
 */
void freeMap(TMapQ3& pMap)
{
	if
		(pMap.mEntity.mBuffer)
	{
		delete [] pMap.mEntity.mBuffer;
		pMap.mEntity.mBuffer = NULL;
	}

	if
		(pMap.mVisData.mBuffer)
	{
		delete [] pMap.mVisData.mBuffer;
		pMap.mVisData.mBuffer = NULL;
	}

	pMap.mTextures.clear();
	pMap.mPlanes.clear();
	pMap.mNodes.clear();			
	pMap.mLeaves.clear();		
	pMap.mLeafFaces.clear();		
	pMap.mLeafBrushes.clear();	
	pMap.mModels.clear();	
	pMap.mBrushes.clear();	
	pMap.mBrushSides.clear();
	pMap.mVertices.clear();	
	pMap.mMeshVertices.clear();
	pMap.mEffects.clear();
	pMap.mFaces.clear();	
	pMap.mLightMaps.clear();
	pMap.mLightVols.clear();
}

