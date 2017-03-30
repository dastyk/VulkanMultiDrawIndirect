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
	//renderer.CreateMesh("asdasd", "asdasd");
	//_renderer.Submit(idididi, ididid, ididi )
}

const void Scene::Start()
{
	_renderer.Render();
}

const void Scene::Shutdown()
{
	return void();
}
