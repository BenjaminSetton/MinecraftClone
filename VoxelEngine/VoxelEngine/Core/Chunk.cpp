#include "../Misc/pch.h"
#include "Chunk.h"
#include "../Utility/Utility.h"
#include "BlockUVs.h"

#include "ChunkManager.h"
#include "ChunkBufferManager.h"

#include "FrustumCulling.h"

#include "D3D.h"

#include "../Utility/Utility.h"
#include "../Utility/ImGuiLayer.h"
#include "../Utility/Math.h"
#include "../Utility/SimplexNoise.h"


using namespace DirectX;

constexpr uint32_t BUFFER_SIZE = 6 * 6 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 0.1f;

constexpr int32_t TERRAIN_STARTING_HEIGHT = 80;
constexpr int32_t TERRAIN_HEIGHT_RANGE = 50;

constexpr int32_t LOW_CHUNK_LIMIT = -256;
constexpr int32_t HIGH_CHUNK_LIMIT = -LOW_CHUNK_LIMIT;


Chunk::Chunk(const DirectX::XMFLOAT3 pos) : m_pos(pos), m_vertexBufferStartIndex(0), m_blockCount(0) 
{

}

Chunk::~Chunk()
{
	// This check will remain here until I find a better way
	// to notify other chunks that their starting index has
	// to change
	if(!ChunkManager::IsShuttingDown())
	{ 
		ShutdownVertexBuffer();
		VX_ASSERT(m_blockCount == 0 && m_vertexBufferStartIndex == 0);
	}
}

Block* Chunk::GetBlock(unsigned int x, unsigned int y, unsigned int z) { return &m_chunk[x][y][z]; }

const DirectX::XMFLOAT3 Chunk::GetPosition() { return m_pos; }

const uint32_t Chunk::GetFaceCount() { return static_cast<uint32_t>(m_blockCount * 6); }

void Chunk::DrawChunkBorder()
{
	XMFLOAT4 color = { 1.0f, 0.0f, 0.0f, 1.0f };

	XMFLOAT3 posWS = { m_pos.x * CHUNK_SIZE, m_pos.y * CHUNK_SIZE, m_pos.z * CHUNK_SIZE };
	XMFLOAT3 tlf = { posWS.x, posWS.y, posWS.z };
	XMFLOAT3 trf = { posWS.x + CHUNK_SIZE, posWS.y, posWS.z };
	XMFLOAT3 blf = { posWS.x, posWS.y - CHUNK_SIZE, posWS.z };
	XMFLOAT3 brf = { posWS.x + CHUNK_SIZE, posWS.y - CHUNK_SIZE, posWS.z };
	XMFLOAT3 tln = { posWS.x, posWS.y, posWS.z + CHUNK_SIZE };
	XMFLOAT3 trn = { posWS.x + CHUNK_SIZE, posWS.y, posWS.z + CHUNK_SIZE };
	XMFLOAT3 bln = { posWS.x, posWS.y - CHUNK_SIZE, posWS.z + CHUNK_SIZE };
	XMFLOAT3 brn = { posWS.x + CHUNK_SIZE, posWS.y - CHUNK_SIZE, posWS.z + CHUNK_SIZE };

	// Back
	DebugLine::AddLine(tln, trn, color);
	DebugLine::AddLine(trn, brn, color);
	DebugLine::AddLine(brn, bln, color);
	DebugLine::AddLine(bln, tln, color);

	// Front
	DebugLine::AddLine(tlf, trf, color);
	DebugLine::AddLine(trf, brf, color);
	DebugLine::AddLine(brf, blf, color);
	DebugLine::AddLine(blf, tlf, color);

	// Left side
	DebugLine::AddLine(tln, tlf, color);
	DebugLine::AddLine(blf, bln, color);

	// Right side
	DebugLine::AddLine(trn, trf, color);
	DebugLine::AddLine(brf, brn, color);
}

const uint32_t Chunk::GetVertexBufferStartIndex() { return m_vertexBufferStartIndex; }
void Chunk::SetVertexBufferStartIndex(const uint32_t startIndex) { m_vertexBufferStartIndex = startIndex; }

const uint32_t Chunk::GetBlockCount() { return m_blockCount; }
void Chunk::SetVertexCount(const uint32_t vertexCount) { m_blockCount = vertexCount; }

