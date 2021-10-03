#pragma once

#include <DirectXMath.h>

#include "DebugCamera.h"

class Player
{
public:

	Player();
	Player(const Player& other) = default;
	~Player();

	DebugCamera* GetCamera();
	void SetCamera(DebugCamera* camera);

	DirectX::XMFLOAT3 GetAcceleration();
	void SetAcceleration(DirectX::XMFLOAT3 accel);

	DirectX::XMFLOAT3 GetVelocity();
	void SetVelocity(DirectX::XMFLOAT3 vel);

	void Update(const float& dt);

private:

	DebugCamera* m_camera;

	DirectX::XMFLOAT3 m_acceleration;
	DirectX::XMFLOAT3 m_velocity;
	DirectX::XMFLOAT3 m_position;

	float m_movementSpeed;


};

