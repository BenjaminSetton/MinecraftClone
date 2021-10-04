#ifndef _PLAYER_H
#define _PLAYER_H

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

	DirectX::XMFLOAT3 GetAcceleration() const;
	void SetAcceleration(const DirectX::XMFLOAT3 accel);

	DirectX::XMFLOAT3 GetVelocity() const;
	void SetVelocity(const DirectX::XMFLOAT3 vel);

	DirectX::XMFLOAT3 GetPosition() const;
	void SetPosition(const DirectX::XMFLOAT3 pos);

	void Update(const float& dt);

private:

	DebugCamera* m_camera;

	DirectX::XMFLOAT3 m_acceleration;
	DirectX::XMFLOAT3 m_velocity;
	DirectX::XMFLOAT3 m_position;

	float m_movementSpeed;


};

#endif // _PLAYER_H
