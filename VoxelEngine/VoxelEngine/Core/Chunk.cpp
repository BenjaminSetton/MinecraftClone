#include "../Misc/pch.h"
#include "Chunk.h"
#include "../Utility/Noise.h"
#include "../Utility/Utility.h"
#include "BlockUVs.h"

#include "ChunkManager.h"
#include "ChunkBufferManager.h"

#include "FrustumCulling.h"

#include "D3D.h"

#include "../Utility/Utility.h"
#include "../Utility/ImGuiLayer.h"


using namespace DirectX;

constexpr uint32_t BUFFER_SIZE = 6 * 6 * CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 0.1f;

constexpr int32_t TERRAIN_STARTING_HEIGHT = 105;
constexpr int32_t TERRAIN_HEIGHT_RANGE = 40;

constexpr int32_t LOW_CHUNK_LIMIT = -256;
constexpr int32_t HIGH_CHUNK_LIMIT = -LOW_CHUNK_LIMIT;


Chunk::Chunk(const DirectX::XMFLOAT3 pos) : m_pos(pos), m_vertexBufferStartIndex(0), m_vertexCount(0)
{
	//VX_PROFILE_SCOPE_MSG_MODE("Chunk Constructor", 1);

	InitializeChunk();
	CreateVertexBuffer();

}

Chunk::~Chunk()
{
	// This check will remain here until I find a better way
	// to notify other chunks that their starting index has
	// to change
	if(!ChunkManager::IsShuttingDown())
	{ 
		ShutdownVertexBuffer();
		VX_ASSERT(m_vertexCount == 0 && m_vertexBufferStartIndex == 0);
	}
}

Block* Chunk::GetBlock(unsigned int x, unsigned int y, unsigned int z) { return &m_chunk[x][y][z]; }

const DirectX::XMFLOAT3 Chunk::GetPosition() { return m_pos; }

const uint32_t Chunk::GetFaceCount() { return static_cast<uint32_t>(m_vertexCount / 6); }

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
	VX_ASSERT(TERRAIN_HEIGHT_RANGE > 0);

	XMFLOAT3 posWS = { m_pos.x * CHUNK_SIZE, m_pos.y * CHUNK_SIZE, m_pos.z * CHUNK_SIZE };

	// Populate the chunk array
	// For now we'll just initialize all blocks to "DIRT" blocks
	for(int64_t x = 0; x < CHUNK_SIZE; x++)
	{
		for (int64_t z = 0; z < CHUNK_SIZE; z++)
		{
			// Returns a values between MAXIMUM_TERRAIN_HEIGHT and MINIMUM_TERRAIN_HEIGHT
			float height = Noise2D::GenerateValue(static_cast<double>(x + posWS.x), static_cast<double>(z + posWS.z)) 
				* (TERRAIN_HEIGHT_RANGE) + TERRAIN_STARTING_HEIGHT;
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

	if (BlockShader_Data::enableFrustumCulling)
	{
		if (FrustumCulling::CalculateChunkPosAgainstFrustum(posWS)) return;
	}

	// Get start index
	size_t initialArraySize = ChunkBufferManager::GetVertexArray().size();

	// Retrieve neighboring chunks
	Chunk* leftChunk = ChunkManager::GetChunkAtPos({ m_pos.x - 1, m_pos.y, m_pos.z });
	Chunk* rightChunk = ChunkManager::GetChunkAtPos({ m_pos.x + 1, m_pos.y, m_pos.z });
	Chunk* topChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y + 1, m_pos.z });
	Chunk* bottomChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y - 1, m_pos.z });
	Chunk* frontChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y, m_pos.z - 1 });
	Chunk* backChunk = ChunkManager::GetChunkAtPos({ m_pos.x, m_pos.y, m_pos.z + 1 });

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

				// Only append faces if current block is not an air block
				BlockType blockType = m_chunk[x][y][z].GetType();
				if(blockType != BlockType::Air)
				{
					// left limit 
					if (x - 1 < 0)
					{
						if(leftChunk && leftChunk->GetBlock(CHUNK_SIZE - 1, y, z)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::LEFT, blockType, blockPos, ChunkBufferManager::GetVertexArray());
					}
					// left neighbor
					else if(m_chunk[x - 1][y][z].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::LEFT, blockType, blockPos, ChunkBufferManager::GetVertexArray());

					// right limit
					if (x + 1 > CHUNK_SIZE - 1)
					{
						if(rightChunk && rightChunk->GetBlock(0, y, z)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::RIGHT, blockType, blockPos, ChunkBufferManager::GetVertexArray());
					}
					// right neighbor
					else if(m_chunk[x + 1][y][z].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::RIGHT, blockType, blockPos, ChunkBufferManager::GetVertexArray());

					// top neighbor
					if (y + 1 > CHUNK_SIZE - 1)
					{
						if (topChunk && topChunk->GetBlock(x, 0, z)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::TOP, blockType, blockPos, ChunkBufferManager::GetVertexArray());
					}
					else if(m_chunk[x][y + 1][z].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::TOP, blockType, blockPos, ChunkBufferManager::GetVertexArray());

					// bottom neighbor
					if (y - 1 < 0)
					{
						if (bottomChunk && bottomChunk->GetBlock(x, CHUNK_SIZE - 1, z)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::BOTTOM, blockType, blockPos, ChunkBufferManager::GetVertexArray());
					}
					else if(m_chunk[x][y - 1][z].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::BOTTOM, blockType, blockPos, ChunkBufferManager::GetVertexArray());

					// front neighbor
					if (z - 1 < 0)
					{
						if(frontChunk && frontChunk->GetBlock(x, y, CHUNK_SIZE - 1)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::FRONT, blockType, blockPos, ChunkBufferManager::GetVertexArray());
					}
					else if(m_chunk[x][y][z - 1].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::FRONT, blockType, blockPos, ChunkBufferManager::GetVertexArray());

					// back neighbor
					if (z + 1 > CHUNK_SIZE - 1)
					{
						if(backChunk && backChunk->GetBlock(x, y, 0)->GetType() == BlockType::Air)
							AppendBlockFaceToArray(BlockFace::BACK, blockType, blockPos, ChunkBufferManager::GetVertexArray());
					}
					else if(m_chunk[x][y][z + 1].GetType() == BlockType::Air)
						AppendBlockFaceToArray(BlockFace::BACK, blockType, blockPos, ChunkBufferManager::GetVertexArray());
				}
			}
		}
	}

	// If vertices were allocated, populate the starting index
	if(m_vertexCount > 0) 
	{
		m_vertexBufferStartIndex = initialArraySize;
	}

}

