#ifndef _FRUSTUM_CAMERA_H
#define _FRUSTUM_CAMERA_H

#include "Camera.h"

class FrustumCamera : public Camera
{
public:

	FrustumCamera();
	FrustumCamera(const FrustumCamera& other) = default;
	~FrustumCamera() = default;

	virtual void Update(float deltaTime) override;
	
	DirectX::XMMATRIX GetWorldMatrix();

private:

	float m_movementSpeed;
	float m_rotationSpeed;

};


#endif
