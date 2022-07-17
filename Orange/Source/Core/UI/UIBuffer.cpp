
#include <queue>

#include "UIBuffer.h"
#include "UITypes.h"
#include "../../Utility/Utility.h"

namespace Orange
{

	std::queue<UIDrawCommand> UIBuffer::m_drawCommands = std::queue<UIDrawCommand>();

	std::queue<UIVertex> UIBuffer::m_vertexQueue = std::queue<UIVertex>();

	void UIBuffer::PushDrawCommand(const UIDrawCommand drawCommand)
	{
		m_drawCommands.emplace(drawCommand);
	}

	const Orange::UIDrawCommand& UIBuffer::PeekDrawCommand()
	{
		return m_drawCommands.front();
	}

	void UIBuffer::PopDrawCommand()
	{
		m_drawCommands.pop();
	}

	const bool UIBuffer::IsDrawCommandQueueEmpty()
	{
		return m_drawCommands.empty();
	}

	void UIBuffer::PushUIVertex(const UIVertex vert)
	{
		m_vertexQueue.emplace(vert);
	}

	const Orange::UIVertex& UIBuffer::PeekUIVertex()
	{
		return m_vertexQueue.front();
	}

	void UIBuffer::PopUIVertex()
	{
		m_vertexQueue.pop();
	}

	const bool UIBuffer::IsVertexQueueEmpty()
	{
		return m_vertexQueue.empty();
	}

}


