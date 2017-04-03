#include "Scene.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
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


	_CreateObject("../Assets/Meshes/deer-obj.obj", "../Assets/Textures/deer texture.tga", glm::translate(glm::mat4(), glm::vec3(-20, 0, 0)));
	//_CreateObject("../Assets/Meshes/bear-obj.obj", "../Assets/Textures/bear.tga");
	//_CreateObject("../Assets/Meshes/boar-obj.obj", "../Assets/Textures/boar.tga");
	_CreateObject("../Assets/Meshes/cube2.obj", "../Assets/Textures/cube2.png", glm::mat4());

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

const void Scene::_CreateObject(const char * mesh, const char * texture, const glm::mat4& translation)
{
	_objects.push_back({
		_renderer.CreateMesh(mesh),
		_renderer.CreateTexture(texture),
		_renderer.CreateTranslation(translation)
	});

}
