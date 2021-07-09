#ifndef _APPLICATION_H
#define _APPLICATION_H

////////////////////////
///	INCLUDES
////////////////////////
#include <windows.h>

#include "Graphics.h"
#include "Clock.h"
#include "Input.h"

class Application
{
public:

	Application();
	~Application();
	Application(const Application&);

	bool Initialize();

	void Run();

	void Shutdown();

private:

	bool Frame();


	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input* m_Input;
	Graphics* m_Graphics;
	Clock* m_Clock;
};

#endif
