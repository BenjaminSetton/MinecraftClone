#include "pch.h"

#include "Application.h"

void CreateConsole();
void DestroyConsole();


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR pScmdline, _In_ int iCmdshow)
{
#ifdef _DEBUG
	CreateConsole();
#endif

	// 1. Declare and instantiate an application
	Application* app = new Application;

	// 2. Initialize the application
	bool hasInitialized = app->Initialize();
	if (!hasInitialized) return 0;

	// 3. Run the application
	app->Run();

	// 4. Shutdown and cleanup the app before exiting
	app->Shutdown();


#ifdef _DEBUG
	DestroyConsole();
#endif

	return 0;
}

void CreateConsole()
{
	AllocConsole();
	FILE* new_std_in_out;
	freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
	freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
}

void DestroyConsole()
{
	FreeConsole();
}