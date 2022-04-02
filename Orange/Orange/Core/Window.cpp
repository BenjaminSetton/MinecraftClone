#include "../Misc/pch.h"

#include "Application.h"
#include "Window.h"

// Windows procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return Application::Handle->MessageHandler(hwnd, msg, wparam, lparam);
}

void Window::Create(const WindowParameters& parameters)
{
	int posX = parameters.x;
	int posY = parameters.y;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_windowName = parameters.name;

	// Store the fullscreen mode
	m_isFullscreen = parameters.fullScreen;

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
	wc.lpszClassName = m_windowName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Setup window alignment position
	switch (parameters.alignment)
	{
	case TopLeft:
	{
		posX = 0;
		posY = 0;
		break;
	}
	case Centered:
	{
		posX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (parameters.width / 2);
		posY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (parameters.height / 2);
		break;
	}
	case BottomRight:
	{
		posX = GetSystemMetrics(SM_CXSCREEN) - parameters.width;
		posY = GetSystemMetrics(SM_CYSCREEN) - parameters.height;
		break;
	}
	}

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (m_isFullscreen)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)GetSystemMetrics(SM_CXSCREEN);
		dmScreenSettings.dmPelsHeight = (unsigned long)GetSystemMetrics(SM_CYSCREEN);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_windowName, m_windowName,
		WS_OVERLAPPEDWINDOW | WS_POPUP,
		posX, posY, parameters.width, parameters.height, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	return;
}

void Window::Destroy()
{
	// Fix the display settings if leaving full screen mode.
	if (m_isFullscreen)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	// Remove the application instance.
	UnregisterClass(m_windowName, m_hinstance);
	m_hinstance = nullptr;

	return;
}

int32_t Window::GetWidth() const
{
	WINDOWINFO wi;
	wi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(m_hwnd, &wi);
	return wi.rcClient.right - wi.rcClient.left;
}

int32_t Window::GetHeight() const
{
	WINDOWINFO wi;
	wi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(m_hwnd, &wi);
	return wi.rcClient.bottom - wi.rcClient.top;
}

int32_t Window::GetX() const
{
	WINDOWINFO wi;
	wi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(m_hwnd, &wi);
	return wi.rcWindow.left;
}

int32_t Window::GetY() const
{
	WINDOWINFO wi;
	wi.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(m_hwnd, &wi);
	return wi.rcWindow.top;
}

float Window::GetAspectRatio() const
{
	return static_cast<float>(GetWidth()) / GetHeight();
}

LPCWSTR Window::GetName() const
{
	return m_windowName;
}

HWND Window::GetHWND() const
{
	return m_hwnd;
}

bool Window::IsFullScreen() const
{
	return m_isFullscreen;
}
