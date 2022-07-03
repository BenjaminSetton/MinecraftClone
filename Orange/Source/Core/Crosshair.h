#ifndef _CROSSHAIR_H
#define _CROSSHAIR_H

namespace Orange
{

	class Crosshair
	{
	public:

		Crosshair() = delete;
		Crosshair(const Crosshair& other) = delete;
		~Crosshair() = delete;

		static void Update(const float dt);

		const float GetScale();
		void SetScale(const float scale);

	private:

		static void PushQuadsToManager();

		static float m_scale;
	};

}


#endif
