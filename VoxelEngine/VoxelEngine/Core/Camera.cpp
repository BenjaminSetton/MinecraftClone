#include "../Misc/pch.h"

#include "Camera.h"

// A #def for converting degrees to radians
#define DEGREES_TO_RADIANS 0.0174532925f

using namespace DirectX;

Camera::Camera()
{
	m_position = { 0.0f, 0.0f, 0.0f };
	m_rotation = { 0.0f, 0.0f, 0.0f };
	m_worldMatrix = XMMatrixIdentity();
}

void Camera::ConstructMatrix()
{

	// Setup the vector that points upwards.
	XMVECTOR up = { 0, 1.0f, 0 };

	// Setup the position of the camera in the world.
	XMVECTOR position = { m_position.x, m_position.y, m_position.z };

	// Setup where the camera is looking by default.
	XMVECTOR lookAt = { 0, 0, 1.0f };

	// Create the rotation matrix from the yaw, pitch, and roll values.
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.	
	lookAt = XMVector3Transform(lookAt, rotationMatrix);
	up = XMVector3Transform(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt += position;

	// Finally create the view matrix from the three updated vectors.
	m_worldMatrix = XMMatrixLookAtLH(position, lookAt, up);

	return;
}

DirectX::XMFLOAT3 Camera::GetPosition() { return m_position; }
void Camera::SetPosition(const DirectX::XMFLOAT3 pos) { m_position = pos; }

DirectX::XMFLOAT3 Camera::GetRotation() { return m_rotation; }
void Camera::SetRotation(const DirectX::XMFLOAT3 rot) { m_rotation = rot; }

DirectX::XMMATRIX Camera::GetViewMatrix() { return XMMatrixInverse(nullptr, m_worldMatrix); }