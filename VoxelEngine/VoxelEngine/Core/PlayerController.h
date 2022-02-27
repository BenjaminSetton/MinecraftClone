#ifndef _PLAYERCONTROLLER_H
#define _PLAYERCONTROLLER_H

#include <DirectXMath.h>

class Player;


class PlayerController
{
public:

	PlayerController() = default;
	PlayerController(const PlayerController& other) = default;
	~PlayerController() = default;

	static void Update(const float& dt, Player* player);

private:

	static void CheckForCollision( Player* player, DirectX::XMMATRIX& worldMatrix, DirectX::XMFLOAT3& prevPos, DirectX::XMVECTOR& prevFootPos, const float& dt);

};


#endif // _PLAYERCONTROLLER_H