#ifndef _CAMERA_H
#define _CAMERA_H

#include <DirectXMath.h>

// Defines a camera base class with all the basic functionality
class Camera
{
public:

	Camera();
	Camera(const float rotationSpeed, const float smoothingFactor);
	~Camera() = default;
	Camera(const Camera& camera) = default;

	void Update(float dt);

	void ConstructMatrix(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot);

	DirectX::XMMATRIX GetViewMatrix();
	DirectX::XMMATRIX GetWorldMatrix();

	void SetViewMatrix(const DirectX::XMMATRIX viewMatrix);
	void SetWorldMatrix(const DirectX::XMMATRIX worldMatrix);

	const float GetRotationSpeed();
	void SetRotationSpeed(const float speed);

	void SetCameraParameters(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& rotation);

private:

	DirectX::XMMATRIX m_viewMatrix;

	float m_smoothingFactor;
	float m_rotationSpeed;
	DirectX::XMFLOAT3 m_currentRotation;

	// These are temporary values that should be set by the player controller
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_targetRotation;
};


#endif
