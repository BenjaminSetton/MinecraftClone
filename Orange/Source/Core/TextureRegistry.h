#ifndef _TEXTUREREGISTRY_H
#define _TEXTUREREGISTRY_H

#include <unordered_map>

namespace Orange
{
	// Forward declaration
	class Texture;

	class TextureRegistry
	{
	public:

		static uint64_t AddTextureToRegistry(const Texture& tex);

		static void RemoveIdFromRegistry(const uint64_t id);

		static bool IsIdInRegistry(const uint64_t id);

		static Texture GetTextureWithId(const uint64_t id);

	private:

		static std::unordered_map<uint64_t, Texture> m_registry;

	};
}

#endif