#pragma once
#include <Windows.h>
#include <gl/GL.h>
#include <malloc.h>
#include "../../common/glmath.h"
#include <vector> //for renderer vertex buffer, indices buffer
#include "../../common/rutility/rmemory.h"

// -------------------------------------------------------------------------------------
// Voxel Engine Main Part
// 
// Voxel regions/chunks operations defined here
// Marching cubes mesh generation defined here
// 
// Authors: Kikker, Catalyst
// Last update: 18.04.2022
// Updated: 02.07.2022
// 
// Tasks:
// - Add voxel sectors to chunks
// - Add chunks move
// 
// 
// -------------------------------------------------------------------------------------

typedef char chunk_cell_t;
typedef short voxel_cell_t;

struct vec3_compressed { 
	char x, y, z;
};

struct triangle_t {
	vec3 p[3];
};

/**
* Chunks functions
*/

#define CHUNK_SIZE(width, height) ((width * height * width) * sizeof(voxel))

//TODO: check this formula!
#define COORD2OFFSET(pchunk, x, y, z) ((x * (pchunk)->chunk_width + z) * (pchunk)->chunk_height + y) //(x * width + y) * width + z

#define OFFSET2COORD(x, y, z, pchunk, offset) \
    x = offset / ((pchunk)->chunk_width * (pchunk)->chunk_width)/* % ARRAY_SIZE*/;\
    y = (offset / (pchunk)->chunk_width) % (pchunk)->chunk_width;\
    z = offset % (pchunk)->chunk_width;

/**
* Voxel flags
*/
#define VOXEL_ANY	0
#define VOXEL_FLAG_AIR		(1 << 0)
#define VOXEL_FLAG_SOLID	(1 << 1)
#define VOXEL_FLAG_LIQUID	(1 << 2)
#define VOXEL_FLAG_SHAPE	(1 << 3) //вокруг вокселя будет построена фигура (куб и тд)
#define VOXEL_FLAG_NOTOUCH (1 << 4)

#define COORD2OFFSET2(w, h, x, y, z) ((y * w + x) * h + z) //(x * width + y) * width + z
//#define COORD2OFFSET2(w, x, y, z) ((y * w + z) * w + x) //(x * width + y) * width + z
//#define OFFSET2COORD2(x, y, z, w, offset) \
//    x = offset / (w * w)/* % ARRAY_SIZE*/;\
//    y = (offset / w) % w;\
//    z = offset % w;

class CChunk;

class CVoxel
{
public:
	CVoxel() {}
	~CVoxel() {}

	inline voxel_cell_t GetFlags();
	void SetFlag(voxel_cell_t flag);
	bool IsEmpty();
	bool IsSolid();
	bool IsLiquid();
private:
	voxel_cell_t m_Flags;
};

#define COMPUTE_CHUNK_SIZE(w, h) (w * h * w * sizeof(CVoxel))

#define VSF_INITIALIZED ( 1 << 1 )

class CVoxelSector
{
	int nFlags;
public:
	CVoxelSector() : nFlags(0), m_pVoxels(NULL), m_nWidth(16), m_nHeight(16), m_ChunkPos(0, 0, 0), m_vecMax(16, 16, 16) {
#ifdef DEBUG_DRAW
		m_nDDBounds = true;
#endif
	}
	CVoxelSector(vec3int pos, int width, int height);
	~CVoxelSector() {}

	int Init(vec3int pos, int width, int height, int flags = VOXEL_FLAG_AIR);
	int InitNoAlloc(vec3int pos, int width, int height);
	int AllocVoxels(int width, int height, int flags = VOXEL_FLAG_AIR);
	int FreeVoxels();

	void SetChunkPosition(vec3int pos);
	void SetChunkPosition(int x, int y, int z);
	void SetChunkSize(int width, int height);
	inline int GetLayerArea();
	int GetChunkWidth();
	int GetChunkHeight();
	int GetNumberVoxels();

	CVoxel *GetVoxels();
	CVoxel *SetVoxels(CVoxel *pvoxels);
	CVoxel *VoxelAt(int x, int y, int z);

	//__declspec(deprecated) bool DestroyVoxelByRay_Legacy(ray &r, int voxflags = VOXEL_FLAG_AIR);
	bool FindVoxelByRay(CVoxel **ppvoxel, vec3 *ppos, ray &r, int checkflag, float distance, float stepoccuracy);
	bool ChangeVoxelFlagsByRay(ray &r, float distance, int checkflag, int newflag);
	bool DestroyVoxel(ray &r, float distance, int voxflags = VOXEL_FLAG_AIR);
	bool PlaceVoxel(ray &r, float distance, int voxflags = VOXEL_FLAG_SOLID);

	//11.10.2021
	int VoxelsModifyRadial(vec3int pos, float radius, int voxflags);
	int VoxelsModifyCubical(vec3int min, vec3int max, int voxflags);

	bool VoxelInAir(int locx, int locy, int locz);
	bool VoxelOnGround(int locx, int locy, int locz);
	bool PointInChunk(int x, int y, int z);

	void MarchCube(vec3 min_corner_pos);
	void ComputeNormals();
	void DrawMesh(); //draw
	void ClearMesh();
	void BuildMesh();
	void RebuildMesh(); //rebuild mesh from voxels

	vec3 *GetVertices();
	int *GetIndices();
	int GetNumOfVertices();
	int GetNumOfIndices();

	// --- Flags ---
	int GetFlags();
	int SetFlags(int flag);

#ifdef DEBUG_DRAW
	void DebugDraw_ChunkBounds(bool b);
	void DebugDraw_ChunkVoxels(bool b);
	void DebugDraw_ChunkCubes(bool b);
	void DebugDraw_LastSelectTriangle(bool b);
	void DebugDraw_SetLastSelectTriangle(triangle_t &tri);

	bool m_nDDBounds;
	bool m_nDDVoxels;
	bool m_nDDCubes;
	bool m_bDDLastSelectTri;
	triangle_t m_LastSelectTriangle;
#endif

	CChunk *m_pLeft;
	CChunk *m_pRight;
	CChunk *m_pFront;
	CChunk *m_pBack;

	vec3int m_ChunkPos;
	vec3int m_vecMax;
//private:

	int m_nBytesInMemory;
	int m_nWidth, m_nHeight;
	int m_nMeshgenClipWidth;
	int m_nMeshgenClipHeight;
	CVoxel *m_pVoxels;
	std::vector<vec3> m_vertices;
	std::vector<vec3> m_normals;
	std::vector<vec2> m_uvs;
	std::vector<int> m_indices;
};


// 
// World Chunk
//

#define CF_INIT_ALL_SECTORS (1 << 1)

class CChunk
{
	int Flags;
	long chunkWidth;
	long chunkHeight;
	vec3int Position;

	size_t sectors_size;
	CVoxelSector *p_sectors;

public:
	CChunk() {}
	~CChunk() {}

	int Init(vec3int &position, int sectors_count, int flags, long chunk_width);
	int Shutdown();

	long GetChunkWidth();
	long GetChunkHeight();

#define CRB_BYIDX 0
#define CRB_ALL (1 << 1)
	bool RebuildMesh(size_t sector, int flags);

	void DrawChunk();
#ifdef DEBUG_DRAW
#endif

	CVoxel *GetVoxel(long x, long y, long z, int *pFlags);
};

//////////////////// 07.10.2021 ////////////////////////
/**
* Tests
*/
bool GetChunkMinByRay(vec3 &chunkmin, ray &r, float distance);