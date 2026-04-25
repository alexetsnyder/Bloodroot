#include "BloodrootApp.h"

#include <vector>

BloodrootApp::BloodrootApp()
	: window(WINDOW_WIDTH, WINDOW_HEIGHT, "Bloodroot App!"),
	  renderer(window, glfwInstance.getRequiredInstanceExtensions())
{
	window.setRenderer(&renderer);
}

void BloodrootApp::run()
{
	mainLoop();
}

void BloodrootApp::mainLoop()
{
	while (!window.windowShouldClose())
	{
		window.pollEvents();
		renderer.drawFrame(window);
	}

	renderer.waitIdle();
}
