#include "Scene.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SDL.h>
#include <ConsoleThread.h>

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

	auto deer = _objects[0];
	for (int i = 0; i < 40; i++)
	{
		for (int j = 0; j < 40; j++)
		{
			
			deer.translation = _renderer.CreateTranslation(glm::translate(glm::mat4(), glm::vec3(20.0f * i, 0, 16.0f * j)));
			_objects.push_back(deer);
		}
	}

	//_CreateObject("../Assets/Meshes/bear-obj.obj", "../Assets/Textures/bear.tga");
	//_CreateObject("../Assets/Meshes/boar-obj.obj", "../Assets/Textures/boar.tga");
	_CreateObject("../Assets/Meshes/cube2.obj", "../Assets/Textures/cube2.png", glm::mat4());


	for (auto& o : _objects)
		_renderer.Submit(o.mesh, o.texture, o.translation);


	DebugUtils::DebugConsole::Command_Structure testStart =
	{
		this,
		[](void* userData, int argc, char** argv) {

		auto& scene = *(Scene*)userData;
		int res;
		char* filename = "log.log";
		DebugUtils::GetArg("-o", &filename, argc, argv);
		res = scene.StartTest(filename);

		if (res == -1)
			printf("An error occured!\n");
		else
			printf("Test Started... Output will be saved to: %s\n", filename);





	},
		[](void* userData, int argc, char** argv) {
		printf("Usage: Measure the current rendering technique.\n");
		printf("\t-o, specify the output file.\n");

	},
		"test",
		"Starts the test sequence."
	};


	DebugUtils::ConsoleThread::AddCommand(&testStart);
	
}

const void Scene::Frame(float dt)
{
	_timer.TimeStart("Frame");
	_renderer.SetViewMatrix(_camera.view());
	_renderer.Render();
	_timer.TimeEnd("Frame");

	//printf("MS: %f\n", _timer.GetTime("Frame"));
}

const void Scene::Shutdown()
{
	return void();
}

int Scene::StartTest(const char * outfile)
{
	out.open(outfile, std::ios::ate);
	if (!out.is_open())
		return -1;


	return 0;
}

const void Scene::_CreateObject(const char * mesh, const char * texture, const glm::mat4& translation)
{
	_objects.push_back({
		_renderer.CreateMesh(mesh),
		_renderer.CreateTexture(texture),
		_renderer.CreateTranslation(translation)
	});

}
