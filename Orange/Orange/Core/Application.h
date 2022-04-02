#ifndef _APPLICATION_H
#define _APPLICATION_H

#include "EditorLayer.h"
#include "../Utility/Input.h"
#include "Graphics.h"
#include "../Utility/Clock.h"
#include "./Events/IEvent.h"
#include "./Events/KeyboardEvent.h"
#include "./Events/MouseEvent.h"
#include "Window.h"

class Application : public EventSubject
{
public:

	static Application* Handle;

	Application();
	Application(const Application&) = default;
	~Application();

	bool Initialize();

	void Run();

	void Shutdown();

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

	const Window* GetMainWindow();

private:

	bool Update();

	Window* m_mainWindow;

	Input* m_Input;
	Graphics* m_Graphics;
	Clock* m_Clock;

	EditorLayer* m_editorLayer;


};

#endif
