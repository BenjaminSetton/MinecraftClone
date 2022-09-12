
#include <random>

#include "Texture.h"
#include "TextureRegistry.h"

static constexpr uint32_t MAX_COLLISION_ITERATIONS = 1000;
static constexpr uint32_t INVALID_TEX_ID = 0;

namespace Orange
{
	std::unordered_map<uint64_t, Texture> TextureRegistry::m_registry = std::unordered_map<uint64_t, Texture>();

	uint64_t TextureRegistry::AddTextureToRegistry(const Texture& tex)
	{
		// Generate the texture's id
		std::mt19937 mt;
		uint64_t hash = mt();

		// Make sure that we deal with hash collisions
		uint32_t repetitions = 0;
		while (m_registry.contains(hash) || hash == INVALID_TEX_ID)
		{
			if (repetitions++ > MAX_COLLISION_ITERATIONS)
			{
				OG_ASSERT_MSG(false, "Something is wrong - either we have an insane amount of registered textures or the RNG is generating bad hashes somehow");
				return INVALID_TEX_ID;
			}
			hash = mt();
		}

		// We can be sure that there will be no collisions at this point...right?
		m_registry[hash] = tex;

		return hash;
	}

	void TextureRegistry::RemoveIdFromRegistry(const uint64_t id)
	{
		m_registry.erase(id);
	}

	bool TextureRegistry::IsIdInRegistry(const uint64_t id)
	{
		return m_registry.find(id) != m_registry.end();
	}

	Texture TextureRegistry::GetTextureWithId(const uint64_t id)
	{
		auto iter = m_registry.find(id);

		// Id is not found within registry
		if (iter == m_registry.end()) return Texture();

		// Otherwise it exists, so just return it
		return iter->second;
	}
}


