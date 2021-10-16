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
bool BlockShader_Data::enableFrustumCulling = true;

//
//	RENDERER_DATA
//
DirectX::XMFLOAT3 Renderer_Data::playerPos = { 0.0f, 0.0f, 0.0f };
DirectX::XMFLOAT3 Renderer_Data::playerPosChunkSpace = { 0.0f, 0.0f, 0.0f };
int Renderer_Data::numActiveChunks = 0;

//
// PLAYERPHYSICS_DATA
//
DirectX::XMFLOAT3 PlayerPhysics_Data::accel = { 0.0f, 0.0f, 0.0f };
DirectX::XMFLOAT3 PlayerPhysics_Data::vel = { 0.0f, 0.0f, 0.0f };
bool PlayerPhysics_Data::isCollidingWall = false;
bool PlayerPhysics_Data::isCollidingFloor = false;



