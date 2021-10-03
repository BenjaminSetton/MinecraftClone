#ifndef _DEBUG_CAMERA_H
#define _DEBUG_CAMERA_H

#include "Camera.h"

class DebugCamera : public Camera
{
public:

	DebugCamera();
	DebugCamera(const DebugCamera& other) = default;
	~DebugCamera() = default;

	void Update(const float dt) override;

	const float GetRotationSpeed();
	void SetRotationSpeed(const float speed);

private:

	float m_rotationSpeed;

};


#endif
