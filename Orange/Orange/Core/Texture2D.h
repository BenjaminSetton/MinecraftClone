#ifndef _TEXTURE2D_H
#define _TEXTURE2D_H

#include "MathTypes.h"
#include "Texture.h"

class Texture2D : public Texture
{
public:

	Texture2D();
	Texture2D(const Texture2D& other) = default;
	~Texture2D() = default;

	void Create(void* data, const Orange::Vec2& dimensions);

	Orange::Vec2 GetDimensions() const;

private:

	virtual void OverridePlease() override;

	Orange::Vec2 m_dimensions;

};

#endif
