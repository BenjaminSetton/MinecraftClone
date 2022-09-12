#ifndef _PLAYER_H
#define _PLAYER_H

#include <DirectXMath.h>
#include "Camera.h"

#include "PlayerController.h"
#include "Physics.h"

namespace Orange
{
	enum class CameraType
	{
		FirstPerson,
		ThirdPerson,
		Debug
	};

	class Player
	{
	public:

		friend class PlayerController;

		Player();
		Player(const Player& other) = default;
		~Player();

		Camera* GetCamera(const CameraType type);
		void SetCamera(Camera* camera, const CameraType type);

		DirectX::XMFLOAT3 GetAcceleration() const;
		void SetAcceleration(const DirectX::XMFLOAT3 accel);

		DirectX::XMFLOAT3 GetVelocity() const;
		void SetVelocity(const DirectX::XMFLOAT3 vel);

		DirectX::XMFLOAT3 GetPosition() const;
		void SetPosition(const DirectX::XMFLOAT3 pos);

		float GetInteractionRange();
		void SetInteractionRange(float interactionRange);

		DirectX::XMFLOAT3 GetRotation() const;
		void SetRotation(const DirectX::XMFLOAT3 rot);

		AABB GetHitbox() const;
		void SetHitbox(const AABB& hitbox);

		CameraType GetSelectedCameraType() const;
		void SetSelectedCameraType(const CameraType cameraType);

		void Update(const float& dt);

	private:

		Camera* m_FPSCamera;
		Camera* m_debugCamera;
		CameraType m_selectedCameraType;

		DirectX::XMFLOAT3 m_acceleration;
		DirectX::XMFLOAT3 m_velocity;
		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT3 m_rotation; // in degrees

		AABB m_hitbox;

		bool m_allowJump;

		float m_movementSpeed;

		float m_interactionRange;
	};
}


#endif // _PLAYER_H
