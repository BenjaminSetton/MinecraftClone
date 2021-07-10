#include "Input.h"

std::bitset<256> Input::m_keys = std::bitset<256>(false);

Input::Input(){}
Input::~Input(){}
Input::Input(const Input&){}


void Input::KeyUp(unsigned int key)
{
	m_keys[key] = false;
}
void Input::KeyDown(unsigned int key) 
{
	m_keys[key] = true;
}
bool Input::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}