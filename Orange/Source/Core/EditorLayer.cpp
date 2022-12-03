#include "../Misc/pch.h"

#include "Application.h"
#include "DefaultBlockShader.h"
#include "EditorLayer.h"
#include "../Utility/HeapOverrides.h"
#include "../Utility/MathTypes.h"
#include "Panels/MainViewportPanel.h"
#include "UI/UIHelper.h"
#include "../Utility/Utility.h"

using namespace Orange;

Panel* EditorLayer::m_panels[EditorLayer_NumberOfPanels] = { nullptr, };

// DEBUG - PLEASE REMOVE
Texture globalTex;

// TEMP TEST
bool EditorLayer::tempTest = false;
float EditorLayer::tempTest2 = 1.0f;

void EditorLayer::Initialize()
{
	Vec2 windowSize = Application::Handle->GetMainWindow()->GetSize();
	Vec2 windowPos = Application::Handle->GetMainWindow()->GetPosition();

	// Initialize all the panels
	PanelPropertyFlags panelFlags = 0;
	//panelFlags |= PanelPropertyFlags_NoDock;
	//panelFlags |= PanelPropertyFlags_NoMove;
	//panelFlags |= PanelPropertyFlags_NoResize;
	panelFlags |= PanelPropertyFlags_NoCollapse;
	panelFlags |= PanelPropertyFlags_NoTitleBar;

	//Panel* windowPanel =\s*OG_NEW\s+Panel();
	//windowPanel->Create("WindowPanel", windowSize, { 0.0f, 0.0f }, panelFlags);

	Panel* leftPanel = OG_NEW Panel();
	Vec2 leftPanelSize = { 200.0f, windowSize.y * 0.8f };
	Vec2 leftPanelPos = Application::Handle->GetMainWindow()->GetPosition();
	leftPanel->Create("LeftPanel", leftPanelSize, leftPanelPos, panelFlags);

	Panel* rightPanel = OG_NEW Panel();
	Vec2 rightPanelSize = { 200.0f, windowSize.y * 0.8f };
	Vec2 rightPanelPos = { windowPos.x + windowSize.x - rightPanelSize.x, windowPos.y };
	rightPanel->Create("RightPanel", rightPanelSize, rightPanelPos, panelFlags);

	Panel* bottomPanel = OG_NEW Panel();
	Vec2 bottomPanelSize = { windowSize.x, ceil(windowSize.y * 0.2f) };
	Vec2 bottomPanelPos = { windowPos.x, windowPos.y + (windowSize.y * 0.8f) };
	bottomPanel->Create("BottomPanel", bottomPanelSize, bottomPanelPos, panelFlags);

	Panel* mainViewportPanel = OG_NEW Panel();
	Vec2 mainViewportPanelSize = { windowSize.x - leftPanelSize.x - rightPanelSize.x, windowSize.y - bottomPanelSize.y };
	Vec2 mainViewportPanelPos = { windowPos.x + leftPanelSize.x, windowPos.y };
	mainViewportPanel->Create("MainViewportPanel", mainViewportPanelSize, mainViewportPanelPos, panelFlags);

	// Create all the panel components and attach them to the panels
	MainViewportPanel* viewportPanelComponent = OG_NEW MainViewportPanel(mainViewportPanel);
	mainViewportPanel->AddComponent(viewportPanelComponent);

	// Push back all the panels
	m_panels[static_cast<int>(PanelLocation::LEFT)] = leftPanel;
	m_panels[static_cast<int>(PanelLocation::RIGHT)] = rightPanel;
	m_panels[static_cast<int>(PanelLocation::BOTTOM)] = bottomPanel;
	m_panels[static_cast<int>(PanelLocation::CENTER)] = mainViewportPanel;


	// SUPER TEMP - DEBUG
	const uint32_t w = 256;
	const uint32_t h = 256;
	uint32_t* data = new uint32_t[w * h];
	for (uint32_t height = 0; height < h; height++)
	{
		for (uint32_t width = 0; width < w; width++)
		{
			//uint32_t widthNorm = (uint32_t)(((float)width / w) * 0xFF);
			//uint32_t heightNorm = (uint32_t)(((float)height / h) * 0xFF);
			//uint32_t currColor = 0xFF000000;
			//currColor |= heightNorm << 8;
			//currColor |= widthNorm << 0;
			//currColor |= 0x000000FF << 16;
			uint32_t flatIx = height * w + width;
			data[flatIx] = 0xFFFFFFFF;
		}
	}
	TextureSpecs specs;
	specs.dimensions = TextureDimensions::TWO;
	specs.format = TextureFormat::RGBA_32;
	specs.size = Vec3(w, h, 0.0f);

	globalTex = Texture(specs, (void*)data);
}

void EditorLayer::Shutdown()
{
	// Delete all the panels
	for (uint32_t i = 0; i < EditorLayer_NumberOfPanels; i++)
	{
		delete m_panels[i];
		m_panels[i] = nullptr;
	}
}

Panel* EditorLayer::GetPanel(const PanelLocation whichPanel)
{
	return m_panels[static_cast<int>(whichPanel)];
}

void EditorLayer::Update(const float& dt)
{
	UNUSED(dt);

	// TEMP - PLEASE REMOVE
	static float totalTime = 0.0f;
	totalTime += dt;

	UI::Begin("Debug Container");
	UI::Text("Mouse Position: %f, %f", Input::GetMousePosition().x, Input::GetMousePosition().y);
	UI::Text("ActiveID: %u", UI::GetContextRO()->activeID);
	UI::Text("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Suspendisse in malesuada sem. Phasellus leo nunc, consequat id interdum a, vehicula sed enim.");
	UI::Text("Some other text...testing interaction");
	UI::Checkbox(&tempTest, "Testing new checkbox! %i", (int)tempTest);
	UI::Slider(&tempTest2, 0.0f, 1.0f, "Transparency", tempTest2);


	// TEST - DEBUG
	/*uint32_t* data = (uint32_t*)globalTex.GetData();
	uint32_t w = globalTex.GetSpecs().size.x;
	uint32_t h = globalTex.GetSpecs().size.y;
	for (uint32_t height = 0; height < h; height++)
	{
		for (uint32_t width = 0; width < w; width++)
		{
			uint32_t flatIx = height * w + width;
			uint32_t currColor = data[flatIx];
			uint32_t modifiedAlpha = (uint32_t)(tempTest2 * 0xFF);
			data[flatIx] = currColor & 0x00FFFFFF;
			data[flatIx] |= (modifiedAlpha << 24);
		}
	}*/


	UI::Image(globalTex);
	UI::End();

	//UI::Begin("Other Debug Container");
	//UI::Text("ActiveID: %u", UI::GetContext()->activeID);
	//UI::Text("HoveredID: %u", UI::GetContext()->hoveredID);
	//UI::Text("DistToCenter: %2.2f, %2.2f", UI::GetContext()->distToCenterOfActiveContainer.x, UI::GetContext()->distToCenterOfActiveContainer.y);
	//UI::End();
}

void EditorLayer::Draw()
{
	// Create a dock space over the entire viewport so we can dock all the panels. This means that
	// resizing any panel will be taken care of by ImGui
	//ImGuiID mainViewportID = ImGui::DockSpaceOverViewport(nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
	//ImGui::DockBuilderAddNode(m_panels[static_cast<int>(PanelLocation::LEFT)]->GetID(), ImGuiDockNodeFlags_None);

	// Comment out for now, so I can test my font renderer
	//for (auto panel : m_panels)
	//{
	//	panel->Draw();
	//}
}

