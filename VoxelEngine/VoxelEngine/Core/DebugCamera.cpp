#include "../Misc/pch.h"
#include "DebugCamera.h"

#include "../Utility/Input.h"

#include "Events/KeyCodes.h"

using namespace DirectX;

DebugCamera::DebugCamera() : 
	m_movementSpeed(3.0f), m_rotationSpeed(3.0f), Camera()
{}

void DebugCamera::Update(float dt)
{
	XMFLOAT3 prevPos = m_position;
	XMVECTOR deltaTranslation = { 0, 0, 0, 1 };
	XMVECTOR deltaRotation = XMQuaternionIdentity();


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
		float xAmount = Input::GetMouseDeltaX() * dt * m_rotationSpeed;
		float yAmount = Input::GetMouseDeltaY() * dt * m_rotationSpeed;
		if(xAmount > 0 && yAmount > 0)
		{
			int test = 0;
		}

		// Rotation around the X axis (look up/down)
		XMVECTOR xRot = XMQuaternionRotationNormal({ 1, 0, 0, 0 }, Input::GetMouseDeltaY() * dt * m_rotationSpeed);
		// Rotation around the Y axis (look left/right)
		XMVECTOR yRot = XMQuaternionRotationNormal({ 0, 1, 0, 0 }, Input::GetMouseDeltaX() * dt * m_rotationSpeed);

		// In order to avoid camera roll, multiply quaternions in this order
		// upDownQuat * currentRotation
		// currentRotation * leftRightQuat
		deltaRotation = XMQuaternionMultiply(deltaRotation, xRot);
		deltaRotation = XMQuaternionMultiply(yRot, deltaRotation);

	}


	// Build the rotation and translation matrices
	XMMATRIX rotMatrix = XMMatrixRotationQuaternion(deltaRotation);
	XMMATRIX translationMatrix = XMMatrixTranslation
	(
		deltaTranslation.m128_f32[0], deltaTranslation.m128_f32[1], deltaTranslation.m128_f32[2]
	);

	// Calculate the new world matrix
	m_worldMatrix.r[3] = { 0, 0, 0, 1.0f };
	m_worldMatrix = m_worldMatrix * rotMatrix;
	m_worldMatrix.r[3] = { prevPos.x, prevPos.y, prevPos.z, 1.0f };
	m_worldMatrix = translationMatrix * m_worldMatrix;

	// Decompose the matrix to obtain the original variables
	XMVECTOR rot;
	XMVECTOR pos;
	XMVECTOR scale; // NOT USED!
	XMMatrixDecompose(&scale, &rot, &pos, m_worldMatrix);
	XMStoreFloat3(&m_position, pos);
	XMStoreFloat4(&m_rotation, rot);

}
