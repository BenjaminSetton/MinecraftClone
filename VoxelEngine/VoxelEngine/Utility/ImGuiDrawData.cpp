#include "../Misc/pch.h"
#include "ImGuiDrawData.h"

//
//	CYCLE_DATA
//
DirectX::XMFLOAT4 Cycle_Data::sunColor = { 0.0f, 0.0f, 0.0f, 1.0f };
DirectX::XMFLOAT4 Cycle_Data::moonColor = { 0.0f, 0.0f, 0.0f, 1.0f };
DirectX::XMFLOAT3 Cycle_Data::sunDir = { 0.0f, 0.0f, 0.0f };
DirectX::XMFLOAT3 Cycle_Data::moonDir = { 0.0f, 0.0f, 0.0f };
DirectX::XMFLOAT3 Cycle_Data::sunPos = { 0.0f, 0.0f, 0.0f };
DirectX::XMFLOAT3 Cycle_Data::moonPos = { 0.0f, 0.0f, 0.0f };
float Cycle_Data::elapsedTime = 0.0f;
float Cycle_Data::timePct = 0.0f;
const char* Cycle_Data::cycle = "";
const char* Cycle_Data::time = "";

//
//	BLOCKSHADER_DATA
//
int BlockShader_Data::debugVerts = 0;
int BlockShader_Data::numDrawCalls = 0;
bool BlockShader_Data::enableFrustumCulling = false;

//
//	RENDERER_DATA
//
DirectX::XMFLOAT3 Renderer_Data::playerPos = { 0.0f, 0.0f, 0.0f };
DirectX::XMFLOAT3 Renderer_Data::playerPosChunkSpace = { 0.0f, 0.0f, 0.0f };
int Renderer_Data::numActiveChunks = 0;
uint32_t Renderer_Data::renderDist = 0;

//
// PLAYERPHYSICS_DATA
//
DirectX::XMFLOAT3 PlayerPhysics_Data::accel = { 0.0f, 0.0f, 0.0f };
DirectX::XMFLOAT3 PlayerPhysics_Data::vel = { 0.0f, 0.0f, 0.0f };
bool PlayerPhysics_Data::isCollidingWall = false;
bool PlayerPhysics_Data::isCollidingFloor = false;
float PlayerPhysics_Data::deltaXRot = 0.0f;
float PlayerPhysics_Data::deltaYRot = 0.0f;

//
// CHUNKMANAGER_DATA
//
float ChunkManager_Data::updateTimer = 0.0f;
float ChunkManager_Data::deletionLoop = 0.0f;
float ChunkManager_Data::creationLoop = 0.0f;
float ChunkManager_Data::deletingChunks = 0.0f;
float ChunkManager_Data::creatingChunks = 0.0f;


//
// GRAPHICSTIMER_DATA
//
float GraphicsTimer_Data::frameTimer = 0.0f;
float GraphicsTimer_Data::presentTimer = 0.0f;