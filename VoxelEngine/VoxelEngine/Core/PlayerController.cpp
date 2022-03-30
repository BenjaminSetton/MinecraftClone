#include "../Misc/pch.h"
#include "PlayerController.h"

#include "Player.h"

#include "../Utility/Input.h"
#include "Events/KeyCodes.h"
#include "Physics.h"
#include "../Utility/Math.h"
#include "ChunkManager.h"
#include "BlockSelectionIndicator.h"

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
	//

	int32_t numberOfCurveSamples = 15;
	static XMFLOAT3 startingLerpValues = { 0.0f, 0.0f, 0.0f };
	static XMFLOAT3 stoppingLerpValues = { 0.0f, 0.0f, 0.0f };

	switch (player->GetSelectedCameraType())
	{
	case CameraType::FirstPerson:
	{
		XMFLOAT3 deltaTranslation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 deltaRotation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 currentVelocity = { 0.0f, player->m_velocity.y, 0.0f };
		std::function<float(float)> startFunction = [=](const float val) { return POW2(val); };
		std::function<float(float)> stopFunction = [=](const float val) 
		{
			float mappedVal = (-0.2f * log(val));
			VX_MATH::Clamp(mappedVal, 0.0f, 1.0f);
			return mappedVal;
		};

		// Update position
		if (Input::IsKeyDown(KeyCode::O) || Input::IsKeyDown(KeyCode::W)) // FORWARD
		{
			// We're switching directions, so stop first
			if (currentVelocity.z < 0)
			{
				stoppingLerpValues.z += 1.0f / static_cast<float>(numberOfCurveSamples);
				startingLerpValues.z = 0.0f;
				VX_MATH::Clamp(stoppingLerpValues.z, 0.0f, 1.0f);
				currentVelocity.z = stopFunction(stoppingLerpValues.z) * player->m_movementSpeed;
			}
			else
			{
				stoppingLerpValues.z = 0.0f;
				startingLerpValues.z += 1.0f / static_cast<float>(numberOfCurveSamples);
				VX_MATH::Clamp(startingLerpValues.z, 0.0f, 1.0f);
				currentVelocity.z = startFunction(startingLerpValues.z) * player->m_movementSpeed;
			}
		}
		if (Input::IsKeyDown(KeyCode::L) || Input::IsKeyDown(KeyCode::S)) // BACKWARD
		{ 
			if (currentVelocity.z > 0)
			{
				stoppingLerpValues.z += 1.0f / static_cast<float>(numberOfCurveSamples);
				startingLerpValues.z = 0.0f;
				VX_MATH::Clamp(stoppingLerpValues.z, 0.0f, 1.0f);
				currentVelocity.z = -stopFunction(stoppingLerpValues.z) * player->m_movementSpeed;
			}
			else
			{
				stoppingLerpValues.z = 0.0f;
				startingLerpValues.z += 1.0f / static_cast<float>(numberOfCurveSamples);
				VX_MATH::Clamp(startingLerpValues.z, 0.0f, 1.0f);
				currentVelocity.z = -startFunction(startingLerpValues.z) * player->m_movementSpeed;

			}
		}
		if (Input::IsKeyDown(KeyCode::K) || Input::IsKeyDown(KeyCode::A)) // LEFT
		{
			if (currentVelocity.x > 0)
			{
				stoppingLerpValues.x += 1.0f / static_cast<float>(numberOfCurveSamples);
				startingLerpValues.x = 0.0f;
				VX_MATH::Clamp(stoppingLerpValues.x, 0.0f, 1.0f);
				currentVelocity.x = -stopFunction(stoppingLerpValues.x) * player->m_movementSpeed;
			}
			else
			{
				stoppingLerpValues.x = 0.0f;
				startingLerpValues.x += 1.0f / static_cast<float>(numberOfCurveSamples);
				VX_MATH::Clamp(startingLerpValues.x, 0.0f, 1.0f);
				currentVelocity.x = -startFunction(startingLerpValues.x) * player->m_movementSpeed;
			}
		}
		if (Input::IsKeyDown(KeyCode::SEMICOLON) || Input::IsKeyDown(KeyCode::D)) // RIGHT
		{
			if (currentVelocity.x < 0)
			{
				stoppingLerpValues.x += 1.0f / static_cast<float>(numberOfCurveSamples);
				startingLerpValues.x = 0.0f;
				VX_MATH::Clamp(stoppingLerpValues.x, 0.0f, 1.0f);
				currentVelocity.x = stopFunction(stoppingLerpValues.x) * player->m_movementSpeed;
			}
			else
			{
				stoppingLerpValues.x = 0.0f;
				startingLerpValues.x += 1.0f / static_cast<float>(numberOfCurveSamples);
				VX_MATH::Clamp(startingLerpValues.x, 0.0f, 1.0f);
				currentVelocity.x = startFunction(startingLerpValues.x) * player->m_movementSpeed;
			}
		}

		// Use Euler integration to get the translation
		Physics::ApplyVelocity(deltaTranslation, currentVelocity, dt);

		// Update rotation only if LMB is held down
		if (Input::IsMouseDown(MouseCode::LBUTTON))
		{
			// Rotation around the X axis (look up/down)
			deltaRotation.x = Input::GetMouseDeltaY() * player->m_FPSCamera->GetRotationSpeed();
			// Rotation around the Y axis (look left/right)
			deltaRotation.y = Input::GetMouseDeltaX() * player->m_FPSCamera->GetRotationSpeed();
		}

		// X Rotation
		player->m_rotation.y += deltaRotation.y;
		VX_MATH::Wrap(player->m_rotation.y, 0.0f, 360.0f);

		// Y Rotation
		player->m_rotation.x += deltaRotation.x;
		VX_MATH::Clamp(player->m_rotation.x, -89.9f, 89.9f);

		// we have rotations for x and y
		// create two rotation matrices, and translate along those axis for "newPos"
		XMFLOAT3 currentPlayerPos = player->m_hitbox.center;
		XMMATRIX localTransform = XMMatrixIdentity();
		XMMATRIX rotXMatrix = XMMatrixRotationX(VX_MATH::DegreesToRadians(player->m_rotation.x));
		XMMATRIX rotYMatrix = XMMatrixRotationY(VX_MATH::DegreesToRadians(player->m_rotation.y));
		localTransform = localTransform * rotYMatrix;
		localTransform = rotXMatrix * localTransform;
		XMMATRIX translationMatrixXZ = XMMatrixTranslation(deltaTranslation.x, 0.0f, deltaTranslation.z);

		XMVECTOR realZ = localTransform.r[2];

		// Cancel the Y offset so the player can't move up and down by looking up and down (this is only done for the sake of local translation calculation)
		XMVECTOR modifiedZ = localTransform.r[2];
		modifiedZ.m128_f32[1] = 0.0f;
		modifiedZ = XMVector3Normalize(modifiedZ);

		localTransform.r[2] = modifiedZ;
		localTransform = translationMatrixXZ * localTransform; // Local XZ translation
		localTransform.r[2] = realZ;

		XMFLOAT3 finalTranslatedPosition = { currentPlayerPos.x + localTransform.r[3].m128_f32[0], currentPlayerPos.y, currentPlayerPos.z + localTransform.r[3].m128_f32[2] };

		// Calculate local translation using deltaTranslation above, and pass in as "newPos"
		AABB prevPosHorizontal = { currentPlayerPos, player->m_hitbox.extent };
		AABB newPosHorizontal = { finalTranslatedPosition, player->m_hitbox.extent };
		XMFLOAT3 horizontalCollision = CheckForHorizontalCollision(newPosHorizontal, prevPosHorizontal);
		finalTranslatedPosition = newPosHorizontal.center;
		// Cancel out velocity on axes of collision
		if (horizontalCollision.x)
		{
			startingLerpValues.x = 0.0f;
			currentVelocity.x = 0.0f;
		}
		if (horizontalCollision.z)
		{
			startingLerpValues.z = 0.0f;
			currentVelocity.z = 0.0f;
		}

		// Apply gravity
		Physics::ApplyAcceleration(currentVelocity, { 0.0f, GRAVITY, 0.0f }, dt);

		// Cap velocity at a reasonable terminal velocity value
		if (abs(currentVelocity.y) > TERMINAL_VELOCITY) currentVelocity.y = VX_MATH::Sign(currentVelocity.y) * TERMINAL_VELOCITY;

		// Apply acceleration to velocity
		XMFLOAT3 posBeforeGravity = finalTranslatedPosition;
		Physics::ApplyVelocity(finalTranslatedPosition, { 0.0f, currentVelocity.y, 0.0f }, dt);

		// Takes in the following arguments: posAfterGravityF3, posBeforeGravityF3, dt
		AABB prevPosVertical = { posBeforeGravity, player->m_hitbox.extent };
		AABB newPosVertical = { finalTranslatedPosition, player->m_hitbox.extent };
		XMFLOAT3 verticalCollision = CheckForVerticalCollision(newPosVertical, prevPosVertical);
		finalTranslatedPosition = newPosVertical.center;
		if (verticalCollision.y == 1.0f)
		{
			currentVelocity.y = 0.0f;
			player->m_allowJump = true;
		}

		// Allow the player to jump
		if (Input::IsKeyDown(KeyCode::SPACE))
		{
			if (player->m_allowJump)
			{
				// Apply an initial vertical velocity
				currentVelocity.y += INITIAL_JUMP_VELOCITY;
				Physics::ApplyVelocity(finalTranslatedPosition, { 0.0f, currentVelocity.y, 0.0f }, dt);
				player->m_allowJump = false;
			}
		}



		// Finally store the final player position
		player->m_position = { finalTranslatedPosition.x, finalTranslatedPosition.y + 1.0f, finalTranslatedPosition.z };
		player->m_velocity = currentVelocity;
		player->m_hitbox.center = finalTranslatedPosition;
		DebugRenderer::DrawAABB(player->m_hitbox.center, player->m_hitbox.extent, { 1.0f, 0.0f, 0.0f, 1.0f });

		BlockSelectionIndicator::Update(dt);

		player->m_FPSCamera->SetCameraParameters(player->m_position, player->m_rotation);
		player->m_FPSCamera->Update(dt);

		// TEST RAYCASTING, PLEASE REMOVE
#pragma region RAYCAST_TEST
		VX_COOLDOWN(0.1f, dt)
		{
			XMFLOAT3 rayHit = { 0, 0, 0 };
			if (Input::IsMouseDown(MouseCode::RBUTTON))
			{
				XMFLOAT3 rayPos, rayDir;
				XMStoreFloat3(&rayPos, player->m_FPSCamera->GetWorldMatrix().r[3]);
				XMStoreFloat3(&rayDir, DirectX::XMVector3Normalize(player->m_FPSCamera->GetWorldMatrix().r[2]));
				if (VX_MATH::Raycast(rayPos, rayDir, player->GetInteractionRange(), ChunkManager::CheckBlockRaycast, &rayHit))
				{
					//VX_LOG("Target Hit - [%2.2f, %2.2f, %2.2f]", rayHit.x, rayHit.y, rayHit.z);
					DebugRenderer::DrawLine(rayPos, rayHit, { 1.0f, 0, 0, 1.0f });
					DebugRenderer::DrawSphere(1, rayHit, 0.01f, { 1.0f, 0, 0, 1.0f });
				}
				else
				{
					//VX_LOG("Target Missed");
				}
			}
		}
#pragma endregion
		break;
	}
	case CameraType::Debug:
	{
		XMFLOAT3 deltaTranslation = { 0.0f, 0.0f, 0.0f };
		XMFLOAT3 deltaRotation = { 0.0f, 0.0f, 0.0f };
		static XMFLOAT3 position = player->m_position;
		static XMFLOAT3 rotation = player->m_rotation;

		// Update position
		if (Input::IsKeyDown(KeyCode::O) || Input::IsKeyDown(KeyCode::W)) // FORWARD
			deltaTranslation.z += player->m_movementSpeed * dt;
		if (Input::IsKeyDown(KeyCode::L) || Input::IsKeyDown(KeyCode::S)) // BACKWARD
			deltaTranslation.z -= player->m_movementSpeed * dt;
		if (Input::IsKeyDown(KeyCode::K) || Input::IsKeyDown(KeyCode::A)) // LEFT
			deltaTranslation.x -= player->m_movementSpeed * dt;
		if (Input::IsKeyDown(KeyCode::SEMICOLON) || Input::IsKeyDown(KeyCode::D)) // RIGHT
			deltaTranslation.x += player->m_movementSpeed * dt;
		if (Input::IsKeyDown(KeyCode::SPACE)) // UP
			deltaTranslation.y += player->m_movementSpeed * dt;
		if (Input::IsKeyDown(KeyCode::RSHIFT)) // DOWN
			deltaTranslation.y -= player->m_movementSpeed * dt;

		// Update rotation only if LMB is held down
		if (Input::IsMouseDown(MouseCode::LBUTTON))
		{
			// Rotation around the X axis (look up/down)
			deltaRotation.x = Input::GetMouseDeltaY() * player->m_debugCamera->GetRotationSpeed();
			// Rotation around the Y axis (look left/right)
			deltaRotation.y = Input::GetMouseDeltaX() * player->m_debugCamera->GetRotationSpeed();
		}

		// X Rotation
		rotation.y += deltaRotation.y;
		VX_MATH::Wrap(rotation.y, 0.0f, 360.0f);

		// Y Rotation
		rotation.x += deltaRotation.x;
		VX_MATH::Clamp(rotation.x, -89.9f, 89.9f);

		XMMATRIX localTransform = XMMatrixIdentity();
		XMMATRIX rotXMatrix = XMMatrixRotationX(VX_MATH::DegreesToRadians(rotation.x));
		XMMATRIX rotYMatrix = XMMatrixRotationY(VX_MATH::DegreesToRadians(rotation.y));
		XMMATRIX translationMatrixXZ = XMMatrixTranslation(deltaTranslation.x, 0.0f, deltaTranslation.z);
		localTransform = localTransform * rotYMatrix;
		localTransform = rotXMatrix * localTransform;
		localTransform = translationMatrixXZ * localTransform; // Local XZ translation, global Y translation

		position = { position.x + localTransform.r[3].m128_f32[0], position.y + localTransform.r[3].m128_f32[1] + deltaTranslation.y, position.z + localTransform.r[3].m128_f32[2]};

		player->m_debugCamera->SetCameraParameters(position, rotation);
		player->m_debugCamera->Update(dt);

		break;
	}
	case CameraType::ThirdPerson:
	{
		// Unimplemented
		break;
	}
	}

	// DEBUG STUFF
	XMFLOAT3 zAxis = { 0.0f, 0.0f, 0.0f };
	XMStoreFloat3(&zAxis, player->m_FPSCamera->GetWorldMatrix().r[2]);
	DebugRenderer::DrawAABB(player->m_hitbox.center, player->m_hitbox.extent, { 1.0f, 0.0f, 0.0f, 1.0f });
	//DebugRenderer::DrawSphere(1, { player->m_hitbox.center.x - player->m_hitbox.extent.x, player->m_hitbox.center.y - player->m_hitbox.extent.y, player->m_hitbox.center.z - player->m_hitbox.extent.z }, 0.05f, { 0.0f, 0.0f, 1.0f, 1.0f });
	//DebugRenderer::DrawSphere(1, { player->m_hitbox.center.x + player->m_hitbox.extent.x, player->m_hitbox.center.y + player->m_hitbox.extent.y, player->m_hitbox.center.z + player->m_hitbox.extent.z }, 0.05f, { 0.0f, 0.0f, 1.0f, 1.0f });
	DebugRenderer::DrawLine(player->m_position, { player->m_position.x + zAxis.x, player->m_position.y + zAxis.y, player->m_position.z + zAxis.z }, { 0.0f, 1.0f, 0.0f, 1.0f });

	// DEBUG SETTINGS
	Renderer_Data::playerRot = { player->GetRotation().x, player->GetRotation().y, player->GetRotation().z };
	PlayerPhysics_Data::accel = player->m_acceleration;
	PlayerPhysics_Data::vel = player->m_velocity;
}

