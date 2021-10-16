#include "../Misc/pch.h"
#include "Player.h"

#include "PlayerController.h"

using namespace DirectX;

Player::Player() : m_acceleration({0.0f, 0.0f, 0.0f}), m_velocity({0.0f, 0.0f, 0.0f}), m_camera(nullptr),
					m_movementSpeed(7.0f), m_position({ -4.0f, 150.0f, -10.0f }), m_allowJump(false)
{
	m_camera = new DebugCamera;
	m_camera->ConstructMatrix(m_position);
}

Player::~Player()
{
	delete m_camera;
}

DebugCamera* Player::GetCamera() { return m_camera; }
void Player::SetCamera(DebugCamera* camera) { m_camera = camera; }

DirectX::XMFLOAT3 Player::GetAcceleration() const { return m_acceleration; }
void Player::SetAcceleration(const DirectX::XMFLOAT3 accel) { m_acceleration = accel; }

DirectX::XMFLOAT3 Player::GetVelocity() const { return m_velocity; }
void Player::SetVelocity(const DirectX::XMFLOAT3 vel) { m_velocity = vel; }

DirectX::XMFLOAT3 Player::GetPosition() const { return m_position; }
void Player::SetPosition(const DirectX::XMFLOAT3 pos) { m_position = pos; }

void Player::Update(const float& dt)
{
	// Update movement through controller
	PlayerController::Update(dt, this);
}