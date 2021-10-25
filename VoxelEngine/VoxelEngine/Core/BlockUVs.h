#ifndef _BLOCK_UVS_H
#define _BLOCK_UVS_H

#define NUM_BLOCKS 4

constexpr auto TEX_WIDTH = 1024;
constexpr auto TEX_HEIGHT = 512;

#include "Block.h"
#include <DirectXMath.h>

static DirectX::XMFLOAT2 uvs[NUM_BLOCKS][36]
{
    // AIR
    { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
      {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, 
      {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },

    // DIRT
    { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },

    // STONE
    { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
      {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },

    // GRASS
    {
        { 16.0f / TEX_WIDTH, 0.0f },                        // TRB(2)
        { 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // TRF(1)	// TOP
        { 0.0f, 16.0f / TEX_HEIGHT },                       // TLF(0)
        { 0.0f, 16.0f / TEX_HEIGHT },                       // TLF(0)
        { 0.0f, 0.0f },                                     // TLB(3)
        { 16.0f / TEX_WIDTH, 0.0f },                        // TRB(2)

        { 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BRB(6)
        { 0.03125f, 0.0625f },                              // BRF(5)	// BOTTOM
        { 0.0f, 0.0625f },                                  // BLF(4)
        { 0.0f, 0.0625f },                                  // BLF(4)
        { 0.0f, 16.0f / TEX_HEIGHT },                       // BLB(7)
        { 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BRB(6)

        { 32.0f / TEX_WIDTH, 0.0f },                        // TLF(10)
        { 32.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BLF(9)	// LEFT
        { 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BLB(8)
        { 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BLB(8)
        { 16.0f / TEX_WIDTH, 0.0f },                        // TLB(11)
        { 32.0f / TEX_WIDTH, 0.0f },                        // TLF(10)

        { 32.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BRB(12)
        { 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BRF(13)	// RIGHT
        { 16.0f / TEX_WIDTH, 0.0f },                        // TRF(14)
        { 16.0f / TEX_WIDTH, 0.0f },                        // TRF(14)
        { 32.0f / TEX_WIDTH, 0.0f },                        // TRB(15)
        { 32.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BRB(12)

        { 32.0f / TEX_WIDTH, 0.0f },                        // TRF(18)
        { 32.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BRF(17)	// FRONT
        { 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BLF(16)
        { 16.0f / TEX_WIDTH, 0.0f },                        // TLF(19)
        { 32.0f / TEX_WIDTH, 0.0f },                        // TRF(18)
        { 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BLF(16)

        { 32.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BLB(20)
        { 16.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BRB(21)	// BACK
        { 16.0f / TEX_WIDTH, 0.0f },                        // TRB(22)
        { 32.0f / TEX_WIDTH, 16.0f / TEX_HEIGHT },          // BLB(20)
        { 16.0f / TEX_WIDTH, 0.0f },                        // TRB(22)
        { 32.0f / TEX_WIDTH, 0.0f }                         // TLB(23)

    },
};



#endif