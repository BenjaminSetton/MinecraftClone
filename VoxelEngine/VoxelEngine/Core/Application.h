#ifndef _APPLICATION_H
#define _APPLICATION_H

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN

////////////////////////
///	INCLUDES
////////////////////////
#include <windows.h>

#include "../Utility/Input.h"
#include "Graphics.h"
#include "../Utility/Clock.h"
#include "./Events/IEvent.h";

class Application : public EventSubject
{
public:

	Application();
	Application(const Application&);
	~Application();

	bool Initialize();

	void Run();

	void Shutdown();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

private:

	bool Frame();

	void InitializeWindows(UINT& screenWidth, UINT& screenHeight);

	void ShutdownWindows();


	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	Input* m_Input;
	Graphics* m_Graphics;
	Clock* m_Clock;
};

#endif
