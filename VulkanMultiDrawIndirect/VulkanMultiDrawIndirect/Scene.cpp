#include "Scene.h"



Scene::Scene(Renderer& renderer) : _renderer(renderer)
{
}


Scene::~Scene()
{
}

const void Scene::Init()
{
	_CreateObject("../Assets/Meshes/deer-obj.obj", "../Assets/Textures/deer texture.tga");

	for (auto& o : _objects)
		_renderer.Submit(o.mesh, o.texture, o.translation);

}

const void Scene::Start()
{
	//_renderer.Submit(_deer);
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
