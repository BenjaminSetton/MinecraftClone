#include "../Misc/pch.h"
#include "Chunk.h"
#include "../Utility/Noise.h"
#include "../Utility/Utility.h"
#include "BlockUVs.h"

#include "ChunkManager.h"
#include "ChunkBufferManager.h"

#include "D3D.h"

#include "../Utility/Utility.h"


using namespace DirectX;

constexpr uint32_t BUFFER_SIZE = 6 * 6 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 0.1;

constexpr int32_t MINIMUM_TERRAIN_HEIGHT = 50;
constexpr int32_t MAXIMUM_TERRAIN_HEIGHT = 80;

constexpr int32_t LOW_CHUNK_LIMIT = -256;
constexpr int32_t HIGH_CHUNK_LIMIT = -LOW_CHUNK_LIMIT;


Chunk::Chunk(const DirectX::XMFLOAT3 pos) : m_pos(pos), m_numFaces(0), m_vertexBufferStartIndex(0), m_vertexCount(0)
{
	InitializeChunk();
	CreateVertexBuffer();
}

Chunk::~Chunk()
{
	m_buffer->Release();
}

Block* Chunk::GetBlock(unsigned int x, unsigned int y, unsigned int z) { return &m_chunk[x][y][z]; }

const DirectX::XMFLOAT3 Chunk::GetPosition() { return m_pos; }

const uint32_t Chunk::GetFaceCount() { return m_numFaces; }

const uint32_t Chunk::GetVertexCount() { return GetFaceCount() * 6; }

ID3D11Buffer* Chunk::GetBuffer() { return m_buffer; }

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

const uint32_t Chunk::GetVertexCount() { return m_vertexCount; }
void Chunk::SetVertexCount(const uint32_t vertexCount) { m_vertexCount = vertexCount; }

void Chunk::InitializeChunk()
{
	VX_ASSERT(MINIMUM_TERRAIN_HEIGHT < MAXIMUM_TERRAIN_HEIGHT);

	XMFLOAT3 posWS = { m_pos.x * CHUNK_SIZE, m_pos.y * CHUNK_SIZE, m_pos.z * CHUNK_SIZE };

	// Populate the chunk array
	// For now we'll just initialize all blocks to "DIRT" blocks
	for(int64_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (int64_t z = 0; z < CHUNK_SIZE; z++)
		{
			// Returns a values between MAXIMUM_TERRAIN_HEIGHT and MINIMUM_TERRAIN_HEIGHT
			float height = Noise2D::GenerateValue(static_cast<double>(x + posWS.x), static_cast<double>(z + posWS.z)) 
				* (MAXIMUM_TERRAIN_HEIGHT - MINIMUM_TERRAIN_HEIGHT) + MINIMUM_TERRAIN_HEIGHT;
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
	ResetFaces();

	// Retrieve neighboring chunks
	std::shared_ptr<Chunk> leftChunk = ChunkManager::GetChunkAtPos({ m_pos.x - 1, m_pos.y, m_pos.z });
	std::shared_ptr<Chunk> rightChunk = ChunkManager::GetChunkAtPos({ m_pos.x + 1, m_pos.y, m_pos.z });
	std::shared_ptr<Chunk> topChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y + 1, m_pos.z });
	std::shared_ptr<Chunk> bottomChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y - 1, m_pos.z });
	std::shared_ptr<Chunk> frontChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y, m_pos.z - 1 });
	std::shared_ptr<Chunk> backChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y, m_pos.z + 1 });

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
	if(FAILED(hr))
	{
		hr = D3D::GetDevice()->GetDeviceRemovedReason();
		VX_ASSERT(false);
	}
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
