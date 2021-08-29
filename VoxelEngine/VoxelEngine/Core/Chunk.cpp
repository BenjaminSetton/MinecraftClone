#include "../Misc/pch.h"
#include "Chunk.h"
#include "../Utility/Noise.h"
#include "../Utility/Utility.h"
#include "BlockUVs.h"

#include "ChunkManager.h"
#include "D3D.h"


using namespace DirectX;

constexpr uint32_t BUFFER_SIZE = 6 * 6 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 0.1;


Chunk::Chunk(const XMFLOAT3 pos) : m_id(0), m_pos(pos), m_numFaces(0)
{
	InitializeChunk();
	CreateVertexBuffer();
}

Chunk::~Chunk()
{
	m_buffer->Release();
}

const uint32_t Chunk::GetID() { return m_id; }

Block* Chunk::GetBlock(unsigned int x, unsigned int y, unsigned int z) { return &m_chunk[x][y][z]; }

const DirectX::XMFLOAT3 Chunk::GetPosition() { return m_pos; }

const uint32_t Chunk::GetFaceCount() { return m_numFaces; }

const uint32_t Chunk::GetVertexCount() { return GetFaceCount() * 6; }

ID3D11Buffer* Chunk::GetBuffer() { return m_buffer; }

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
				if(y <= height) m_chunk[x][y][z] = Block(BlockType::Grass);
				else m_chunk[x][y][z] = Block(BlockType::Air);
			}
		}
	}
}

void Chunk::InitializeVertexBuffer()
{
	ResetFaces();

	// Retrieve neighboring chunks
	Chunk* leftChunk = ChunkManager::GetChunkAtPos({ m_pos.x - 1, m_pos.y, m_pos.z });
	Chunk* rightChunk = ChunkManager::GetChunkAtPos({ m_pos.x + 1, m_pos.y, m_pos.z });
	Chunk* topChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y + 1, m_pos.z });
	Chunk* bottomChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y - 1, m_pos.z });
	Chunk* frontChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y, m_pos.z - 1 });
	Chunk* backChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y, m_pos.z + 1 });

	XMFLOAT3 posWS = { m_pos.x * CHUNK_SIZE, m_pos.y * CHUNK_SIZE, m_pos.z * CHUNK_SIZE };


	// Local array (deleted after loop ends and buffer is updated)
	BlockVertex* blockFaces = new BlockVertex[BUFFER_SIZE];

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
				BlockType blockType = m_chunk[x][y][z].GetType();
				if(blockType != BlockType::Air)
				{
					// left limit 
					if (x - 1 < 0)
					{
						if(leftChunk && leftChunk->GetBlock(CHUNK_SIZE - 1, y, z)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::LEFT, blockType, index, blockPos, blockFaces);
					}
					// left neighbor
					else if(m_chunk[x - 1][y][z].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::LEFT, blockType, index, blockPos, blockFaces);

					// right limit
					if (x + 1 > CHUNK_SIZE - 1)
					{
						if(rightChunk && rightChunk->GetBlock(0, y, z)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::RIGHT, blockType, index, blockPos, blockFaces);
					}
					// right neighbor
					else if(m_chunk[x + 1][y][z].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::RIGHT, blockType, index, blockPos, blockFaces);

					// top neighbor
					if (y + 1 > CHUNK_SIZE - 1)
					{
						if (!topChunk)
							AppendBlockFaceToArray(BlockFace::TOP, blockType, index, blockPos, blockFaces);
						else if(topChunk->GetBlock(x, 0, z)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::TOP, blockType, index, blockPos, blockFaces);
					}
					else if(m_chunk[x][y + 1][z].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::TOP, blockType, index, blockPos, blockFaces);

					// bottom neighbor
					if (y - 1 < 0)
					{

						// This will be commented out since there are no vertical chunks yet

						//if (!bottomChunk)
						//	AppendBlockFaceToArray(BlockFace::BOTTOM, index, blockPos);
						//else 
						if(bottomChunk && bottomChunk->GetBlock(x, CHUNK_SIZE - 1, z)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::BOTTOM, blockType, index, blockPos, blockFaces);
					}
					else if(m_chunk[x][y - 1][z].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::BOTTOM, blockType, index, blockPos, blockFaces);

					// front neighbor
					if (z - 1 < 0)
					{
						if(frontChunk && frontChunk->GetBlock(x, y, CHUNK_SIZE - 1)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::FRONT, blockType, index, blockPos, blockFaces);
					}
					else if(m_chunk[x][y][z - 1].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::FRONT, blockType, index, blockPos, blockFaces);

					// back neighbor
					if (z + 1 > CHUNK_SIZE - 1)
					{
						if(backChunk && backChunk->GetBlock(x, y, 0)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::BACK, blockType, index, blockPos, blockFaces);
					}
					else if(m_chunk[x][y][z + 1].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::BACK, blockType, index, blockPos, blockFaces);
				}

			}
		}
	}

	// Update Buffer
	D3D::GetDeviceContext()->UpdateSubresource(m_buffer, 0, nullptr, blockFaces, 0, 0);

	// Delete array
	delete[] blockFaces;

}

void Chunk::CreateVertexBuffer()
{
	HRESULT hr;

	// Create the vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(BlockVertex) * BUFFER_SIZE;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	hr = D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_buffer);
	VX_ASSERT(!FAILED(hr));
}

void Chunk::AppendBlockFaceToArray(const BlockFace& face, const BlockType& type, uint32_t& index, const XMFLOAT3& blockPos, 
	BlockVertex* blockArray)
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
		BlockVertex currVert = verts[indicies[startIndexArray]];
		currVert.pos = { currVert.pos.x + blockPos.x, currVert.pos.y + blockPos.y, currVert.pos.z + blockPos.z };
		currVert.uv = GetUVsForBlock(type, indicies[startIndexArray++]);
		blockArray[index++] = currVert;
	}

	m_numFaces++;

}

void Chunk::ResetFaces()
{
	m_numFaces = 0;
}
