#ifndef _PLAYER_H
#define _PLAYER_H

#include <DirectXMath.h>
#include "DebugCamera.h"

#include "PlayerController.h"

class Player
{
public:

	friend class PlayerController;

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

	float GetInteractionRange();
	void SetInteractionRange(float interactionRange);

	void Update(const float& dt);

private:

	DebugCamera* m_camera;

	DirectX::XMFLOAT3 m_acceleration;
	DirectX::XMFLOAT3 m_velocity;
	DirectX::XMFLOAT3 m_position;

	bool m_allowJump;

	float m_movementSpeed;

	float m_interactionRange;
};

#endif // _PLAYER_H
