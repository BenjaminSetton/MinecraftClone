#include "../Misc/pch.h"
#include "DebugCamera.h"

#include "../Utility/Input.h"

#include "Events/KeyCodes.h"

using namespace DirectX;

DebugCamera::DebugCamera() : 
	m_movementSpeed(3.0f), m_rotationSpeed(1.0f), Camera()
{}

void DebugCamera::Update(float dt)
{
	/*
		FXMVECTOR ScalingOrigin,
		FXMVECTOR ScalingOrientationQuaternion,
		FXMVECTOR Scaling,
		GXMVECTOR RotationOrigin,
		HXMVECTOR RotationQuaternion,
		HXMVECTOR Translation
	*/

	//Gather input from Input class

	// Update position
	if (Input::IsKeyDown(KeyCode::O) || Input::IsKeyDown(KeyCode::W)) // FORWARD
		m_position.z += m_movementSpeed * dt;
	if(Input::IsKeyDown(KeyCode::L) || Input::IsKeyDown(KeyCode::S)) // BACKWARD
		m_position.z -= m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::K) || Input::IsKeyDown(KeyCode::A)) // LEFT
		m_position.x -= m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::SEMICOLON) || Input::IsKeyDown(KeyCode::D)) // RIGHT
		m_position.x += m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::SPACE)) // UP
		m_position.y += m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::RSHIFT) || Input::IsKeyDown(KeyCode::LSHIFT)) // DOWN
		m_position.y -= m_movementSpeed * dt;

	// Update rotation only if LMB is held down
	if(Input::IsMouseDown(MouseCode::LBUTTON))
	{
		m_rotation.y += Input::GetMouseDeltaX() * dt * m_rotationSpeed;
		m_rotation.x += Input::GetMouseDeltaY() * dt * m_rotationSpeed;
	}


	// Build the rotation and translation matrices
	m_viewMatrix = XMMatrixInverse(nullptr, m_viewMatrix);
	XMMATRIX rotMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, 0);
	XMMATRIX translationMatrix = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	m_viewMatrix = rotMatrix * m_viewMatrix * translationMatrix;
	//m_viewMatrix = translationMatrix * m_viewMatrix;
	m_viewMatrix = XMMatrixInverse(nullptr, m_viewMatrix);

}
