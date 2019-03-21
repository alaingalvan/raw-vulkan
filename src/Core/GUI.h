#pragma once

namespace raw
{
struct GUIDesc
{
    unsigned width;
    unsigned height;
};

/**
 * A fork of imgui's existing Vulkan based UI solution to better fit this set of
 * examples:
 */
class GUI
{
  public:
    GUI();

    ~GUI();

    bool create(GUIDesc desc);

    void update(float deltaTime);

    void updateEvent(xwin::Event e);

  protected:
    void renderDrawData(ImDrawData* draw_data);

    bool createFontTexture();

    void destroyFontTexture();

    bool createDeviceObjects();

    void destroyDeviceObjects();
};
}