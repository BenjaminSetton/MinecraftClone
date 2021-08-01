#ifndef _DEBUG_CAMERA_H
#define _DEBUG_CAMERA_H

#include "Camera.h"

class DebugCamera : public Camera
{
public:

	DebugCamera();
	DebugCamera(const DebugCamera& other) = default;
	~DebugCamera() = default;

	virtual void Update(float deltaTime) override;

private:

	float m_movementSpeed;
	float m_rotationSpeed;

};


#endif
