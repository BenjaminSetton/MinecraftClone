#include "../Misc/pch.h"

#include "BlockSelectionIndicator.h"
#include "Game.h"
#include "../Utility/Math.h"
#include "Player.h"
#include "ShaderBufferManagers/QuadBufferManager.h"

using namespace DirectX;

float BlockSelectionIndicator::m_transitionDamping = 0.5f;

DirectX::XMFLOAT3 BlockSelectionIndicator::m_currentIndicatorPos = { 0.0f, 0.0f, 0.0f };
DirectX::XMFLOAT3 BlockSelectionIndicator::m_targetIndicatorPos = { 0.0f, 0.0f, 0.0f };
DirectX::XMFLOAT3 BlockSelectionIndicator::m_selectedBlockPos = { 0.0f, 0.0f, 0.0f };

void BlockSelectionIndicator::Update(const float dt)
{
	Player* player = Game::GetPrimaryPlayer();

	// Check if we're selecting a new block
	XMFLOAT3 rayPos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 rayDir = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 rayHit = { 0.0f, 0.0f, 0.0f };
	XMStoreFloat3(&rayPos, player->GetCamera()->GetWorldMatrix().r[3]);
	XMStoreFloat3(&rayDir, DirectX::XMVector3Normalize(player->GetCamera()->GetWorldMatrix().r[2]));
	if (VX_MATH::Raycast(rayPos, rayDir, player->GetInteractionRange(), ChunkManager::CheckBlockRaycast, &rayHit))
	{
		// We're selecting a new block, move indicator towards it
		XMFLOAT3 newBlock = { static_cast<float>(static_cast<int>(rayHit.x)), static_cast<float>(static_cast<int>(rayHit.y)), static_cast<float>(static_cast<int>(rayHit.z)) };
		if (!XMFLOAT3_IS_EQUAL(newBlock, m_selectedBlockPos))
		{
			m_selectedBlockPos = newBlock;
			m_targetIndicatorPos = { newBlock.x + 0.5f, newBlock.y + 0.5f, newBlock.z + 0.5f };
		}

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

		// Update indicator towards target if we haven't reached it, whether target is new or not
		if (!XMFLOAT3_IS_EQUAL(m_targetIndicatorPos, m_currentIndicatorPos))
		{
			m_currentIndicatorPos =
			{
				m_currentIndicatorPos.x + ((m_targetIndicatorPos.x - m_currentIndicatorPos.x) * m_transitionDamping * dt),
				m_currentIndicatorPos.y + ((m_targetIndicatorPos.y - m_currentIndicatorPos.y) * m_transitionDamping * dt),
				m_currentIndicatorPos.z + ((m_targetIndicatorPos.z - m_currentIndicatorPos.z) * m_transitionDamping * dt)
			};
		}
	}

}

void BlockSelectionIndicator::GenerateQuadsInPos(const XMFLOAT3& pos, QuadInstanceData* out_quadData)
{

	// FRONT
	out_quadData[0].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixTranslation(0.0f, 0.0f, -0.5f));

	// LEFT
	out_quadData[1].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(-90.0f)), DirectX::XMMatrixTranslation(-0.5f, 0.0f, 0.0f));

	// BACK
	out_quadData[2].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(-180.0f)), DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.5f));
	
	// RIGHT
	out_quadData[3].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(90.0f)), DirectX::XMMatrixTranslation(0.5f, 0.0f, 0.0f));

	// TOP
	out_quadData[4].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(-90.0f)), DirectX::XMMatrixTranslation(0.0f, 0.5f, 0.0f));

	// BOTTOM
	out_quadData[5].transform = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(90.0f)), DirectX::XMMatrixTranslation(0.0f, -0.5f, 0.0f));
}