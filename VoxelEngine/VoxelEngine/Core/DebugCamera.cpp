#include "../Misc/pch.h"
#include "DebugCamera.h"

#include "../Utility/Input.h"

#include "Events/KeyCodes.h"

DebugCamera::DebugCamera() : m_movementSpeed(1.0f), m_rotationSpeed(1.0f)
{}

void DebugCamera::Update(float deltaTime)
{
	//Gather input from Input class
	
	// Update position
	if(Input::IsKeyDown(KeyCode::W))

	// Update position and rotation

	// Update the matrix
}
