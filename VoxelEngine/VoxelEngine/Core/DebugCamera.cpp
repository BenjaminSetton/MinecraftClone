#include "../Misc/pch.h"
#include "DebugCamera.h"

#include "../Utility/Input.h"

#include "Events/KeyCodes.h"

using namespace DirectX;

DebugCamera::DebugCamera() : 
	m_movementSpeed(7.0f), m_rotationSpeed(5.0f), Camera()
{}

void DebugCamera::Update(float dt)
{
	XMFLOAT3 prevPos = m_position;
	XMVECTOR deltaTranslation = { 0, 0, 0, 1 };
	XMVECTOR deltaRotation = { 0.0f, 0.0f, 0.0f };

	XMVECTOR prevX, prevY, prevZ;
	prevX = m_worldMatrix.r[0];
	prevY = m_worldMatrix.r[1];
	prevZ = m_worldMatrix.r[2];

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

	// NOTE!
	//
	//	THERE IS AN ISSUE WITH WINDOW RESIZING/FRAMERATE WHERE
	//	CAMREA ROTATION SPEEDS UP OR SLOWS DOWN
	//
	if(Input::IsMouseDown(MouseCode::LBUTTON))
	{
		// Rotation around the X axis (look up/down)
		deltaRotation.m128_f32[0] = Input::GetMouseDeltaY() * m_rotationSpeed * 0.001f;
		// Rotation around the Y axis (look left/right)
		deltaRotation.m128_f32[1] = Input::GetMouseDeltaX() * m_rotationSpeed * 0.001f;
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


	// TODO: Orthonormalize the camera world matrix in order to prevent it from flipping

	// Get the Z axis
	float epsilon = 0.0025f;
	XMVECTOR zAxis = m_worldMatrix.r[2];
	XMVECTOR upVec = { 0.0f, 1.0f, 0.0f, 0.0f };


	if(abs(zAxis.m128_f32[1]) + epsilon < 1.0f)
	{
		// Calculate the X (right) axis
		m_worldMatrix.r[0] = XMVector3Normalize(XMVector3Cross(upVec, zAxis));
		// Calculate the Y (up) axis
		m_worldMatrix.r[1] = XMVector3Normalize(XMVector3Cross(zAxis, m_worldMatrix.r[0]));
	}
	else
	{
		m_worldMatrix.r[0] = prevX;
		m_worldMatrix.r[1] = prevY;
		m_worldMatrix.r[2] = prevZ;
	}


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
