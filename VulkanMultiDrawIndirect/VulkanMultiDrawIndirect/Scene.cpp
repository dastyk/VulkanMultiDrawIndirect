#include "Scene.h"

#include <SDL.h>

Scene::Scene(Renderer& renderer) : _renderer(renderer)
{
}


Scene::~Scene()
{
}

const void Scene::Init()
{
	_camera.setViewportAspectRatio(_renderer.GetAspect());
	glm::mat4 p = _camera.projection();
	p[1][1] = -p[1][1];
	_renderer.SetProjectionMatrix(p);


	_CreateObject("../Assets/Meshes/deer-obj.obj", "../Assets/Textures/deer texture.tga", glm::vec3(-10, 0, 0));
	//_CreateObject("../Assets/Meshes/bear-obj.obj", "../Assets/Textures/bear.tga");
	//_CreateObject("../Assets/Meshes/boar-obj.obj", "../Assets/Textures/boar.tga");
	_CreateObject("../Assets/Meshes/wolf-obj.obj", "../Assets/Textures/wolf texture.tga", glm::vec3(0, 0, 0));

	for (auto& o : _objects)
		_renderer.Submit(o.mesh, o.texture, o.translation);


	
}

const void Scene::Frame(float dt)
{
	_renderer.SetViewMatrix(_camera.view());
	_renderer.Render();
}

const void Scene::Shutdown()
{
	return void();
}

const void Scene::_CreateObject(const char * mesh, const char * texture, const glm::vec3& translation)
{
	_objects.push_back({
		_renderer.CreateMesh(mesh),
		_renderer.CreateTexture(texture),
		_renderer.CreateTranslation(translation)
	});

}
