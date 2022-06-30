#include "../Misc/pch.h"
#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(DirectX::XMFLOAT3 dir /*= { 0.0f, -1.0f, 0.0f }*/, DirectX::XMFLOAT4 color /*= { 1.0f, 1.0f, 1.0f, 1.0f }*/)
	: m_dir(dir), m_color(color)
{

}

const DirectX::XMFLOAT3 DirectionalLight::GetDirection() { return m_dir; }

void DirectionalLight::SetDirection(const DirectX::XMFLOAT3 dir) { m_dir = dir; }

const DirectX::XMFLOAT4 DirectionalLight::GetColor() { return m_color; }

void DirectionalLight::SetColor(const DirectX::XMFLOAT4 color) { m_color = color; }
