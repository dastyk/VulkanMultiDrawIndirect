#include "Scene.h"



Scene::Scene(Renderer& renderer) : _renderer(renderer)
{
}


Scene::~Scene()
{
}

const void Scene::Init()
{
	Renderer::MeshHandle deer = _renderer.CreateMesh("../Assets/Meshes/deer-obj.obj");
	Renderer::TextureHandle deert = _renderer.CreateTexture("../Assets/Textures/deer texture.tga");


	_CreateObject("../Assets/Meshes/deer-obj.obj", "../Assets/Textures/deer texture.tga");

	//renderer.CreateMesh("asdasd", "asdasd");
	//_renderer.Submit(idididi, ididid, ididi )


	Renderer::TextureHandle test = _renderer.CreateTexture("../Assets/Textures/testimage.png");

}

const void Scene::Start()
{
	_renderer.Render();
}

const void Scene::Shutdown()
{
	return void();
}

const void Scene::_CreateObject(const char * mesh, const char * texture)
{
	_objects.push_back({
		_renderer.CreateMesh(mesh),
		_renderer.CreateTexture(texture),
		0
	});

}
