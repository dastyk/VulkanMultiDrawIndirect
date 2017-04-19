#pragma once

#include "Renderer.h"
#include <fstream>
#include "CPUTimer.h"
#include "CameraManager.h"

class Scene
{
	struct Object
	{
		Renderer::MeshHandle mesh;
		Renderer::TextureHandle texture;
		Renderer::TranslationHandle translation;
	
	};
public:
	Scene(Renderer& renderer, float width, float height);
	~Scene();


	const void Init();
	const void Frame(float dt);
	const void Shutdown();

	int StartTest(const char* outfile);

	CameraManager _camera;
private:
	const void _CreateObject(const char* mesh, const char* texture, const DirectX::XMMATRIX& translation);
private:
	Renderer& _renderer;

	std::vector<Object> _objects;

	CPUTimer _timer;
	std::ofstream out;
};

