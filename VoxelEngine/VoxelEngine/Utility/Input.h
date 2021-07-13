#ifndef _INPUT_H
#define _INPUT_H

#include <bitset>

class Input
{
public:

	Input();
	~Input();
	Input(const Input&);

	static void KeyUp(unsigned int key);
	static void KeyDown(unsigned int key);
	static bool IsKeyDown(unsigned int key);

private:

	static std::bitset<256> m_keys;

};

#endif