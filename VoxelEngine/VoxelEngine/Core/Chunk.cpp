#include "../Misc/pch.h"
#include "Chunk.h"
#include "../Utility/Noise.h"
#include "ChunkManager.h"


using namespace DirectX;


Chunk::Chunk(const XMFLOAT3 pos) : m_id(0), m_pos(pos), m_numFaces(0)
{
	InitializeChunk();
}

const uint32_t Chunk::GetID() { return m_id; }

Block* Chunk::GetBlock(unsigned int x, unsigned int y, unsigned int z) { return &m_chunk[x][y][z]; }

const DirectX::XMFLOAT3 Chunk::GetPosition() { return m_pos; }

const uint32_t Chunk::GetNumFaces() { return m_numFaces; }

BlockVertex* Chunk::GetFaceArray() { return &m_blockFaces[0]; }

const uint32_t Chunk::GetStartIndex() { return m_chunkStartIndex; }

void Chunk::InitializeChunk()
{
	//m_id = 0; // TODO: Replace with an actual UUID

	XMFLOAT3 posWS = { m_pos.x * CHUNK_SIZE, m_pos.y * CHUNK_SIZE, m_pos.z * CHUNK_SIZE };

	// Populate the chunk array
	// For now we'll just initialize all blocks to "DIRT" blocks
	for(int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			float height = Noise2D::GenerateValue(x + posWS.x, z + posWS.z) * static_cast<float>(CHUNK_SIZE);
			for(int y = 0; y < CHUNK_SIZE; y++)
			{
				if(y <= height) m_chunk[x][y][z] = Block(BlockType::Dirt);
				else m_chunk[x][y][z] = Block(BlockType::Air);
			}
		}
	}
}

