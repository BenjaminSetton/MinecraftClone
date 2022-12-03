#include "../Misc/pch.h"

//#define ENABLE_MEMORY_TRACKING

// Allows for allocation tracking - include before everything
#include "../Utility/HeapOverrides.h"

#include "Application.h"
#include "../Utility/Utility.h"
#include "../Utility/MemoryUtilities.h"

void CreateConsole();
void DestroyConsole();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR pScmdline, _In_ int iCmdshow)
{
	UNUSED(hInstance);
	UNUSED(hPrevInstance);
	UNUSED(pScmdline);
	UNUSED(iCmdshow);

	{

#ifdef OG_DEBUG
		CreateConsole();
#endif

		Orange::Application* app = OG_NEW Orange::Application;

		bool hasInitialized = app->Initialize();
		if (!hasInitialized) return -1;

		app->Run();

		app->Shutdown();
		OG_DELETE app;

#ifdef OG_DEBUG
		DestroyConsole();
#endif

	}

#if defined (ENABLE_MEMORY_TRACKING)
	Orange::Memory::FindMemoryLeaks();
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