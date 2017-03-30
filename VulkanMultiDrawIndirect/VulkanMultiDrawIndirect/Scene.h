#pragma once

#include "Renderer.h"

class Scene
{
	struct Object
	{
		Renderer::MeshHandle mesh;
		Renderer::TextureHandle texture;
		Renderer::TranslationHandle translation;

	};
public:
	Scene(Renderer& renderer);
	~Scene();


	const void Init();
	const void Start();
	const void Shutdown();

private:
	const void _CreateObject(const char* mesh, const char* texture);

private:
	Renderer& _renderer;

	std::vector<Object> _objects;
};