XMFLOAT3 PlayerController::CheckForHorizontalCollision(AABB& newPos, const AABB& prevPos)
{

	float epsilon = 0.0001f;
	bool isCollidingWithWall = Physics::DetectCollision(newPos);
	PlayerPhysics_Data::isCollidingWall = isCollidingWithWall;
	XMFLOAT3 axesOfCollision = { 0.0f, 0.0f, 0.0f };

	// We have "moved into collision", so revert enough distance to resolve AABB to AABB collision
	if (isCollidingWithWall)
	{
		XMFLOAT3 velocityToAdd = { newPos.center.x - prevPos.center.x, newPos.center.y - prevPos.center.y, newPos.center.z - prevPos.center.z };

		// Only check X and Z axis
		for (uint32_t axis = 0; axis <= 2; axis += 2)
		{
			AABB currentPos;
			currentPos.center = prevPos.center;
			currentPos.extent = prevPos.extent;
			XMFLOAT3_BRACKET_OP_32(currentPos.center, axis) += XMFLOAT3_BRACKET_OP_32(velocityToAdd, axis);
			if (XMFLOAT3_BRACKET_OP_32(velocityToAdd, axis) == 0.0f) continue;

			if (Physics::DetectCollision(currentPos))
			{
				float overlap = 0.0f;
				if (XMFLOAT3_BRACKET_OP_32(velocityToAdd, axis) > 0)
				{
					XMFLOAT3 aabbMax = { newPos.center.x + newPos.extent.x, newPos.center.y + newPos.extent.y, newPos.center.z + newPos.extent.z };
					overlap = XMFLOAT3_BRACKET_OP_32(aabbMax, axis) - floor(XMFLOAT3_BRACKET_OP_32(aabbMax, axis)) + epsilon;
				}
				else if (XMFLOAT3_BRACKET_OP_32(velocityToAdd, axis) < 0)
				{
					XMFLOAT3 aabbMin = { newPos.center.x - newPos.extent.x, newPos.center.y - newPos.extent.y, newPos.center.z - newPos.extent.z };
					overlap = ceil(XMFLOAT3_BRACKET_OP_32(aabbMin, axis)) - XMFLOAT3_BRACKET_OP_32(aabbMin, axis);
				}

				XMFLOAT3_BRACKET_OP_32(velocityToAdd, axis) = VX_MATH::Sign(XMFLOAT3_BRACKET_OP_32(velocityToAdd, axis)) * (abs(XMFLOAT3_BRACKET_OP_32(velocityToAdd, axis)) - overlap);
				VX_ASSERT(overlap > 0);

				// Set collision flag to true on current axis
				XMFLOAT3_BRACKET_OP_32(axesOfCollision, axis) = 1.0f;
				
			}
		}

		newPos.center = { prevPos.center.x + velocityToAdd.x, newPos.center.y, prevPos.center.z + velocityToAdd.z };

		VX_ASSERT((abs(velocityToAdd.x) >= 0 && abs(velocityToAdd.x) < 1));
		VX_ASSERT((abs(velocityToAdd.z) >= 0 && abs(velocityToAdd.z) < 1));
	}

	return axesOfCollision;
}

