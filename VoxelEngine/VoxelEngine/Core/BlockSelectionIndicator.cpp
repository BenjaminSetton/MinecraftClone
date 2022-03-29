#include "../Misc/pch.h"

#include "BlockSelectionIndicator.h"
#include "ChunkManager.h"
#include "Game.h"
#include "../Utility/Math.h"
#include "Player.h"
#include "ShaderBufferManagers/QuadBufferManager.h"
#include "../Utility/ImGuiDrawData.h"

using namespace DirectX;

float BlockSelectionIndicator::m_transitionDamping = 10.0f;

DirectX::XMFLOAT3 BlockSelectionIndicator::m_currentIndicatorPos = { 0.0f, 0.0f, 0.0f };
DirectX::XMFLOAT3 BlockSelectionIndicator::m_targetIndicatorPos = { 0.0f, 0.0f, 0.0f };
DirectX::XMFLOAT3 BlockSelectionIndicator::m_selectedBlockPos = { 0.0f, 0.0f, 0.0f };

void BlockSelectionIndicator::Update(const float dt)
{

	Player* player = Game::GetPrimaryPlayer();
	float epsilon = 0.001f;

	// Check if we're selecting a new block
	XMFLOAT3 rayPos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 rayDir = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 rayHit = { 0.0f, 0.0f, 0.0f };
	XMStoreFloat3(&rayPos, player->GetCamera(CameraType::FirstPerson)->GetWorldMatrix().r[3]);
	for (uint32_t i = 0; i < 3; i++) { XMFLOAT3_BRACKET_OP_32(rayPos, i) += epsilon; }
	XMStoreFloat3(&rayDir, DirectX::XMVector3Normalize(player->GetCamera(CameraType::FirstPerson)->GetWorldMatrix().r[2]));
	if (VX_MATH::Raycast(rayPos, rayDir, player->GetInteractionRange(), ChunkManager::CheckBlockRaycast, &rayHit))
	{
		// We're selecting a new block, move indicator towards it
		XMFLOAT3 newBlock = 
		{ 
			static_cast<float>(static_cast<int>((static_cast<int>(rayHit.x) != rayHit.x) && rayHit.x < 0.0f ? rayHit.x - 1.0f : rayHit.x)),
			static_cast<float>(static_cast<int>((static_cast<int>(rayHit.y) != rayHit.y) && rayHit.y < 0.0f ? rayHit.y - 1.0f : rayHit.y)),
			static_cast<float>(static_cast<int>((static_cast<int>(rayHit.z) != rayHit.z) && rayHit.z < 0.0f ? rayHit.z - 1.0f : rayHit.z))
		};
		if (!XMFLOAT3_IS_EQUAL(newBlock, m_selectedBlockPos))
		{
			m_targetIndicatorPos = { newBlock.x + 0.5f, newBlock.y + 0.5f, newBlock.z + 0.5f };
			m_selectedBlockPos = newBlock;

			// New target position when hitting block face
			for (uint32_t i = 0; i < 3; i++)
			{
				if (XMFLOAT3_BRACKET_OP_32(rayHit, i) == XMFLOAT3_BRACKET_OP_32(newBlock, i))
				{
					if (XMFLOAT3_BRACKET_OP_32(rayDir, i) < 0)
					{
						XMFLOAT3_BRACKET_OP_32(m_targetIndicatorPos, i) -= 1.0f;
						XMFLOAT3_BRACKET_OP_32(m_selectedBlockPos, i) -= 1.0f;
					}

					break;
				}
			}

		}

		// Update indicator towards target if we haven't reached it, whether target is new or not
		if (!XMFLOAT3_IS_EQUAL(m_targetIndicatorPos, m_currentIndicatorPos))
		{
			float dist = sqrt(pow2(m_targetIndicatorPos.x - m_currentIndicatorPos.x) + pow2(m_targetIndicatorPos.y - m_currentIndicatorPos.y) + pow2(m_targetIndicatorPos.z - m_currentIndicatorPos.z));
			if (dist > 2 * player->GetInteractionRange())
			{
				m_currentIndicatorPos = m_targetIndicatorPos;
			}
			else // lerp between prev blcok
			{
				m_currentIndicatorPos =
				{
					m_currentIndicatorPos.x + ((m_targetIndicatorPos.x - m_currentIndicatorPos.x) * m_transitionDamping * dt),
					m_currentIndicatorPos.y + ((m_targetIndicatorPos.y - m_currentIndicatorPos.y) * m_transitionDamping * dt),
					m_currentIndicatorPos.z + ((m_targetIndicatorPos.z - m_currentIndicatorPos.z) * m_transitionDamping * dt)
				};
			}
		}

		if (Renderer_Data::enableBlockSelector)
		{
			// Create quads
			QuadInstanceData quadData[6];
			GenerateQuadsInPos(m_currentIndicatorPos, quadData);

			// Send quads to QuadBufferManager
			QuadBufferManager::PushQuad(quadData[0]);
			QuadBufferManager::PushQuad(quadData[1]);
			QuadBufferManager::PushQuad(quadData[2]);
			QuadBufferManager::PushQuad(quadData[3]);
			QuadBufferManager::PushQuad(quadData[4]);
			QuadBufferManager::PushQuad(quadData[5]);
		}

		// DEBUG INFO

		Chunk* selectedChunk = ChunkManager::GetChunkAtPos(VX_MATH::WorldToChunkSpace(m_targetIndicatorPos));
		XMFLOAT3 selectedBlockPosCS = VX_MATH::ChunkToWorldSpace(VX_MATH::WorldToChunkSpace(m_targetIndicatorPos)); 
		XMFLOAT3 selectedBlockPosLocal = { m_targetIndicatorPos.x - selectedBlockPosCS.x, m_targetIndicatorPos.y - selectedBlockPosCS.y, m_targetIndicatorPos.z - selectedBlockPosCS.z};
		BlockType currentBlock = selectedChunk->GetBlock(static_cast<unsigned int>(selectedBlockPosLocal.x), static_cast<unsigned int>(selectedBlockPosLocal.y), static_cast<unsigned int>(selectedBlockPosLocal.z))->GetType();
		Renderer_Data::playerLookAt = { m_targetIndicatorPos.x - 0.5f, m_targetIndicatorPos.y - 0.5f , m_targetIndicatorPos.z - 0.5f };
		Renderer_Data::blockType = static_cast<uint8_t>(currentBlock);
		//
	}

}

void BlockSelectionIndicator::GenerateQuadsInPos(const XMFLOAT3& pos, QuadInstanceData* out_quadData)
{

	// FRONT
	out_quadData[0].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z - 0.5f));

	// LEFT
	out_quadData[1].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(XMConvertToRadians(90.0f)), DirectX::XMMatrixTranslation(pos.x - 0.5f, pos.y, pos.z));

	// BACK
	out_quadData[2].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(XMConvertToRadians(-180.0f)), DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z + 0.5f));
	
	// RIGHT
	out_quadData[3].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(XMConvertToRadians(-90.0f)), DirectX::XMMatrixTranslation(pos.x + 0.5f, pos.y, pos.z));

	// TOP
	out_quadData[4].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationX(XMConvertToRadians(90.0f)), DirectX::XMMatrixTranslation(pos.x, pos.y + 0.5f, pos.z));

	// BOTTOM
	out_quadData[5].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationX(XMConvertToRadians(-90.0f)), DirectX::XMMatrixTranslation(pos.x, pos.y - 0.5f, pos.z));
}