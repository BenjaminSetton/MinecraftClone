#ifndef _INPUT_H
#define _INPUT_H

#include <bitset>
#include "../Core/Events/IEvent.h"

class Input : public EventObserver
{
public:

	Input();
	~Input();
	Input(const Input&) = default;

	static void KeyUp(unsigned int key);
	static void KeyDown(unsigned int key);
	static bool IsKeyDown(unsigned int key);

	virtual void OnEvent(const Event& event) override;

private:

	static std::bitset<256> m_keys;

};

#endif