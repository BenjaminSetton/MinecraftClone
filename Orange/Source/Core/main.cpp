#include "../Misc/pch.h"

#include "Application.h"
#include "../Utility/Utility.h"

void CreateConsole();
void DestroyConsole();


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR pScmdline, _In_ int iCmdshow)
{
	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(pScmdline);
	UNUSED(iCmdshow);

#ifdef OG_DEBUG
	CreateConsole();
#endif

	// 1. Declare and instantiate an application
	Orange::Application* app = new Orange::Application;

	// 2. Initialize the application
	bool hasInitialized = app->Initialize();
	if (!hasInitialized) return 0;

	// 3. Run the application
	app->Run();

	// 4. Shutdown and cleanup the app before exiting
	app->Shutdown();


#ifdef OG_DEBUG
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