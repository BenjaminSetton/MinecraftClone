#include "../Misc/pch.h"
#include "DebugCamera.h"

using namespace DirectX;

DebugCamera::DebugCamera() : 
	m_rotationSpeed(5.0f), Camera()
{}

void DebugCamera::Update(const float dt){}

const float DebugCamera::GetRotationSpeed() { return m_rotationSpeed; }
void DebugCamera::SetRotationSpeed(const float speed) { m_rotationSpeed = speed; }
