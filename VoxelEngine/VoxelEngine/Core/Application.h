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
#include "./Events/IEvent.h"
#include "./Events/KeyboardEvent.h"
#include "./Events/MouseEvent.h"

class Application : public EventSubject
{
public:

	Application();
	Application(const Application&) = default;
	~Application();

	bool Initialize();

	void Run();

	void Shutdown();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

	static const uint32_t GetWindowWidth();

	static const uint32_t GetWindowHeight();

private:

	bool Frame();

	void InitializeWindows(UINT& screenWidth, UINT& screenHeight);

	void ShutdownWindows();

	static HWND m_hwnd;

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;

	Input* m_Input;
	Graphics* m_Graphics;
	Clock* m_Clock;

	Player* m_player = nullptr;
};

// Static application handle pointer
static Application* ApplicationHandle = nullptr;

#endif
