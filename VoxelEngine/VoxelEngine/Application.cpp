#include "Application.h"

bool Application::Initialize() 
{
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;
	if (!m_Input) { return false; }

	// Initialize the input object.
	m_Input->Initialize();

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics) { return false; }

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	// Create the timer object.
	m_Timer = new TimerClass;
	if (!m_Timer) return false;

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void Application::Run() 
{
	MSG msg;
	bool running = true;

	while (running) 
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Check if the quit message is given and stop running if it has
		if (msg.message == WM_QUIT) running = false;
		else // we can go to the next frame
		{

		}


	}

}

void Application::Shutdown() 
{
	// Shutdown the application and all of it's dependencies 
}

bool Application::Frame() 
{

}