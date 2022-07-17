#ifndef _UIMANAGER_H
#define _UIMANAGER_H

#include <queue>

#include "../../Utility/MathTypes.h"

namespace Orange
{
	// Forward declarations
	struct UIVertex;
	struct UIDrawCommand;

	class UIBuffer
	{
	public:

		static void PushDrawCommand(const UIDrawCommand drawCommand);
		static const UIDrawCommand& PeekDrawCommand();
		static void PopDrawCommand();
		static const bool IsDrawCommandQueueEmpty();

		static void PushUIVertex(const UIVertex vert);
		static const UIVertex& PeekUIVertex();
		static void PopUIVertex();
		static const bool IsVertexQueueEmpty();

	private:

		static std::queue<UIDrawCommand> m_drawCommands;
		static std::queue<UIVertex> m_vertexQueue;

	};
}

#endif