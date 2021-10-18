#ifndef _IMGUIDRAWDATA_H
#define _IMGUIDRAWDATA_H

#include <DirectXMath.h>

// This file includes all structs that ImGuiWrapper will use to render data

struct Cycle_Data
{

	static DirectX::XMFLOAT4 sunColor;
	static DirectX::XMFLOAT4 moonColor;

	static DirectX::XMFLOAT3 sunDir;
	static DirectX::XMFLOAT3 moonDir;
	static DirectX::XMFLOAT3 sunPos;
	static DirectX::XMFLOAT3 moonPos;

	static float elapsedTime;
	static float timePct;

	static const char* cycle;
	static const char* time;

};

struct BlockShader_Data
{

	static int debugVerts;
	static int numDrawCalls;
	static bool enableFrustumCulling;

};

struct Renderer_Data
{

	static DirectX::XMFLOAT3 playerPos;
	static DirectX::XMFLOAT3 playerPosChunkSpace;
	static int numActiveChunks;
	static uint32_t renderDist;

};

struct PlayerPhysics_Data
{
	static DirectX::XMFLOAT3 accel;
	static DirectX::XMFLOAT3 vel;
	static bool isCollidingFloor;
	static bool isCollidingWall;
};

#endif _IMGUIDRAWDATA_H