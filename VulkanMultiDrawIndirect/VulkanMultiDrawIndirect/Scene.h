#pragma once

#include "Renderer.h"

#include "CameraManager.h"

class Scene
{
	struct Object
	{
		Renderer::MeshHandle mesh;
		Renderer::TextureHandle texture;
		Renderer::TranslationHandle translationHandle;
		Renderer::BoundingHandle boundingHandle;
		DirectX::XMFLOAT4X4 translation;
	};
public:
	Scene(Renderer& renderer, float width, float height);
	~Scene();


	const void Init();
	const void Frame(float dt);
	const void Shutdown();

	CameraManager _camera;
private:
	const void _CreateObject(const char* mesh, const char* texture, const DirectX::XMMATRIX& translation);

	int _StartTest(const char* outfile);
	void _EndTest();
private:
	Renderer& _renderer;
	CPUTimer _timer;

	uint32_t _frameCount;
	float _frameTimes;
	bool _testRunning;
	std::vector<Object> _objects;
	std::vector<float> _objectOffsetAngles;

	std::ofstream out;
};

