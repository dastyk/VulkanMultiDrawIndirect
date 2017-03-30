#include "Scene.h"



Scene::Scene(Renderer& renderer) : _renderer(renderer)
{
}


Scene::~Scene()
{
}

const void Scene::Init()
{
	_deer = _renderer.CreateMesh("../Assets/Meshes/deer-obj.obj");
	Texture2D* deerTex = _renderer.CreateTexture("../Assets/Textures/deer texture.tga");
	Texture2D* test = _renderer.CreateTexture("../Assets/Textures/testimage.png");
	//renderer.CreateMesh("asdasd", "asdasd");
	//_renderer.Submit(idididi, ididid, ididi )
}

const void Scene::Start()
{
	_renderer.Submit(_deer);
	_renderer.Render();
}

const void Scene::Shutdown()
{
	return void();
}
