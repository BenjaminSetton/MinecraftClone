#ifndef _LAYER_H
#define _LAYER_H

// Base layer class
// TODO: Figure out how to relate editor vs. playmode layers, and put all common data here
class Layer
{
public:

	virtual void Push() {}

	virtual void Pop() {}

private:
};

#endif