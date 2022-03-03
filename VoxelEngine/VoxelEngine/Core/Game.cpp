#include "../Misc/pch.h"
#include "Game.h"
#include "Player.h"

Player* Game::m_primaryPlayer = nullptr;

void Game::Initialize()
{
	// Create a primary player
	m_primaryPlayer = new Player();
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