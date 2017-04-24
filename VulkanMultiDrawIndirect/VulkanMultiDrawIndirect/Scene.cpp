#include "Scene.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SDL.h>
#include <ConsoleThread.h>
using namespace DirectX;

Scene::Scene(Renderer& renderer, float width, float height) : _renderer(renderer), _camera(width, height)
{

}


Scene::~Scene()
{
}

const void Scene::Init()
{
	_renderer.SetProjectionMatrix(_camera.GetProj());
	_camera.TranslateActiveCamera(100.0f, 0.0f, -80.0f);

	_CreateObject("../Assets/Meshes/deer-obj.obj", "../Assets/Textures/deer texture.tga", XMMatrixTranslation(-20, 0, 0));

	auto deer = _objects[0];
	for (int i = 0; i < 100; i++)
	{
		for (int j = 0; j < 100; j++)
		{

			deer.translation = _renderer.CreateTranslation(XMMatrixTranslation(20.0f * i, 0, 16.0f * j));
			_objects.push_back(deer);
		}
	}

	//_CreateObject("../Assets/Meshes/bear-obj.obj", "../Assets/Textures/bear.tga");
	//_CreateObject("../Assets/Meshes/boar-obj.obj", "../Assets/Textures/boar.tga");
	_CreateObject("../Assets/Meshes/cube2.obj", "../Assets/Textures/cube2.png", XMMatrixIdentity());


	for (auto& o : _objects)
		_renderer.Submit(o.mesh, o.texture, o.translation);



}

const void Scene::Frame(float dt)
{

	_renderer.SetViewMatrix(_camera.GetView());
	_renderer.Render();


	//printf("MS: %f\n", _timer.GetTime("Frame"));
}

const void Scene::Shutdown()
{
	return void();
}


const void Scene::_CreateObject(const char * mesh, const char * texture, const XMMATRIX& translation)
{
	_objects.push_back({
		_renderer.CreateMesh(mesh),
		_renderer.CreateTexture(texture),
		_renderer.CreateTranslation(translation)
	});

}