void Chunk::Init()
{
	VX_ASSERT(TERRAIN_HEIGHT_RANGE > 0);

	XMFLOAT3 posWS = { m_pos.x * CHUNK_SIZE, m_pos.y * CHUNK_SIZE, m_pos.z * CHUNK_SIZE };

	// Populate the chunk array
	// For now we'll just initialize all blocks to "DIRT" blocks
	for(int64_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (int64_t z = 0; z < CHUNK_SIZE; z++)
		{
			// Returns a values between MAXIMUM_TERRAIN_HEIGHT and MINIMUM_TERRAIN_HEIGHT
			SimplexNoise noiseGenerator( 0.012f, 1.0f, 2.0f, 0.5f );
			float sampledNoise = noiseGenerator.fractal(4, static_cast<double>(x + posWS.x), static_cast<double>(z + posWS.z));
			float height = ((sampledNoise * 0.5f + 0.5f) * TERRAIN_HEIGHT_RANGE) + TERRAIN_STARTING_HEIGHT;
			for(int64_t y = 0; y < CHUNK_SIZE; y++)
			{
				float yWS = posWS.y + y;
				if(yWS <= static_cast<int32_t>(height))
					m_chunk[x][y][z] = Block(BlockType::Grass);
				else 
					m_chunk[x][y][z] = Block(BlockType::Air);
			}
		}
	}
}

void Chunk::InitializeVertexBuffer()
{
	// We need this here because the chunk will not necessarily
	// be deleted if it's being used by other system because
	// of the shared_ptr
	ShutdownVertexBuffer();

	XMFLOAT3 posWS = { m_pos.x * CHUNK_SIZE, m_pos.y * CHUNK_SIZE, m_pos.z * CHUNK_SIZE };

	// Get start index
	uint32_t initialArraySize = ChunkBufferManager::GetVertexArray().size();

	// Retrieve neighboring chunks
	Chunk* leftChunk = ChunkManager::GetChunkAtPos({ m_pos.x - 1, m_pos.y, m_pos.z });
	Chunk* rightChunk = ChunkManager::GetChunkAtPos({ m_pos.x + 1, m_pos.y, m_pos.z });
	Chunk* topChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y + 1, m_pos.z });
	Chunk* bottomChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y - 1, m_pos.z });
	Chunk* frontChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y, m_pos.z - 1 });
	Chunk* backChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y, m_pos.z + 1 });

	// Retrieve a reference to the vertex array
	auto& vertexArray = ChunkBufferManager::GetVertexArray();

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = CHUNK_SIZE - 1; y >= 0; y--)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				XMFLOAT3 blockPos = 
				{ 
					static_cast<float>(x) + posWS.x,
					static_cast<float>(y) + posWS.y,
					static_cast<float>(z) + posWS.z
				};

				unsigned int blockFaces = 0;

				// Only append new block if the block type is not an air block
				BlockType blockType = m_chunk[x][y][z].GetType();
				if (blockType != BlockType::Air)
				{
					// left limit 
					if (x - 1 < 0)
					{
						if (leftChunk && leftChunk->GetBlock(CHUNK_SIZE - 1, y, z)->GetType() == BlockType::Air)
							blockFaces |= static_cast<unsigned int>(BlockFace::LEFT);
					}
					// left neighbor
					else if (m_chunk[x - 1][y][z].GetType() == BlockType::Air)
						blockFaces |= static_cast<unsigned int>(BlockFace::LEFT);

					// right limit
					if (x + 1 > CHUNK_SIZE - 1)
					{
						if (rightChunk && rightChunk->GetBlock(0, y, z)->GetType() == BlockType::Air)
							blockFaces |= static_cast<unsigned int>(BlockFace::RIGHT);
					}
					// right neighbor
					else if (m_chunk[x + 1][y][z].GetType() == BlockType::Air)
						blockFaces |= static_cast<unsigned int>(BlockFace::RIGHT);

					// top neighbor
					if (y + 1 > CHUNK_SIZE - 1)
					{
						if (topChunk && topChunk->GetBlock(x, 0, z)->GetType() == BlockType::Air)
							blockFaces |= static_cast<unsigned int>(BlockFace::TOP);
					}
					else if (m_chunk[x][y + 1][z].GetType() == BlockType::Air)
						blockFaces |= static_cast<unsigned int>(BlockFace::TOP);

					// bottom neighbor
					if (y - 1 < 0)
					{
						if (bottomChunk && bottomChunk->GetBlock(x, CHUNK_SIZE - 1, z)->GetType() == BlockType::Air)
							blockFaces |= static_cast<unsigned int>(BlockFace::BOTTOM);
					}
					else if (m_chunk[x][y - 1][z].GetType() == BlockType::Air)
						blockFaces |= static_cast<unsigned int>(BlockFace::BOTTOM);

					// front neighbor
					if (z - 1 < 0)
					{
						if (frontChunk && frontChunk->GetBlock(x, y, CHUNK_SIZE - 1)->GetType() == BlockType::Air)
							blockFaces |= static_cast<unsigned int>(BlockFace::FRONT);
					}
					else if (m_chunk[x][y][z - 1].GetType() == BlockType::Air)
						blockFaces |= static_cast<unsigned int>(BlockFace::FRONT);


					// back neighbor
					if (z + 1 > CHUNK_SIZE - 1)
					{
						if (backChunk && backChunk->GetBlock(x, y, 0)->GetType() == BlockType::Air)
							blockFaces |= static_cast<unsigned int>(BlockFace::BACK);
					}
					else if (m_chunk[x][y][z + 1].GetType() == BlockType::Air)
						blockFaces |= static_cast<unsigned int>(BlockFace::BACK);


					// Add the new block to the ChunkBufferManager vertex array if we can render faces
					if(blockFaces != 0)
					{
						BlockInstanceData currBlock;
						currBlock.blockFaces = blockFaces;
						currBlock.blockType = static_cast<unsigned int>(blockType);
						currBlock.worldPos = blockPos;

						vertexArray.emplace_back(currBlock);

						m_blockCount++;
					}
				}
			}
		}
	}


	// If vertices were allocated, populate the starting index
	if(m_blockCount > 0) 
	{
		m_vertexBufferStartIndex = initialArraySize;

		//VX_LOG("[%2.2f, %2.2f, %2.2f] Added %i blocks at index %i (to %i) NS %i",
		//	m_pos.x, m_pos.y, m_pos.z, m_blockCount, 
		//	m_vertexBufferStartIndex, m_vertexBufferStartIndex + m_blockCount, vertexArray.size());
	}


	

}

