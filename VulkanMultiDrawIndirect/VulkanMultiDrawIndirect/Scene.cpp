#include "Scene.h"



Scene::Scene(Renderer& renderer) : _renderer(renderer)
{
}


Scene::~Scene()
{
}

const void Scene::Init()
{
	//_renderer.CreateMesh();
	Texture2D* test = _renderer.CreateTexture("../Assets/Textures/testimage.png");

}

const void Scene::Start()
{
	_renderer.Render();
}

const void Scene::Shutdown()
{
	return void();
}
