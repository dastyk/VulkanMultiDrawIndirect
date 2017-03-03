#pragma once

#include "Renderer.h"

class Scene
{
public:
	Scene(Renderer& renderer);
	~Scene();


	const void Init();
	const void Start();
	const void Shutdown();

private:
	Renderer& _renderer;
};

