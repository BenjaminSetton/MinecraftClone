#include "../Misc/pch.h"
#include "Player.h"

#include "../Utility/Input.h"
#include "Events/KeyCodes.h"
#include "Physics.h"

#include "../Utility/ImGuiLayer.h"

using namespace DirectX;

Player::Player() : m_acceleration({0.0f, 0.0f, 0.0f}), m_velocity({0.0f, 0.0f, 0.0f}), m_camera(nullptr),
					m_movementSpeed(7.0f), m_position({ -4.0f, 25.0f, -10.0f }), m_allowJump(false)
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

	m_allowJump = false;

	if (Input::IsKeyDown(KeyCode::O) || Input::IsKeyDown(KeyCode::W)) // FORWARD
		deltaTranslation.m128_f32[2] += m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::L) || Input::IsKeyDown(KeyCode::S)) // BACKWARD
		deltaTranslation.m128_f32[2] -= m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::K) || Input::IsKeyDown(KeyCode::A)) // LEFT
		deltaTranslation.m128_f32[0] -= m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::SEMICOLON) || Input::IsKeyDown(KeyCode::D)) // RIGHT
		deltaTranslation.m128_f32[0] += m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::SPACE)) // SPACE
	{
		m_acceleration.y = (1.0f);
		m_allowJump = true;
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

	// gather input
	// apply gravity
	// apply accel then vel
	// check is we moved into collision


	// We have "moved into collision", so don't apply translation
	XMVECTOR newPlayerFeetPos = { worldMatrix.r[3].m128_f32[0], worldMatrix.r[3].m128_f32[1] - 2.0f, worldMatrix.r[3].m128_f32[2] };
	XMFLOAT3 newPos = { worldMatrix.r[3].m128_f32[0], worldMatrix.r[3].m128_f32[1], worldMatrix.r[3].m128_f32[2] };
	bool newPosCollision = Physics::DetectCollision(newPlayerFeetPos);
	if (newPosCollision)
	{
		XMFLOAT3 velocityToAdd = {newPos.x - prevPos.x, newPos.y - prevPos.y, newPos.z - prevPos.z};

		// Test every component of velocity to check which component to nullify
		// TEST X COMPONENT
		{
			XMVECTOR testXComp = prevPlayerFeetPos;
			testXComp.m128_f32[0] += velocityToAdd.x;
			if (Physics::DetectCollision(testXComp)) velocityToAdd.x = 0;
		}
		// TEST Y COMPONENT (IF 0, SKIP?)
		{
			XMVECTOR testYComp = prevPlayerFeetPos;
			testYComp.m128_f32[1] += velocityToAdd.y;
			if (Physics::DetectCollision(testYComp)) velocityToAdd.y = 0;
		}
		// TEST Z COMPONENT
		{
			XMVECTOR testZComp = prevPlayerFeetPos;
			testZComp.m128_f32[2] += velocityToAdd.z;
			if (Physics::DetectCollision(testZComp)) velocityToAdd.z = 0;
		}

		worldMatrix.r[3] = { prevPos.x + velocityToAdd.x, prevPos.y + velocityToAdd.y, prevPos.z + velocityToAdd.z };

		// Set the W component of the position to 1
		worldMatrix.r[3].m128_f32[3] = 1.0f;

		// Reset acceleration, velocity and position
		XMStoreFloat3(&m_position, worldMatrix.r[3]);
	}

	// Store the position after allowing player movement
	XMVECTOR playerPosAfterMovement = { worldMatrix.r[3].m128_f32[0], worldMatrix.r[3].m128_f32[1], worldMatrix.r[3].m128_f32[2] };


	XMVECTOR accel = XMLoadFloat3(&m_acceleration);
	XMVECTOR vel = XMLoadFloat3(&m_velocity);

	// Apply gravity
	Physics::ApplyGravity(vel, dt);
	accel.m128_f32[1] += -9.8f * dt;

	// Apply acceleration and velocity to position vector
	Physics::ApplyAcceleration(vel, accel, dt);
	Physics::ApplyVelocity(worldMatrix.r[3], vel, dt);

	XMStoreFloat3(&m_velocity, vel);
	XMStoreFloat3(&m_acceleration, accel);


	XMVECTOR playerFeetPosAfterGravity = { worldMatrix.r[3].m128_f32[0], worldMatrix.r[3].m128_f32[1] - 2.0f, worldMatrix.r[3].m128_f32[2] };
	// If collision is detected after applying gravity, revert
	if(Physics::DetectCollision(playerFeetPosAfterGravity))
	{
		if(!m_allowJump)
		{
			worldMatrix.r[3] = playerPosAfterMovement;
			// Set the W component of the position to 1
			worldMatrix.r[3].m128_f32[3] = 1.0f;

			// Reset acceleration and velocity
			m_acceleration = m_velocity = { 0.0f, 0.0f, 0.0f };
		}
	}

	// Update the position and rotation camera values
	XMStoreFloat3(&m_position, worldMatrix.r[3]);

	XMFLOAT3 cameraRot = m_camera->GetRotation();
	m_camera->SetRotation(
		{
			cameraRot.x + deltaRotation.m128_f32[0],
			cameraRot.x + deltaRotation.m128_f32[1],
			cameraRot.x + deltaRotation.m128_f32[2]
		});

	// DEBUG SETTINGS
	PlayerPhysics_Data::accel = m_acceleration;
	PlayerPhysics_Data::vel = m_velocity;

	// Re-assign the view matrix with all the changes
	m_camera->SetWorldMatrix(worldMatrix);
}