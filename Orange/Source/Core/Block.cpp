#include "../Misc/pch.h"
#include "Block.h"

/// NOTE!
// The block class definition are super brief on purpose.
// It is important to reduce memory overhead since millions of blocks
// are going to be rendered

Block::Block(){ /* Constructor is not necessary */ }
Block::Block(const Block&){ /* Copy constructor is not necessary */ }
Block::Block(BlockType type) : m_type(type) {}
Block::~Block(){ /* Destructor is not necessary */}

void Block::SetType(const BlockType type) { m_type = type; }
const BlockType Block::GetType() { return m_type; }