#pragma once

#include "IRenderer.h"
#include "Camera.h"

struct AppData
{
	Core::IRenderer* renderer;
	Game::Camera* camera;
};