void Chunk::ShutdownVertexBuffer()
{

	if(m_vertexCount > 0)
	{
		VX_ASSERT(m_vertexBufferStartIndex < ChunkBufferManager::GetVertexArray().size());

		// Remove vertices from the ChunkBufferManager
		ChunkBufferManager::GetVertexArray().erase(
			ChunkBufferManager::GetVertexArray().begin() + m_vertexBufferStartIndex,
			ChunkBufferManager::GetVertexArray().begin() + m_vertexBufferStartIndex + m_vertexCount);

		// Update all other chunk start indicies if they were displaced
		auto chunkVector = ChunkManager::GetChunkPool();
		for (uint32_t i = 0; i < chunkVector.Size(); i++)
		{
			Chunk* chunk = chunkVector[i];

			if (!chunk) continue;

			if (chunk->GetVertexBufferStartIndex() <= m_vertexBufferStartIndex ||
				chunk->GetVertexCount() <= 0) continue;

			// else update the chunks' start position
			int32_t newStartIndex = chunk->GetVertexBufferStartIndex() - m_vertexCount;
			VX_ASSERT(newStartIndex < ChunkBufferManager::GetVertexArray().size());
			VX_ASSERT(newStartIndex >= 0);
			chunk->SetVertexBufferStartIndex(newStartIndex);
		}
	}
	
	// Reset these variables
	m_vertexBufferStartIndex = m_vertexCount = 0;
}

void Chunk::CreateVertexBuffer()
{
	//HRESULT hr;

	//// Create the vertex buffer
	//D3D11_BUFFER_DESC vertexBufferDesc;
	//vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//vertexBufferDesc.ByteWidth = sizeof(BlockVertex) * BUFFER_SIZE;
	//vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vertexBufferDesc.CPUAccessFlags = 0;
	//vertexBufferDesc.MiscFlags = 0;
	//vertexBufferDesc.StructureByteStride = 0;

	//hr = D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &m_buffer);
	//if(FAILED(hr))
	//{
	//	hr = D3D::GetDevice()->GetDeviceRemovedReason();
	//	VX_ASSERT(false);
	//}
}

void Chunk::AppendBlockFaceToArray(const BlockFace& face, const BlockType& type, const XMFLOAT3& blockPos, 
	std::vector<BlockVertex>& out_blockArray)
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
		
		out_blockArray.push_back(currVert);

	}
	
	m_vertexCount += 6;

}
