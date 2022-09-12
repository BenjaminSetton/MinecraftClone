#ifndef _UIHELPER_H
#define _UIHELPER_H

namespace Orange
{
	// Forward declaration
	struct Vec2;
	struct UIContext;

	// This UI namespace will include all the public API that the user will be able to interact with
	namespace UI
	{
		const UIContext* GetContextRO();

		UIContext* GetContext();

		void Text(const char* format, ...);

		void Image(const Texture& tex);

		void Checkbox(bool* pBool, const char* format, ...);

		void Slider(float* value, const float min, const float max, const char* format, ...);

		void Begin(const char* containerName);

		void End();

		void BeginFrame(const float& dt);

		void EndFrame();
	}
}

#endif