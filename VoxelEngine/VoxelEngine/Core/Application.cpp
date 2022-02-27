#include "../Misc/pch.h"

#include "Application.h"
#include "./Events/KeyCodes.h"

HWND Application::m_hwnd = HWND();

// Windows procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return ApplicationHandle->MessageHandler(hwnd, msg, wparam, lparam);
}

Application::Application() : EventSubject(),
	m_applicationName(L""), m_hinstance(nullptr), m_Input(nullptr), m_Graphics(nullptr), m_Clock(nullptr), m_player(nullptr)
{
}

Application::~Application()
{
	// Clean up in reverse order of object initialization

	if (m_Clock)
	{
		delete m_Clock;
		m_Clock = nullptr;
	}

	if (m_Graphics)
	{
		// Shutdown the graphics class before deleting
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	if(m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	if(m_player)
	{
		delete m_player;
	}
}

bool Application::Initialize() 
{
	UINT screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	ApplicationHandle = this;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new Input;
	Subscribe(m_Input);

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new Graphics;
	if (!m_Graphics) { return false; }

	// Initialize the player object
	m_player = new Player();

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd, m_player);
	if (!result) return false;

	// Create the clock object
	m_Clock = new Clock;
	if (!m_Clock) return false;


	return true;
}

void Application::Run() 
{
	MSG msg;
	bool isRunning = true;

	while (isRunning) 
	{
		// Handle the windows messages
		while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT) return;
		}

		isRunning = Frame();
	}

}

void Application::Shutdown() 
{
	// Shutdown the application and all of it's dependencies 
	if (m_Input) 
	{
		delete m_Input;
		m_Input = nullptr;
	}
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}
	if (m_Clock)
	{
		delete m_Clock;
		m_Clock = nullptr;
	}

	ShutdownWindows();
}

bool Application::Frame() 
{

	// Quit running if ESC is pressed
	if (m_Input->IsKeyDown(VK_ESCAPE)) return false;
	m_Input->Update();

	// Signal the clock every frame to obtain deltaTime
	m_Clock->Signal();

	// Store the frame's delta time in dt
	float dt = m_Clock->GetDeltaTime(Clock::TimePrecision::SECONDS);
	// Prevent delta time from giving wack results after moving game window
	dt = dt > 0.5f ? 0.016666f : dt;


	
	// Update physics

	// Update the player
	m_player->Update(dt);

	// Process each frame in the graphics class
	m_Graphics->Frame(dt);

	return true;
}

void Application::InitializeWindows(UINT& screenWidth, UINT& screenHeight) 
{
	int posX, posY;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Voxel Engine";

	ApplicationHandle = this;

	// Setup the windows class
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 1600x900 (16:9) resolution.
		screenWidth = 1600;
		screenHeight = 900;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_OVERLAPPEDWINDOW | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	return;
}


LRESULT CALLBACK Application::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (m_Graphics && m_Graphics->WndProc(hwnd, msg, wparam, lparam)) return true;

	switch (msg)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	case WM_KEYDOWN:
	{
		KeyboardDownEvent keyDown = KeyboardDownEvent(static_cast<uint16_t>(wparam));
		Broadcast(keyDown);
		//VX_LOG("Key down: %c", static_cast<uint16_t>(wparam));
		return 0;
	}
	case WM_KEYUP:
	{
		KeyboardUpEvent keyUp = KeyboardUpEvent(static_cast<uint16_t>(wparam));
		Broadcast(keyUp);
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		MouseMovedEvent mouseMoved = MouseMovedEvent(LOWORD(lparam), HIWORD(lparam));
		Broadcast(mouseMoved);
		//VX_LOG("Mouse moved: %2.2f, %2.2f", LOWORD(lparam), HIWORD(lparam));
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		MouseButtonDownEvent lmbDown = MouseButtonDownEvent(MK_LBUTTON);
		Broadcast(lmbDown);
		return 0;
	}
	case WM_LBUTTONUP:
	{
		MouseButtonUpEvent lmbUp = MouseButtonUpEvent(MK_LBUTTON);
		Broadcast(lmbUp);
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		MouseButtonDownEvent rmbDown = MouseButtonDownEvent(MK_RBUTTON);
		Broadcast(rmbDown);
		return 0;
	}
	case WM_RBUTTONUP:
	{
		MouseButtonUpEvent rmbUp = MouseButtonUpEvent(MK_RBUTTON);
		Broadcast(rmbUp);
		return 0;
	}
	case WM_MOUSEHWHEEL:
	{
		MouseScrolledEvent scroll = MouseScrolledEvent(HIWORD(wparam));
		Broadcast(scroll);
		return 0;
	}
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);

}

const uint32_t Application::GetWindowWidth()
{
	WINDOWINFO wi;
	wi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(m_hwnd, &wi);
	return wi.rcClient.right - wi.rcClient.left;
}

const uint32_t Application::GetWindowHeight()
{
	WINDOWINFO wi;
	wi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(m_hwnd, &wi);
	return wi.rcClient.bottom - wi.rcClient.top;
}

void Application::ShutdownWindows()
{

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;

	// Release the pointer to this class.
	ApplicationHandle = nullptr;

	return;
}