void Chunk::InitializeVertexBuffer(const uint32_t startIndex)
{
	m_chunkStartIndex = startIndex;

	ResetFaces();

	// Retrieve neighboring chunks
	Chunk* leftChunk = ChunkManager::GetChunkAtPos({ m_pos.x - 1, m_pos.y, m_pos.z });
	Chunk* rightChunk = ChunkManager::GetChunkAtPos({ m_pos.x + 1, m_pos.y, m_pos.z });
	Chunk* topChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y + 1, m_pos.z });
	Chunk* bottomChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y - 1, m_pos.z });
	Chunk* frontChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y, m_pos.z - 1 });
	Chunk* backChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y, m_pos.z + 1 });

	XMFLOAT3 posWS = { m_pos.x * CHUNK_SIZE, m_pos.y * CHUNK_SIZE, m_pos.z * CHUNK_SIZE };

	uint32_t index = 0;
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int y = CHUNK_SIZE - 1; y >= 0; y--)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				XMFLOAT3 blockPos = 
				{ static_cast<float>(x) + posWS.x,
				  static_cast<float>(y) + posWS.y,
				  static_cast<float>(z) + posWS.z
				};

				// NOTE: appends faces that are not occluded (only render faces that can be seen)
				// RIGHT BACK AND TOP FACES RENDER TWICE

				// Only append faces if current block is not an air block
				if(m_chunk[x][y][z].GetType() != BlockType::Air)
				{
					// left limit 
					if (x - 1 < 0)
					{
						if(!leftChunk)
							AppendBlockFaceToVector(BlockFace::LEFT, index, blockPos);
						else if(leftChunk->GetBlock(CHUNK_SIZE - 1, y, z)->GetType() == BlockType::Air)
							AppendBlockFaceToVector(BlockFace::LEFT, index, blockPos);
					}
					// left neighbor
					else if(m_chunk[x - 1][y][z].GetType() == BlockType::Air)
						AppendBlockFaceToVector(BlockFace::LEFT, index, blockPos);

					// right limit
					if (x + 1 > CHUNK_SIZE - 1)
					{
						if (!rightChunk)
							AppendBlockFaceToVector(BlockFace::RIGHT, index, blockPos);
						else if(rightChunk->GetBlock(0, y, z)->GetType() == BlockType::Air)
							AppendBlockFaceToVector(BlockFace::RIGHT, index, blockPos);
					}
					// right neighbor
					else if(m_chunk[x + 1][y][z].GetType() == BlockType::Air)
						AppendBlockFaceToVector(BlockFace::RIGHT, index, blockPos);

					// top neighbor
					if (y + 1 > CHUNK_SIZE - 1)
					{
						if (!topChunk)
							AppendBlockFaceToVector(BlockFace::TOP, index, blockPos);
						else if(topChunk->GetBlock(x, 0, z)->GetType() == BlockType::Air)
							AppendBlockFaceToVector(BlockFace::TOP, index, blockPos);
					}
					else if(m_chunk[x][y + 1][z].GetType() == BlockType::Air)
						AppendBlockFaceToVector(BlockFace::TOP, index, blockPos);

					// bottom neighbor
					if (y - 1 < 0)
					{

						// This will be commented out since there are no vertical chunks yet

						//if (!bottomChunk)
						//	AppendBlockFaceToVector(BlockFace::BOTTOM, index, blockPos);
						//else 
						if(bottomChunk && bottomChunk->GetBlock(x, CHUNK_SIZE - 1, z)->GetType() == BlockType::Air)
							AppendBlockFaceToVector(BlockFace::BOTTOM, index, blockPos);
					}
					else if(m_chunk[x][y - 1][z].GetType() == BlockType::Air)
						AppendBlockFaceToVector(BlockFace::BOTTOM, index, blockPos);

					// front neighbor
					if (z - 1 < 0)
					{
						if (!frontChunk) 
							AppendBlockFaceToVector(BlockFace::FRONT, index, blockPos);
						else if(frontChunk->GetBlock(x, y, CHUNK_SIZE - 1)->GetType() == BlockType::Air)
							AppendBlockFaceToVector(BlockFace::FRONT, index, blockPos);
					}
					else if(m_chunk[x][y][z - 1].GetType() == BlockType::Air)
						AppendBlockFaceToVector(BlockFace::FRONT, index, blockPos);

					// back neighbor
					if (z + 1 > CHUNK_SIZE - 1)
					{
						if (!backChunk)
							AppendBlockFaceToVector(BlockFace::BACK, index, blockPos);
						else if(backChunk->GetBlock(x, y, 0)->GetType() == BlockType::Air)
							AppendBlockFaceToVector(BlockFace::BACK, index, blockPos);
					}
					else if(m_chunk[x][y][z + 1].GetType() == BlockType::Air)
						AppendBlockFaceToVector(BlockFace::BACK, index, blockPos);
				}

			}
		}
	}
}

void Chunk::AppendBlockFaceToVector(const BlockFace face, uint32_t& index, const XMFLOAT3& blockPos)
{
	uint32_t startIndexArray = 0;

	switch (face)
	{
	case BlockFace::BACK:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::BACK);
		break;
	}
	case BlockFace::FRONT:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::FRONT);
		break;
	}
	case BlockFace::LEFT:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::LEFT);
		break;
	}
	case BlockFace::RIGHT:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::RIGHT);
		break;
	}
	case BlockFace::TOP:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::TOP);
		break;
	}
	case BlockFace::BOTTOM:
	{
		startIndexArray = static_cast<uint32_t>(BlockFace::BOTTOM);
		break;
	}
	}

	// Append the 6 vertices to the array (an entire face)
	for(uint32_t i = 0; i < 6; i++)
	{
		BlockVertex currVert = verts[indicies[startIndexArray++]];
		currVert.pos = { currVert.pos.x + blockPos.x, currVert.pos.y + blockPos.y, currVert.pos.z + blockPos.z };
		m_blockFaces[index++] = currVert;
	}

	m_numFaces++;

}

void Chunk::ResetFaces()
{
	memset(&m_blockFaces[0], 0, sizeof(BlockVertex) * m_numFaces);
	m_numFaces = 0;
}
