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
	XMFLOAT3 prevPos = m_position;
	XMVECTOR deltaTranslation = { 0, 0, 0, 1 };
	XMVECTOR deltaRotation = { 0.0f, 0.0f, 0.0f };


	//Gather input from Input class

	// Update position
	if (Input::IsKeyDown(KeyCode::O) || Input::IsKeyDown(KeyCode::W)) // FORWARD
		deltaTranslation.m128_f32[2] += m_movementSpeed * dt;
	if(Input::IsKeyDown(KeyCode::L) || Input::IsKeyDown(KeyCode::S)) // BACKWARD
		deltaTranslation.m128_f32[2] -= m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::K) || Input::IsKeyDown(KeyCode::A)) // LEFT
		deltaTranslation.m128_f32[0] -= m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::SEMICOLON) || Input::IsKeyDown(KeyCode::D)) // RIGHT
		deltaTranslation.m128_f32[0] += m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::SPACE)) // UP
		deltaTranslation.m128_f32[1] += m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::RSHIFT) || Input::IsKeyDown(KeyCode::LSHIFT)) // DOWN
		deltaTranslation.m128_f32[1] -= m_movementSpeed * dt;

	// Update rotation only if LMB is held down
	if(Input::IsMouseDown(MouseCode::LBUTTON))
	{
		// Rotation around the X axis (look up/down)
		deltaRotation.m128_f32[0] = Input::GetMouseDeltaY() * dt * m_rotationSpeed;
		// Rotation around the Y axis (look left/right)
		deltaRotation.m128_f32[1] = Input::GetMouseDeltaX() * dt * m_rotationSpeed;
	}


	// Build the rotation and translation matrices
	XMMATRIX rotXMatrix = XMMatrixRotationX(deltaRotation.m128_f32[0]);
	XMMATRIX rotYMatrix = XMMatrixRotationY(deltaRotation.m128_f32[1]);
	XMMATRIX translationMatrix = XMMatrixTranslation
	(
		deltaTranslation.m128_f32[0], deltaTranslation.m128_f32[1], deltaTranslation.m128_f32[2]
	);

	// Calculate the new world matrix
	m_worldMatrix.r[3] = { 0, 0, 0, 1.0f };
	m_worldMatrix = m_worldMatrix * rotYMatrix;
	m_worldMatrix = rotXMatrix * m_worldMatrix;

	m_worldMatrix.r[3] = { prevPos.x, prevPos.y, prevPos.z, 1.0f };
	m_worldMatrix = translationMatrix * m_worldMatrix;

	// Update the position and rotation camera values
	m_position = 
	{
		m_worldMatrix.r[3].m128_f32[0],
		m_worldMatrix.r[3].m128_f32[1],
		m_worldMatrix.r[3].m128_f32[2]
	};

	m_rotation =
	{
		m_rotation.x + deltaRotation.m128_f32[0],
		m_rotation.x + deltaRotation.m128_f32[1],
		m_rotation.x + deltaRotation.m128_f32[2]
	};

}
