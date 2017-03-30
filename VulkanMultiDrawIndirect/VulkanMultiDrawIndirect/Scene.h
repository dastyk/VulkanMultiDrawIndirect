#pragma once

#include "Renderer.h"
#include "Camera.h"

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
	const void Frame(float dt);
	const void Shutdown();

private:
	const void _CreateObject(const char* mesh, const char* texture);
private:
	Renderer& _renderer;

	std::vector<Object> _objects;


	Camera _camera;
	
};

