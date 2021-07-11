#ifndef _BLOCK_H
#define _BLOCK_H

#include<DirectXMath.h>

// NOTE: This is not the most efficient way to store vertices for voxel worlds, since most verts will
//		 be exactly on top of other verts. The origin is at { 0.0f, 0.0f, 0.0f }

static DirectX::XMFLOAT3 verts[8] =
{
	{ 0.0f, 0.0f, -1.0f }, // top-left-front (0)
	{ 1.0f, 0.0f, -1.0f }, // top-right-front (1)
	{ 1.0f, -1.0f, -1.0f }, // bottom-right-front (2)
	{ 0.0f, -1.0f, -1.0f }, // bottom-left-front (3)

	{ 0.0f, 0.0f, 0.0f }, // top-left-back (4)
	{ 1.0f, 0.0f, 0.0f }, // top-right-back (5)
	{ 1.0f, -1.0f, 0.0f }, // bottom-right-back (6)
	{ 0.0f, -1.0f, 0.0f }, // bottom-left-back (7)
};

static unsigned int indicies[36] =
{
	0, 1, 3, 1, 2, 3, // front face
	4, 0, 7, 0, 3, 7, // left face
	1, 5, 2, 5, 6, 2, // right face
	3, 2, 7, 2, 6, 7, // bottom face
	0, 1, 3, 1, 2, 3, // back face
};

#endif