#ifndef _BLOCKSELECTIONINDICATOR_H
#define _BLOCKSELECTIONINDICATOR_H

#include <d3d11.h>
#include <DirectXMath.h>

class QuadInstanceData;

class BlockSelectionIndicator
{
public:

	BlockSelectionIndicator() = delete;
	BlockSelectionIndicator(const BlockSelectionIndicator& other) = delete;
	~BlockSelectionIndicator() = delete;

	static void Update(const float dt);

private:

	static void GenerateQuadsInPos(const DirectX::XMFLOAT3& pos, QuadInstanceData* out_quadData);

private:

	static float m_transitionDamping;

	// Always points to block midpoints
	static DirectX::XMFLOAT3 m_currentIndicatorPos;
	static DirectX::XMFLOAT3 m_targetIndicatorPos;

	// Points to integer position only
	static DirectX::XMFLOAT3 m_selectedBlockPos;

};

#endif
