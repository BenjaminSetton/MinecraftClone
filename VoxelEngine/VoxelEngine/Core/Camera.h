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

	void ConstructMatrix();

	DirectX::XMFLOAT3 GetPosition();
	void SetPosition(const DirectX::XMFLOAT3 pos);

	DirectX::XMFLOAT3 GetRotation();
	void SetRotation(const DirectX::XMFLOAT3 rot);

	// Inverts the camera's world matrix to create a view matrix
	DirectX::XMMATRIX GetViewMatrix();

	virtual void Update(float deltaTime) = 0;

protected:

	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotation; // Stores pitch, yaw and roll

	DirectX::XMMATRIX m_worldMatrix;
};


#endif
