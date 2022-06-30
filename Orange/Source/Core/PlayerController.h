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

	static DirectX::XMFLOAT3 CheckForHorizontalCollision(AABB& newPos, const AABB& prevPos);
	static DirectX::XMFLOAT3 CheckForVerticalCollision(AABB& newPos, const AABB& prevPos);

};


#endif // _PLAYERCONTROLLER_H