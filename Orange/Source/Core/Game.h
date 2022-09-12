#ifndef _GAME_H
#define _GAME_H

namespace Orange
{
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

		static void Update(const float dt);

	private:

		static Player* m_primaryPlayer;

	};
}

#endif