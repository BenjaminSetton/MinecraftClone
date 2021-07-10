#ifndef _D3D_H
#define _D3D_H

// Library (linker)
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

// D3D11 Includes
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>

class D3D
{
public:

	D3D();
	~D3D();
	D3D(const D3D&);

	void Shutdown();

private:
};

#endif

