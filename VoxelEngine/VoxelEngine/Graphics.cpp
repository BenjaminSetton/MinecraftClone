#include "Graphics.h"

Graphics::Graphics(){}
Graphics::~Graphics(){}
Graphics::Graphics(const Graphics&){}

bool Graphics::Initialize(const int& screenWidth, const int& screenHeight, HWND windowHandle)
{
	return true;
}

void Graphics::Shutdown()
{
}

bool Graphics::Frame(const float& deltaTime)
{
	return true;
}