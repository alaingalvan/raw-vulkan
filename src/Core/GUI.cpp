#include "GUI.h"

namespace raw
{
    
void GUI::updateEvent(xwin::Event e)
{
	ImGuiIO& io = ImGui::GetIO();

	if (e.type == xwin::EventType::Resize)
	{
		float x = static_cast<float>(e.data.resize.width);
		float y = static_cast<float>(e.data.resize.height);
		io.DisplaySize.x = x;
		io.DisplaySize.y = y;
		io.DisplayFramebufferScale = ImVec2(1.0, 1.0);
	}

	if (e.type == xwin::EventType::MouseInput)
	{
		xwin::MouseInputData& mid = e.data.mouseInput;

		if (mid.state == xwin::ButtonState::Pressed)
		{
			io.MouseDown[static_cast<size_t>(mid.button)] = true;
		}
		else if (mid.state == xwin::ButtonState::Released)
		{
			io.MouseDown[static_cast<size_t>(mid.button)] = false;
		}
	}

	if (e.type == xwin::EventType::MouseMoved)
	{
		xwin::MouseMoveData mmd = e.data.mouseMoved;

		io.MousePos = ImVec2(static_cast<float>(mmd.x), static_cast<float>(mmd.y));
	}

	if (e.type == xwin::EventType::MouseWheel)
	{
		xwin::MouseWheelData& mwd = e.data.mouseWheel;
		io.MouseWheel += static_cast<float>(mwd.delta);
	}

	if (e.type == xwin::EventType::Keyboard)
	{
		xwin::KeyboardData& kd = e.data.keyboard;

		if (kd.state == xwin::ButtonState::Pressed)
		{
			// map xwin events to win32 since IMGUI models after win32
			io.KeysDown[0] = 1;
		}
		else if (kd.state == xwin::ButtonState::Released)
		{
			io.KeysDown[0] = 0;
		}
	}
}

void GUI::update(float deltaTime)
{
	ImGuiIO& io = ImGui::GetIO();

	io.DeltaTime = deltaTime;

	ImGui::EndFrame();

	ImGui::Render();

	renderDrawData(ImGui::GetDrawData());

	// Start a new UI frame
	ImGui::NewFrame();
}
}
