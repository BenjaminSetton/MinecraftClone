#ifndef _PLAYERCONTROLLER_H
#define _PLAYERCONTROLLER_H

#include <DirectXMath.h>

class Player;
struct AABB;

class PlayerController
{
public:

	PlayerController() = default;
	PlayerController(const PlayerController& other) = default;
	~PlayerController() = default;

	static void Update(const float& dt, Player* player);

private:

	static bool CheckForHorizontalCollision(AABB& newPos, const AABB& prevPos, const float& dt);
	static bool CheckForVerticalCollision(AABB& newPos, const AABB& prevPos, const float& dt);

};


#endif // _PLAYERCONTROLLER_H