void Chunk::ShutdownVertexBuffer()
{

	if(m_blockCount > 0)
	{

		auto& vertexArray = ChunkBufferManager::GetVertexArray();
		VX_ASSERT(m_vertexBufferStartIndex < vertexArray.size());

		int debug_vertexArraySize = vertexArray.size();

		if(m_blockCount == 34)
		{
			int test = 0;// VX_ASSERT_MSG(false, "fuck this");
		}

		// Remove vertices from the ChunkBufferManager
		vertexArray.erase(vertexArray.begin() + m_vertexBufferStartIndex, vertexArray.begin() + m_vertexBufferStartIndex + m_blockCount);

		//VX_LOG("[%2.2f, %2.2f, %2.2f] Removed %i blocks at index %i (to %i) NS %i", m_pos.x, m_pos.y, m_pos.z,
		//	m_blockCount, m_vertexBufferStartIndex, m_vertexBufferStartIndex + m_blockCount, vertexArray.size());

		auto chunkPool = ChunkManager::GetChunkPool();
		for (uint32_t i = 0; i < chunkPool.Size(); i++)
		{
			Chunk* chunk = chunkPool[i];

			if  (
				chunk->GetVertexBufferStartIndex() < m_vertexBufferStartIndex   ||
				chunk->GetBlockCount() == 0										||
				chunk == this
				) continue;

			// else update the chunks' start position
			int32_t newStartIndex = chunk->GetVertexBufferStartIndex() - m_blockCount;
			VX_ASSERT(newStartIndex < vertexArray.size());
			VX_ASSERT(newStartIndex >= 0);
			chunk->SetVertexBufferStartIndex(newStartIndex);
		}
	
	}
	
	// Reset these variables
	m_vertexBufferStartIndex = m_blockCount = 0;
}

