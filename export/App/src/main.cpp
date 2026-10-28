#include <Engine/Core/Engine.hpp>
#include <Engine/Core/Node.hpp>
#include <filesystem>
#include <cstdio>
#include <dlfcn.h>

using create_root_fn  = eng::core::Node*(*)();

int main(int argc, char** argv)
{
	std::filesystem::path root = std::filesystem::canonical(argv[0]).parent_path();
	std::string plugin_path = root/"lib/libApp.so";

	void* handle = dlopen(plugin_path.c_str(), RTLD_NOW | RTLD_LOCAL);
	if (!handle)
	{
		std::fprintf(stderr, "dlopen: %s\n", dlerror());
		return 1;
	}

	auto create  = (create_root_fn) dlsym(handle, "engine_create_root");
	if (!create)
	{
		std::fprintf(stderr, "factory symbols missing\n");
		return 2;
	}

	{
		eng::core::Engine engine{std::unique_ptr<eng::core::Node>(create())};
		engine.setup();
		engine.mainLoop();
	}

	dlclose(handle);
}