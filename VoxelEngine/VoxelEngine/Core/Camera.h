#ifndef _CAMERA_H
#define _CAMERA_H

#include <DirectXMath.h>

// Defines a camera base class with all the basic functionality
class Camera
{
public:

	Camera();
	~Camera() = default;
	Camera(const Camera& camera) = default;

	void ConstructMatrix(const DirectX::XMFLOAT3 pos);

	DirectX::XMFLOAT3 GetRotation();
	void SetRotation(const DirectX::XMFLOAT3 rot);

	DirectX::XMMATRIX GetViewMatrix();
	DirectX::XMMATRIX GetWorldMatrix();

	void SetViewMatrix(const DirectX::XMMATRIX viewMatrix);
	void SetWorldMatrix(const DirectX::XMMATRIX worldMatrix);

	virtual void Update(float deltaTime) = 0;

protected:

	DirectX::XMFLOAT3 m_rotation; // Stores pitch, yaw and roll

	DirectX::XMMATRIX m_viewMatrix;
};


#endif
