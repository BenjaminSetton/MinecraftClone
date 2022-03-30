#ifndef _BLOCK_UVS_H
#define _BLOCK_UVS_H

#define NUM_BLOCKS 4

constexpr auto TEX_WIDTH = 1024;
constexpr auto TEX_HEIGHT = 512;

#include "Block.h"
#include <DirectXMath.h>

//                                                              TRB(2)                                                                 TRF(1)                                                             TLF(0)                                                       TLF(0)                                                      TLB(3)                                                       TRB(2)
#define TOP_FACE(left, top, width, height) { (left + width) / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }

//                                                                BRB(6)                                                            BRF(5)                                                                   BLF(4)                                                             BLF(4)                                                BLB(7)                                                BRB(6)
#define BOTTOM_FACE(left, top, width, height) { (left + width) / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }

//                                         TLF(10)                                                            BLF(9)                                                               BLB(8)                                                           BLB(8)                                                   TLB(11)                                        TLF(10)
#define LEFT_FACE(left, top, width, height) { (left + width) / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }

//                                                              BRB(12)                                                                       BRF(13)                                              TRF(14)                                              TRF(14)                                                 TRB(15)                                                         BRB(12)
#define RIGHT_FACE(left, top, width, height) { (left + width) / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }

//                                                         TRF(18)                                                                  BRF(17)                                                               BLF(16)                                                    TLF(19)                                           TRF(18)                                                              BLF(16)
#define FRONT_FACE(left, top, width, height) { (left + width) / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }

//                                                              BLB(20)                                                                     BRB(21)                                                    TRB(22)                                                         BLB(20)                                                  TRB(22)                                                   TLB(23)
#define BACK_FACE(left, top, width, height) { (left + width) / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, (top + height) / TEX_HEIGHT, 0.0f, 0.0f }, { left / TEX_WIDTH, height / TEX_HEIGHT, 0.0f, 0.0f }, { (left + width) / TEX_WIDTH, top / TEX_HEIGHT, 0.0f, 0.0f }

// They have to be float4's to abide by HLSL's packing optimization
// This is a design decision. Instead of forcing HLSL to pack an array
// into 8-byte vectors, we trade off memory for speed
static DirectX::XMFLOAT4 uvs[NUM_BLOCKS][36]
{
    // AIR
    { 
        TOP_FACE(0.0f, 0.0f, 0.0f, 0.0f),
        BOTTOM_FACE(0.0f, 0.0f, 0.0f, 0.0f),
        LEFT_FACE(0.0f, 0.0f, 0.0f, 0.0f),
        RIGHT_FACE(0.0f, 0.0f, 0.0f, 0.0f),
        FRONT_FACE(0.0f, 0.0f, 0.0f, 0.0f),
        BACK_FACE(0.0f, 0.0f, 0.0f, 0.0f)
    },

    // DIRT
    { 
        TOP_FACE(0.0f, 16.0f, 16.0f, 16.0f),
        BOTTOM_FACE(0.0f, 16.0f, 16.0f, 16.0f),
        LEFT_FACE(0.0f, 16.0f, 16.0f, 16.0f),
        RIGHT_FACE(0.0f, 16.0f, 16.0f, 16.0f),
        FRONT_FACE(0.0f, 16.0f, 16.0f, 16.0f),
        BACK_FACE(0.0f, 16.0f, 16.0f, 16.0f)
    },

    // STONE
    { 
        TOP_FACE(16.0f, 16.0f, 16.0f, 16.0f),
        BOTTOM_FACE(16.0f, 16.0f, 16.0f, 16.0f),
        LEFT_FACE(16.0f, 16.0f, 16.0f, 16.0f),
        RIGHT_FACE(16.0f, 16.0f, 16.0f, 16.0f),
        FRONT_FACE(16.0f, 16.0f, 16.0f, 16.0f),
        BACK_FACE(16.0f, 16.0f, 16.0f, 16.0f)
    },

    // GRASS
    {
        //TOP_FACE(0.0f, 0.0f, 16.0f, 16.0f),
        //BOTTOM_FACE(0.0f, 16.0f, 16.0f, 16.0f),
        //LEFT_FACE(16.0f, 0.0f, 16.0f, 16.0f),
        //RIGHT_FACE(16.0f, 0.0f, 16.0f, 16.0f),
        //FRONT_FACE(16.0f, 0.0f, 16.0f, 16.0f),
        //BACK_FACE(16.0f, 0.0f, 16.0f, 16.0f)

        TOP_FACE(0.0f, 16.0f, 16.0f, 16.0f),
        BOTTOM_FACE(0.0f, 16.0f, 16.0f, 16.0f),
        LEFT_FACE(0.0f, 16.0f, 16.0f, 16.0f),
        RIGHT_FACE(0.0f, 16.0f, 16.0f, 16.0f),
        FRONT_FACE(0.0f, 16.0f, 16.0f, 16.0f),
        BACK_FACE(0.0f, 16.0f, 16.0f, 16.0f)
    },
};



#endif