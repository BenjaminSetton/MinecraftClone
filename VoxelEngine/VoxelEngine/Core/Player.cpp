#include "../Misc/pch.h"
#include "Player.h"

#include "../Utility/Input.h"
#include "Events/KeyCodes.h"
#include "Physics.h"

using namespace DirectX;

Player::Player() : m_acceleration({0.0f, 0.0f, 0.0f}), m_velocity({0.0f, 0.0f, 0.0f}), m_camera(nullptr),
					m_movementSpeed(7.0f), m_position({ -4.0f, 30.0f, -10.0f })
{
	m_camera = new DebugCamera;
	m_camera->ConstructMatrix(m_position);
}

Player::~Player()
{
	delete m_camera;
}

DebugCamera* Player::GetCamera() { return m_camera; }
void Player::SetCamera(DebugCamera* camera) { m_camera = camera; }

DirectX::XMFLOAT3 Player::GetAcceleration() const { return m_acceleration; }
void Player::SetAcceleration(const DirectX::XMFLOAT3 accel) { m_acceleration = accel; }

DirectX::XMFLOAT3 Player::GetVelocity() const { return m_velocity; }
void Player::SetVelocity(const DirectX::XMFLOAT3 vel) { m_velocity = vel; }

DirectX::XMFLOAT3 Player::GetPosition() const { return m_position; }
void Player::SetPosition(const DirectX::XMFLOAT3 pos) { m_position = pos; }

void Player::Update(const float& dt)
{
	// Gather input

	XMFLOAT3 prevPos = m_position;
	XMVECTOR prevPlayerFeetPos = { prevPos.x, prevPos.y - 2.0f, prevPos.z };
	XMVECTOR deltaTranslation = { 0, 0, 0, 1 };
	XMVECTOR deltaRotation = { 0.0f, 0.0f, 0.0f };

	XMMATRIX worldMatrix = m_camera->GetWorldMatrix();


	XMVECTOR prevX, prevY, prevZ;
	prevX = worldMatrix.r[0];
	prevY = worldMatrix.r[1];
	prevZ = worldMatrix.r[2];

	///
	//
	// Update position
	//
	///

	if (Input::IsKeyDown(KeyCode::O) || Input::IsKeyDown(KeyCode::W)) // FORWARD
		deltaTranslation.m128_f32[2] += m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::L) || Input::IsKeyDown(KeyCode::S)) // BACKWARD
		deltaTranslation.m128_f32[2] -= m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::K) || Input::IsKeyDown(KeyCode::A)) // LEFT
		deltaTranslation.m128_f32[0] -= m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::SEMICOLON) || Input::IsKeyDown(KeyCode::D)) // RIGHT
		deltaTranslation.m128_f32[0] += m_movementSpeed * dt;
	

	if(!Physics::DetectCollision(prevPlayerFeetPos))
	{
		// Only apply gravity if we are not colliding with anything
		XMVECTOR vel = XMLoadFloat3(&m_velocity);
		Physics::ApplyGravity(vel, dt);
		Physics::ApplyVelocity(deltaTranslation, vel, dt);
		XMStoreFloat3(&m_velocity, vel);
	}

	// Update rotation only if LMB is held down

	// NOTE!
	//
	//	THERE IS AN ISSUE WITH WINDOW RESIZING/FRAMERATE WHERE
	//	CAMREA ROTATION SPEEDS UP OR SLOWS DOWN
	//
	if (Input::IsMouseDown(MouseCode::LBUTTON))
	{
		// Rotation around the X axis (look up/down)
		deltaRotation.m128_f32[0] = Input::GetMouseDeltaY() * m_camera->GetRotationSpeed() * 0.001f;
		// Rotation around the Y axis (look left/right)
		deltaRotation.m128_f32[1] = Input::GetMouseDeltaX() * m_camera->GetRotationSpeed() * 0.001f;
	}


	// Build the rotation and translation matrices
	XMMATRIX rotXMatrix = XMMatrixRotationX(deltaRotation.m128_f32[0]);
	XMMATRIX rotYMatrix = XMMatrixRotationY(deltaRotation.m128_f32[1]);
	XMMATRIX translationMatrixXZ = XMMatrixTranslation
	(
		deltaTranslation.m128_f32[0], 0, deltaTranslation.m128_f32[2]
	);

	// Calculate the new world matrix
	worldMatrix.r[3] = { 0, 0, 0, 1.0f };
	worldMatrix = worldMatrix * rotYMatrix;
	worldMatrix = rotXMatrix * worldMatrix;
	worldMatrix.r[3] = { prevPos.x, prevPos.y, prevPos.z, 1.0f };

	// Apply translation
	worldMatrix = translationMatrixXZ * worldMatrix; // Local XZ translation
	// NOTE!
	// Y offset due to matrix multiplication is reverted, and deltaTranslation
	// is added on top of it
	worldMatrix.r[3].m128_f32[1] = prevPos.y + deltaTranslation.m128_f32[1]; // Global Y translation


	// Get the Z axis
	float epsilon = 0.0025f;
	XMVECTOR zAxis = worldMatrix.r[2];

	XMVECTOR upVec = { 0.0f, 1.0f, 0.0f, 0.0f };


	if (abs(zAxis.m128_f32[1]) + epsilon < 1.0f)
	{
		// Calculate the X (right) axis
		worldMatrix.r[0] = XMVector3Normalize(XMVector3Cross(upVec, zAxis));
		// Calculate the Y (up) axis
		worldMatrix.r[1] = XMVector3Normalize(XMVector3Cross(zAxis, worldMatrix.r[0]));
	}
	else
	{
		worldMatrix.r[0] = prevX;
		worldMatrix.r[1] = prevY;
		worldMatrix.r[2] = prevZ;
	}


	// Update the position and rotation camera values
	m_position =
	{
		worldMatrix.r[3].m128_f32[0],
		worldMatrix.r[3].m128_f32[1],
		worldMatrix.r[3].m128_f32[2]
	};

	XMFLOAT3 cameraRot = m_camera->GetRotation();
	m_camera->SetRotation(
	{
		cameraRot.x + deltaRotation.m128_f32[0],
		cameraRot.x + deltaRotation.m128_f32[1],
		cameraRot.x + deltaRotation.m128_f32[2]
	});


	// We have "moved into collision", so don't apply translation
	XMVECTOR newPlayerFeetPos = { worldMatrix.r[3].m128_f32[0], worldMatrix.r[3].m128_f32[1] - 2.0f, worldMatrix.r[3].m128_f32[2] };
	bool prevPosCollision = Physics::DetectCollision(prevPlayerFeetPos);
	bool newPosCollision = Physics::DetectCollision(newPlayerFeetPos);
	if (newPosCollision)
	{
		worldMatrix.r[3] = XMLoadFloat3(&prevPos);

		// Set the W component of the position to 1
		worldMatrix.r[3].m128_f32[3] = 1.0f;

		// Reset acceleration and velocity
		m_acceleration = m_velocity = { 0.0f, 0.0f, 0.0f };
	}


	// Re-assign the view matrix with all the changes
	m_camera->SetWorldMatrix(worldMatrix);
}