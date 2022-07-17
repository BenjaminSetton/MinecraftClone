#ifndef _WINDOW_H
#define _WINDOW_H
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	TODO:
// 
//		This Window class uses Windows API, but there is no differentiation between a generic, platform-agnostic Window class
//		and a platform-specific Window class. FileSystem is platform-agnostic, so refer to that when it comes time to refactor
//		this class. PLEASE REWORK!
// 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Orange::Vec2;

enum WindowAlignment
{
	TopLeft = 0,
	Centered,
	BottomRight,
	Custom
};

struct WindowParameters
{
	LPCWSTR name;
	int32_t width, height, x, y;
	bool fullScreen;
	WindowAlignment alignment;
};

// TODO: Switch over all window logic from Application.h to this class
class Window
{
public:

	Window() = default;
	Window(const Window& window) = default;
	~Window() = default;

	void Create(const WindowParameters& parameters);
	void Destroy();

	Orange::Vec2 GetSize() const;
	Orange::Vec2 GetPosition() const;
	float GetAspectRatio() const;
	LPCWSTR GetName() const;
	HWND GetHWND() const;
	bool IsFullScreen() const;

private:

	HWND m_hwnd;
	LPCWSTR m_windowName;
	HINSTANCE m_hinstance;
	bool m_isFullscreen;
};

#endif