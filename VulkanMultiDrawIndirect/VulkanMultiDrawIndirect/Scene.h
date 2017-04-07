#pragma once

#include "Renderer.h"
#include "Camera.h"
#include <fstream>
#include "CPUTimer.h"

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

	int StartTest(const char* outfile);

	Camera _camera;
private:
	const void _CreateObject(const char* mesh, const char* texture, const glm::mat4& translation);
private:
	Renderer& _renderer;

	std::vector<Object> _objects;

	CPUTimer _timer;
	std::ofstream out;
};

