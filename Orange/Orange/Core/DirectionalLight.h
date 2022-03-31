#ifndef _DIRECTIONAL_LIGHT_H
#define _DIRECTIONAL_LIGHT_H

#include <DirectXMath.h>

class DirectionalLight
{
public:

	DirectionalLight(DirectX::XMFLOAT3 dir = { 0.0f, -1.0f, 0.0f }, DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
	DirectionalLight(const DirectionalLight& other) = default;
	~DirectionalLight() = default;

	const DirectX::XMFLOAT3 GetDirection();
	void SetDirection(const DirectX::XMFLOAT3 dir);

	const DirectX::XMFLOAT4 GetColor();
	void SetColor(const DirectX::XMFLOAT4 color);

private:

	DirectX::XMFLOAT3 m_dir;
	DirectX::XMFLOAT4 m_color;

};

#endif