#include "../Misc/pch.h"

#include "Camera.h"
#include "../Utility/Math.h"

using namespace DirectX;

Camera::Camera()
{
	m_viewMatrix = XMMatrixIdentity();
	m_smoothingFactor = 1.0f;
	m_rotationSpeed = 1.0f;
	m_position = { 0.0f, 0.0f, 0.0f };
	m_currentRotation = { 0.0f, 0.0f, 0.0f };
	m_targetRotation = { 0.0f, 0.0f, 0.0f };
}

Camera::Camera(const float rotationSpeed, const float smoothingFactor) : Camera()
{
	m_smoothingFactor = smoothingFactor;
	m_rotationSpeed = rotationSpeed;
}

void Camera::Update(float dt)
{
	// The rotation wrap threshold in degrees
	const float rotationWrapThreshold = 300.0f;
	XMFLOAT3 finalRotationValues = m_targetRotation;

	// For the X and Y axis
	for (uint32_t i = 0; i < 2; i++)
	{
		float deltaAxisRotation = XMFLOAT3_BRACKET_OP_32(m_targetRotation, i) - XMFLOAT3_BRACKET_OP_32(m_currentRotation, i);
		// check if we have wrapped around, so "unwrap" before lerping
		if (abs(deltaAxisRotation) > rotationWrapThreshold)
		{
			// negative number means we went from 360 to 0
			int32_t sign = Orange::Math::Sign(deltaAxisRotation);
			if (sign == -1)
			{
				XMFLOAT3_BRACKET_OP_32(finalRotationValues, i) = 360.0f + XMFLOAT3_BRACKET_OP_32(m_targetRotation, i);
			}
			// positive number means we went from 0 to 360
			else if (sign == 1)
			{
				XMFLOAT3_BRACKET_OP_32(finalRotationValues, i) = XMFLOAT3_BRACKET_OP_32(m_targetRotation, i) - 360.0f;
			}
		}
	}

	for (uint32_t i = 0; i < 2; i++)
	{
		float ratio = m_smoothingFactor * dt;
		// TODO: map the ratio to an exponential function mirrored along x = y, so we can get faster camera movement
		// in the beginning, slow in the middle and fast in the end. Maybe use splines for this?
		Orange::Math::Clamp(ratio, 0.0f, 1.0f);
		XMFLOAT3_BRACKET_OP_32(m_currentRotation, i) = Orange::Math::Lerp(XMFLOAT3_BRACKET_OP_32(m_currentRotation, i), XMFLOAT3_BRACKET_OP_32(finalRotationValues, i), ratio);
	}
	Orange::Math::Wrap(m_currentRotation.y, 0.0f, 360.0f);

	ConstructMatrix(m_position, m_currentRotation);
}

void Camera::ConstructMatrix(const XMFLOAT3& pos, const XMFLOAT3& rot)
{

	// Setup the vector that points upwards.
	XMVECTOR up = { 0, 1.0f, 0 };

	// Setup the position of the camera in the world.
	XMVECTOR position = { pos.x, pos.y, pos.z };

	// Setup where the camera is looking by default.
	XMVECTOR lookAt = { 0, 0, 1.0f };

	// Create the rotation matrix from the yaw, pitch, and roll values.
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(Orange::Math::DegreesToRadians(rot.x), Orange::Math::DegreesToRadians(rot.y), Orange::Math::DegreesToRadians(rot.z));

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = XMVector3Transform(lookAt, rotationMatrix);
	up = XMVector3Transform(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt += position;

	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = XMMatrixLookAtLH(position, lookAt, up);

	return;
}

DirectX::XMMATRIX Camera::GetViewMatrix() { return m_viewMatrix; }

DirectX::XMMATRIX Camera::GetWorldMatrix() { return XMMatrixInverse(nullptr, m_viewMatrix); }

void Camera::SetViewMatrix(const DirectX::XMMATRIX viewMatrix) { m_viewMatrix = viewMatrix; }
void Camera::SetWorldMatrix(const DirectX::XMMATRIX worldMatrix) { m_viewMatrix = XMMatrixInverse(nullptr, worldMatrix); }

const float Camera::GetRotationSpeed() { return m_rotationSpeed; }
void Camera::SetRotationSpeed(const float speed) { m_rotationSpeed = speed; }

void Camera::SetCameraParameters(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation)
{
	m_position = position;
	m_targetRotation = rotation;
}