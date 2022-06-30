#ifndef _LAYER_H
#define _LAYER_H

#include "../Utility/Utility.h"

// Virtual base layer class
// TODO: Figure out how to relate editor vs. playmode layers, and put all common data here
class Layer
{

public:

	virtual void Push() {}

	virtual void Pop() {}

	virtual void Update(const float dt) { UNUSED(dt); }

	virtual void Draw() {}

};

#endif