#ifndef _FRUSTUM_CAMERA_H
#define _FRUSTUM_CAMERA_H

#include "Camera.h"

class FrustumCamera : public Camera
{
public:

	FrustumCamera();
	FrustumCamera(const FrustumCamera& other) = default;
	~FrustumCamera() = default;

	void Update(float deltaTime);

private:

	float m_movementSpeed;
	float m_rotationSpeed;

};


#endif
