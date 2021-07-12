#ifndef _CAMERA_H
#define _CAMERA_H

#include <DirectXMath.h>

class Camera
{
public:

	Camera();
	~Camera();
	Camera(const Camera& camera);

	void ConstructMatrix();

	DirectX::XMFLOAT3 GetPosition();
	void SetPosition(const DirectX::XMFLOAT3 pos);

	DirectX::XMFLOAT3 GetRotation();
	void SetRotation(const DirectX::XMFLOAT3 rot);

	DirectX::XMMATRIX GetViewMatrix();

private:

	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_rotation;
	DirectX::XMMATRIX m_viewMatrix;
};


#endif
