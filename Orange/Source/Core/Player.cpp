#include "../Misc/pch.h"
#include "Player.h"

#include "../Utility/Input.h"
#include "PlayerController.h"

using namespace DirectX;

namespace Orange
{
	Player::Player() :	m_acceleration({0.0f, 0.0f, 0.0f}), m_velocity({0.0f, 0.0f, 0.0f}), m_FPSCamera(nullptr), m_debugCamera(nullptr), m_selectedCameraType(CameraType::FirstPerson),
						m_movementSpeed(7.0f), m_position({ -4.0f, 150.0f, -10.0f }), m_rotation({ 0.0f, 0.0f, 0.0f }), m_allowJump(false), m_interactionRange(7.0f)
	{
		m_FPSCamera = new Camera(0.4f, 20.0f);
		m_FPSCamera->ConstructMatrix(m_position, m_rotation);

		m_debugCamera = new Camera(0.5f, 22.0f);
		m_debugCamera->ConstructMatrix(m_position, m_rotation);

		m_hitbox.center = { m_position.x, m_position.y - 2.0f, m_position.z };
		m_hitbox.extent = { 0.25f, 1.0f, 0.25f };
	}

	Player::~Player()
	{
		delete m_FPSCamera;
		delete m_debugCamera;
	}

	Camera* Player::GetCamera(const CameraType type) 
	{
		switch (type)
		{
		case CameraType::FirstPerson:
			return m_FPSCamera;
		case CameraType::Debug:
			return m_debugCamera;
		case CameraType::ThirdPerson:
			//Unimplemented
			return nullptr;
		default:
		{
			OG_ASSERT_MSG(false, "WTF?");
			return nullptr;
		}
		}
	}
	void Player::SetCamera(Camera* camera, const CameraType type)
	{
		switch (type)
		{
		case CameraType::FirstPerson:
			m_FPSCamera = camera;
		case CameraType::Debug:
			m_debugCamera = camera;
		case CameraType::ThirdPerson:
			return;
		default:
		{
			OG_ASSERT_MSG(false, "WTF?");
		}

		}
	}

	DirectX::XMFLOAT3 Player::GetAcceleration() const { return m_acceleration; }
	void Player::SetAcceleration(const DirectX::XMFLOAT3 accel) { m_acceleration = accel; }

	DirectX::XMFLOAT3 Player::GetVelocity() const { return m_velocity; }
	void Player::SetVelocity(const DirectX::XMFLOAT3 vel) { m_velocity = vel; }

	DirectX::XMFLOAT3 Player::GetPosition() const { return m_position; }
	void Player::SetPosition(const DirectX::XMFLOAT3 pos) { m_position = pos; }

	float Player::GetInteractionRange() { return m_interactionRange; }
	void Player::SetInteractionRange(float interactionRange) { m_interactionRange = interactionRange; }

	DirectX::XMFLOAT3 Player::GetRotation() const { return m_rotation; }
	void Player::SetRotation(const DirectX::XMFLOAT3 rot) { m_rotation = rot; }

	AABB Player::GetHitbox() const { return m_hitbox; }
	void Player::SetHitbox(const AABB& hitbox) { m_hitbox = hitbox; }

	CameraType Player::GetSelectedCameraType() const { return m_selectedCameraType; }
	void Player::SetSelectedCameraType(const CameraType cameraType) { m_selectedCameraType = cameraType; }

	void Player::Update(const float& dt)
	{
		// Change the selected camera type
		if (Input::IsKeyDown(KeyCode::G))
		{
			m_selectedCameraType = CameraType::FirstPerson;
		}
		else if (Input::IsKeyDown(KeyCode::H))
		{
			m_selectedCameraType = CameraType::Debug;
		}

		// Update movement through controller
		PlayerController::Update(dt, this);
	}
}
