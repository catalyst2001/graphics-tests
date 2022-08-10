#pragma once
#include "../../common/glmath.h"
#include "voxel.h"

// 
// CWorldEntity
// 
// World entity data for restore
// 
class CWorldEntity
{
	size_t entid;
	size_t target_entid;
	long entflags;
	vec3 position;
	vec3 rotation;
public:
	CWorldEntity() {}
	~CWorldEntity() {}


};

// 
// CWorldContext
// 
// World context
// 
class CWorldContext
{
	size_t num_of_entities;
public:
	CWorldContext() {}
	~CWorldContext() {}


};

#define DEFAULT_SEED vec3(0.f, 0.f, 0.f)
#define DEFAULT_CHUNKS_LOADING_DISTANCE 20
#define DEFAULT_CHUNK_SECTOR_SIZE 16
#define DEFAULT_CHUNK_SECTORS 5

// 
// CWorldInformation
// 
// Basic world information class
// 
class CWorldInformation
{
	vec3 seed;				     //seed XYZ
	vec3 world_min;				 //world minimum coordinates in units
	vec3 world_max;				 //world maximum coordinates in units
	int chunks_load_distance;    //chunks loading distance
	int sector_size;		     //size one chunk sector that is (16x16x16) or (10x10x10) e.t.c
	size_t chunk_sectors_count;	 //chunk max sectors
	size_t total_chunks;		 //all chunks count in world
	size_t loaded_chunks;		 //number of currently loaded chunks
	size_t loaded_regions;		 //number of currently loaded world regions
public:
	CWorldInformation() : seed(DEFAULT_SEED), world_min(vec3(0.f, 0.f, 0.f)), world_max(vec3(0.f, 0.f, 0.f)) {
		chunks_load_distance = DEFAULT_CHUNKS_LOADING_DISTANCE;
		sector_size = DEFAULT_CHUNK_SECTOR_SIZE;
		chunk_sectors_count = DEFAULT_CHUNK_SECTORS;
		total_chunks = 0;
		loaded_chunks = 0;
		loaded_regions = 0;
	}
	~CWorldInformation() {}

	void SetWorldSeed(const vec3 &wseed) { seed = wseed; }
	vec3 GetWorldSeed() { return seed; }
	void SetWorldMin(const vec3 &wmin) { world_min = wmin; }
	void GetWorldMin(vec3 &dst_wmin) { dst_wmin = world_min; }
	void SetWorldMax(const vec3 &src_wmax) { world_max = src_wmax; }
	void GetWorldMax(vec3 &dst_wmax) { dst_wmax = world_max; }
	void SetChunksLoadDistance(int nchunks_load_distance) { chunks_load_distance = nchunks_load_distance; }
	int  GetChunksLoadDistance() { return chunks_load_distance; }
	void SetChunkSectorSize(int sectorsize) { sector_size = sectorsize; }
	int  GetChunkSectorSize() { return sector_size; }
	void SetChunkSectorsNumber(size_t count) { chunk_sectors_count = count; }
	size_t GetChunkSectorsNumber() { return chunk_sectors_count; }
	void SetWorldTotalChunksNumber(size_t count) { total_chunks = count; }
	size_t GetWorldTotalChunksNumber() { return total_chunks; }
	void SetLoadedChunksNumber(size_t count) { loaded_chunks = count; }
	size_t GetLoadedChunksNumber() { return loaded_chunks; }
	void SetLoadedRegionsNumber(size_t count) { loaded_regions = count; }
	size_t GetLoadedRegionsNumber() { return loaded_regions; }
};

// 
// CWorld
// 
// Main world class
// 
class CWorld
{
	CWorldInformation worldinfo;
	CChunk *p_chunks;
public:
	CWorld() {}
	~CWorld() {}

};
