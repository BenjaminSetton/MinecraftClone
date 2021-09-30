#include "../Misc/pch.h"
#include "FrustumCamera.h"

#include "../Utility/Input.h"

#include "Events/KeyCodes.h"

using namespace DirectX;

FrustumCamera::FrustumCamera() :
	m_movementSpeed(4.0f), m_rotationSpeed(5.0f), Camera()
{}

void FrustumCamera::Update(float dt)
{
	XMFLOAT3 prevPos = m_position;
	XMVECTOR deltaTranslation = { 0, 0, 0, 1 };
	XMVECTOR deltaRotation = { 0.0f, 0.0f, 0.0f };

	XMMATRIX worldMatrix = GetWorldMatrix();

	XMVECTOR prevX, prevY, prevZ;
	prevX = worldMatrix.r[0];
	prevY = worldMatrix.r[1];
	prevZ = worldMatrix.r[2];

	//Gather input from Input class

	// Update position
	if (Input::IsKeyDown(KeyCode::UP)) // FORWARD
		deltaTranslation.m128_f32[2] += m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::DOWN)) // BACKWARD
		deltaTranslation.m128_f32[2] -= m_movementSpeed * dt;


	// NOTE!
	//
	//	THERE IS AN ISSUE WITH WINDOW RESIZING/FRAMERATE WHERE
	//	CAMREA ROTATION SPEEDS UP OR SLOWS DOWN
	//	AND IS SUPER LAGGY
	//
	// Rotation around the Y axis (look left/right)
	if (Input::IsKeyDown(KeyCode::LEFT)) // LEFT
		deltaRotation.m128_f32[1] += m_rotationSpeed * 0.001f;
	if (Input::IsKeyDown(KeyCode::RIGHT)) // RIGHT
		deltaRotation.m128_f32[1] -= m_rotationSpeed * 0.001f;


	// Build the rotation and translation matrices
	XMMATRIX rotYMatrix = XMMatrixRotationY(deltaRotation.m128_f32[1]);
	XMMATRIX translationMatrix = XMMatrixTranslation
	(
		deltaTranslation.m128_f32[0], deltaTranslation.m128_f32[1], deltaTranslation.m128_f32[2]
	);

	// Calculate the new world matrix
	worldMatrix.r[3] = { 0, 0, 0, 1.0f };
	worldMatrix = worldMatrix * rotYMatrix;

	worldMatrix.r[3] = { prevPos.x, prevPos.y, prevPos.z, 1.0f };
	worldMatrix = translationMatrix * worldMatrix;

	// Update the position and rotation camera values
	m_position =
	{
		worldMatrix.r[3].m128_f32[0],
		worldMatrix.r[3].m128_f32[1],
		worldMatrix.r[3].m128_f32[2]
	};

	m_rotation =
	{
		m_rotation.x + deltaRotation.m128_f32[0],
		m_rotation.x + deltaRotation.m128_f32[1],
		m_rotation.x + deltaRotation.m128_f32[2]
	};

	// Re-assign the view matrix with all the changes
	m_viewMatrix = XMMatrixInverse(nullptr, worldMatrix);
}
