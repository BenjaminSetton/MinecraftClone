#include "../Misc/pch.h"
#include "Game.h"
#include "../Utility/HeapOverrides.h"
#include "Player.h"

namespace Orange
{
	Player* Game::m_primaryPlayer = nullptr;

	void Game::Initialize()
	{
		// Create a primary player
		m_primaryPlayer = OG_NEW Player();
	}

	void Game::Shutdown()
	{
		if (m_primaryPlayer)
		{
			delete m_primaryPlayer;
			m_primaryPlayer = nullptr;
		}
	}

	Player* Game::GetPrimaryPlayer() { return m_primaryPlayer; }

	void Game::SetPrimaryPlayer(Player* player) { m_primaryPlayer = player; }

	void Game::Update(const float dt)
	{
		// Update the primary player
		m_primaryPlayer->Update(dt);
	}
}