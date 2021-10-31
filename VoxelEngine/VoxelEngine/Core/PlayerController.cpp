#include "../Misc/pch.h"
#include "PlayerController.h"

#include "Player.h"

#include "../Utility/Input.h"
#include "Events/KeyCodes.h"
#include "Physics.h"
#include <DirectXMath.h>

// DEBUG
#include "../Utility/ImGuiLayer.h"

using namespace DirectX;


// The math for the player jump mechanic was taken from
// this GDC talk: https://www.youtube.com/watch?v=hG9SzQxaCm8
// Full credit to author Kyle Pittman

constexpr float JUMP_HEIGHT = 1.1f;												// in meters
constexpr float JUMP_TIME = 0.32f;												// in seconds

// Defined to be NEGATIVE by convention
constexpr float GRAVITY = (-2.0f * JUMP_HEIGHT) / (JUMP_TIME * JUMP_TIME);		// -2h / t^2
constexpr float INITIAL_JUMP_VELOCITY = (2.0f * JUMP_HEIGHT) / JUMP_TIME;		// 2h / t
constexpr float TERMINAL_VELOCITY = 4.0f * -GRAVITY;

void PlayerController::Update(const float& dt, Player* player)
{

	// DEBUG
	static bool isColliding = false;

	XMFLOAT3 prevPos = player->m_position;
	XMVECTOR prevPlayerFeetPos = { prevPos.x, prevPos.y - 2.0f, prevPos.z };
	XMVECTOR deltaTranslation = { 0, 0, 0, 1 };
	XMVECTOR deltaRotation = { 0.0f, 0.0f, 0.0f };
	player->m_acceleration = { 0.0f, GRAVITY, 0.0f };

	XMMATRIX worldMatrix = player->m_camera->GetWorldMatrix();

	XMVECTOR prevX, prevY, prevZ;
	prevX = worldMatrix.r[0];
	prevY = worldMatrix.r[1];
	prevZ = worldMatrix.r[2];

	///
	//
	// Update position
	//
	///

	player->m_allowJump = false;

	if (Input::IsKeyDown(KeyCode::O) || Input::IsKeyDown(KeyCode::W)) // FORWARD
		deltaTranslation.m128_f32[2] += player->m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::L) || Input::IsKeyDown(KeyCode::S)) // BACKWARD
		deltaTranslation.m128_f32[2] -= player->m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::K) || Input::IsKeyDown(KeyCode::A)) // LEFT
		deltaTranslation.m128_f32[0] -= player->m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::SEMICOLON) || Input::IsKeyDown(KeyCode::D)) // RIGHT
		deltaTranslation.m128_f32[0] += player->m_movementSpeed * dt;
	if (Input::IsKeyDown(KeyCode::W)) // TEMP
		deltaTranslation.m128_f32[1] -= player->m_movementSpeed * dt * 5.0f;


	// Update rotation only if LMB is held down

	// NOTE!
	//
	//	THERE IS AN ISSUE WITH WINDOW RESIZING/FRAMERATE WHERE
	//	CAMREA ROTATION SPEEDS UP OR SLOWS DOWN
	//
	if (Input::IsMouseDown(MouseCode::LBUTTON))
	{
		// Rotation around the X axis (look up/down)
		deltaRotation.m128_f32[0] = Input::GetMouseDeltaY() * player->m_camera->GetRotationSpeed() * 0.001f;
		// Rotation around the Y axis (look left/right)
		deltaRotation.m128_f32[1] = Input::GetMouseDeltaX() * player->m_camera->GetRotationSpeed() * 0.001f;
	}


	// Build the rotation and translation matrices
	XMMATRIX rotXMatrix = XMMatrixRotationX(deltaRotation.m128_f32[0]);
	XMMATRIX rotYMatrix = XMMatrixRotationY(deltaRotation.m128_f32[1]);
	XMMATRIX translationMatrixXZ = XMMatrixTranslation
	(
		deltaTranslation.m128_f32[0], deltaTranslation.m128_f32[1], deltaTranslation.m128_f32[2]
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


	// We have "moved into collision", so don't apply translation
	XMVECTOR newPlayerFeetPos = { worldMatrix.r[3].m128_f32[0], worldMatrix.r[3].m128_f32[1] - 2.0f, worldMatrix.r[3].m128_f32[2] };
	XMFLOAT3 newPos = { worldMatrix.r[3].m128_f32[0], worldMatrix.r[3].m128_f32[1], worldMatrix.r[3].m128_f32[2] };
	bool isCollidingWithWall = Physics::DetectCollision(newPlayerFeetPos);
	if (isCollidingWithWall)
	{
		XMFLOAT3 velocityToAdd = { newPos.x - prevPos.x, newPos.y - prevPos.y, newPos.z - prevPos.z };

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
		XMStoreFloat3(&player->m_position, worldMatrix.r[3]);
	}

	// Store the position after allowing player movement
	XMVECTOR playerPosAfterMovement = { worldMatrix.r[3].m128_f32[0], worldMatrix.r[3].m128_f32[1], worldMatrix.r[3].m128_f32[2] };

	XMVECTOR vel = XMLoadFloat3(&player->m_velocity);

	// Apply gravity
	//Physics::ApplyAcceleration(vel, { 0.0f, GRAVITY, 0.0f }, dt);

	// Cap velocity at a reasonable terminal velocity value
	if (abs(vel.m128_f32[1]) > TERMINAL_VELOCITY) vel.m128_f32[1] = vel.m128_f32[1] > 0 ? TERMINAL_VELOCITY : -TERMINAL_VELOCITY;

	Physics::ApplyVelocity(worldMatrix.r[3], vel, dt);

	XMVECTOR playerFeetPosAfterGravity = { worldMatrix.r[3].m128_f32[0], worldMatrix.r[3].m128_f32[1] - 2.0f, worldMatrix.r[3].m128_f32[2] };
	bool isCollidingWithFloor = Physics::DetectCollision(playerFeetPosAfterGravity);
	// If collision is detected after applying gravity, revert
	if (isCollidingWithFloor)
	{
		worldMatrix.r[3] = 
		{ 
			playerPosAfterMovement.m128_f32[0], 
			floor(playerPosAfterMovement.m128_f32[1]),
			playerPosAfterMovement.m128_f32[2]
		};

		// Set the W component of the position to 1
		worldMatrix.r[3].m128_f32[3] = 1.0f;

		// Reset acceleration and velocity
		vel = { 0.0f, 0.0f, 0.0f };

		// Allow the player to jump
		if(Input::IsKeyDown(KeyCode::SPACE))
		{
			// Apply an initial vertical velocity
			vel.m128_f32[1] += INITIAL_JUMP_VELOCITY;
			Physics::ApplyVelocity(worldMatrix.r[3], vel, dt);
		}
	}

	XMStoreFloat3(&player->m_velocity, vel);

	// Update the position and rotation camera values
	XMStoreFloat3(&player->m_position, worldMatrix.r[3]);

	XMFLOAT3 cameraRot = player->m_camera->GetRotation();
	player->m_camera->SetRotation(
		{
			cameraRot.x + deltaRotation.m128_f32[0],
			cameraRot.x + deltaRotation.m128_f32[1],
			cameraRot.x + deltaRotation.m128_f32[2]
		});

	// DEBUG SETTINGS
	PlayerPhysics_Data::accel = player->m_acceleration;
	PlayerPhysics_Data::vel = player->m_velocity;
	PlayerPhysics_Data::isCollidingFloor = isCollidingWithFloor;
	PlayerPhysics_Data::isCollidingWall = isCollidingWithWall;

	// Re-assign the view matrix with all the changes
	player->m_camera->SetWorldMatrix(worldMatrix);
}