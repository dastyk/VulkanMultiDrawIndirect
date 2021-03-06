#include "Scene.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <SDL.h>
#include <ConsoleThread.h>
#include <iostream>
using namespace DirectX;

Scene::Scene(Renderer& renderer, float width, float height) : _renderer(renderer), _camera(width, height), _testRunning(false)
{

}


Scene::~Scene()
{
}

const void Scene::Init()
{
	srand(1337);
	_renderer.SetProjectionMatrix(_camera.GetProj());
	_camera.TranslateActiveCamera(100.0f, 0.0f, -80.0f);

	//XMMATRIX t = XMMatrixTranslation(-20, 0, 0);
//	_CreateObject("../Assets/Meshes/deer-obj.obj", "../Assets/Textures/deer texture.tga", t);

	//auto deer = _objects[0];
	//XMStoreFloat4x4(&_objects[0].translation, t);

	//t = XMMatrixTranslation(-40, 0, 0);
	//_CreateObject("../Assets/Meshes/wolf-obj.obj", "../Assets/Textures/wolf texture.tga", t);

//	auto wolf = _objects[1];
//	XMStoreFloat4x4(&_objects[1].translation, t);
	Object deer;
	deer.mesh = _renderer.CreateMesh("../Assets/Meshes/deer-obj.obj");
	deer.texture = _renderer.CreateTexture("../Assets/Textures/deer texture.tga");

	Object wolf;
	wolf.mesh = _renderer.CreateMesh("../Assets/Meshes/wolf-obj.obj");
	wolf.texture = _renderer.CreateTexture("../Assets/Textures/wolf texture.tga");

	
	auto t = XMMatrixIdentity();

	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 200; j++)
		{
			
			t = XMMatrixTranslation(40.0f * i, 0, 16.0f * j);
			XMStoreFloat4x4(&deer.translation, t);
			deer.translationHandle = _renderer.CreateTranslation(t);
			_objects.push_back(deer);

			
			t = XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(40.0f * i + 20.0f, 0, 16.0f * j);
			XMStoreFloat4x4(&wolf.translation, t);
			wolf.translationHandle = _renderer.CreateTranslation(t);
			_objects.push_back(wolf);
		}
	}

	//_CreateObject("../Assets/Meshes/bear-obj.obj", "../Assets/Textures/bear.tga");
	//_CreateObject("../Assets/Meshes/boar-obj.obj", "../Assets/Textures/boar.tga");
//	_CreateObject("../Assets/Meshes/cube2.obj", "../Assets/Textures/cube2.png", XMMatrixIdentity());


	for (auto& o : _objects)
		_renderer.Submit(o.mesh, o.texture, o.translationHandle);

	_objectOffsetAngles.resize(_objects.size());
	for (int i = 0; i < _objects.size(); ++i)
	{
		_objectOffsetAngles[i] = i * XMConvertToRadians(10.0f);
	}

	DebugUtils::DebugConsole::Command_Structure testStart =
	{
		this,
		[](void* userData, int argc, char** argv) {

		auto& scene = *(Scene*)userData;
		int res;
		char* filename = "log.log";
		DebugUtils::GetArg("-o", &filename, argc, argv);
		res = scene._StartTest(filename);

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

	DebugUtils::DebugConsole::Command_Structure updateSwitch =
	{
		this,
		[](void* userData, int argc, char** argv) {
			if (argc < 2)
				return;

			Scene* scene = (Scene*)userData;

			if (DebugUtils::GetArg("on", nullptr, argc, argv))
			{
				scene->_update = true;
			}
			else if (DebugUtils::GetArg("off", nullptr, argc, argv))
			{
				scene->_update = false;
			}
			else
			{
				printf("Usage: update [on/off]\n");
			}
		},
		[](void* userData, int argc, char** argv) {
			printf("Usage:\n");
			printf("update [on/off]\n");
		},
		"update",
		"Updates translations of scene meshes."
	};

	DebugUtils::ConsoleThread::AddCommand(&updateSwitch);
}

const void Scene::Frame(float dt)
{
	_timer.TimeStart("Frame");

	if (_update)
	{
		//for (uint32_t i = 0; i < 1000; i++)
		//{
		//	auto& o = _objects[rand()%_objects.size()];
		//	XMMATRIX t = XMLoadFloat4x4(&o.translation);
		//	t *= XMMatrixTranslation((rand() % 200 - 100)/100.0f*dt, (rand() % 200 - 100) / 100.0f*dt, (rand() % 200 - 100) / 100.0f*dt);
		//	XMStoreFloat4x4(&o.translation, t);
		//	_renderer.UpdateTranslation(t, o.translationHandle);
		//}
		for (uint32_t i = 0; i < _objects.size(); ++i)
		{
			auto& o = _objects[i];
			_objectOffsetAngles[i] += XM_2PI * dt;
			o.translation._42 = 10.0f * sinf(_objectOffsetAngles[i]);
			_renderer.UpdateTranslation(XMLoadFloat4x4(&o.translation), o.translationHandle);
		}
	}

	_renderer.SetViewMatrix(_camera.GetView());
	_renderer.Render();
	_timer.TimeEnd("Frame");
	if (_testRunning)
	{
		_frameTimes += _timer.GetTime("Frame");
		_frameCount++;

		const uint32_t targetFrameCount = 100;

		if (_frameCount > targetFrameCount)
		{
			_EndTest();
		}
		else
		{
			printf("\rMeasuring: %.0f%%", 100.0f * _frameCount / targetFrameCount);
			fflush(stdout);
		}
	}
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

int Scene::_StartTest(const char * outfile)
{
	out.open(outfile, std::ios::ate);
	if (!out.is_open())
		return -1;
	_renderer.StartTest();
	_frameCount = 0;
	_frameTimes = 0.0f;
	_testRunning = true;
	return 0;
}

void Scene::_EndTest()
{
	float avgTime = _frameTimes / _frameCount;

	_frameCount = 0;
	_testRunning = false;
	float rendererAvgCpu;
	float rendererAvgGpu;
	_renderer.EndTest(rendererAvgCpu, rendererAvgGpu);

	out << avgTime << " " << rendererAvgCpu << " " << rendererAvgGpu << std::endl;
	out.close();
	printf("\nTest complete, Total: %f, Recording: %f, GPU: %f, Recording-GPU: %f\n", avgTime, rendererAvgCpu, rendererAvgGpu, rendererAvgCpu- rendererAvgGpu);

}
