#ifndef _GAME_H
#define _GAME_H

class Player;

class Game
{
public:

	Game() = delete;
	Game(const Game& other) = delete;
	~Game() = delete;

	static void Initialize();

	static void Shutdown();

	static Player* GetPrimaryPlayer();

	static void SetPrimaryPlayer(Player* player);

private:

	static Player* m_primaryPlayer;

};

#endif