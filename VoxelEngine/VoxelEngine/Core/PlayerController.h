#ifndef _PLAYERCONTROLLER_H
#define _PLAYERCONTROLLER_H

class Player;

class PlayerController
{
public:

	PlayerController() = default;
	PlayerController(const PlayerController& other) = default;
	~PlayerController() = default;

	static void Update(const float& dt, Player* player);

private:

};


#endif // _PLAYERCONTROLLER_H