XMFLOAT3 PlayerController::CheckForVerticalCollision(AABB& newPos, const AABB& prevPos)
{
	bool isCollidingWithFloor = Physics::DetectCollision(newPos);
	XMFLOAT3 axesOfCollision = { 0.0f, 0.0f, 0.0f };

	// DEBUG
	PlayerPhysics_Data::isCollidingFloor = isCollidingWithFloor;
	static XMFLOAT3 uniqueCollisionPos = { 0.0f, 0.0f, 0.0f };
	//

	float velocityToAddOnY = newPos.center.y - prevPos.center.y;

	// If collision is detected after applying gravity, revert
	if (isCollidingWithFloor)
	{
		float overlap = 0;
		if (velocityToAddOnY > 0)
		{
			XMFLOAT3 aabbMax = { newPos.center.x + newPos.extent.x, newPos.center.y + newPos.extent.y, newPos.center.z + newPos.extent.z };
			overlap = aabbMax.y - floor(aabbMax.y);
		}
		else if (velocityToAddOnY < 0)
		{
			XMFLOAT3 aabbMin = { newPos.center.x - newPos.extent.x, newPos.center.y - newPos.extent.y, newPos.center.z - newPos.extent.z };
			overlap = ceil(aabbMin.y) - aabbMin.y;
		}
		VX_ASSERT(overlap > 0);
		velocityToAddOnY = VX_MATH::Sign(velocityToAddOnY) * (abs(velocityToAddOnY) - overlap);
		newPos.center.y = prevPos.center.y + velocityToAddOnY;

		// Set collision flag to true on Y axis
		axesOfCollision.y = 1.0f;

	}

	return axesOfCollision;
}