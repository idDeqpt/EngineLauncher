#include <Engine/Core/Engine.hpp>
#include <filesystem>

int main(int argc, char** argv)
{
	std::filesystem::path root = std::filesystem::canonical(argv[0]).parent_path();

	{
		eng::core::Engine engine;
		engine.getPluginManager().loadPlugin((root/"lib").string(), "App");
		engine.setup();
		engine.mainLoop();
	}

	return 0;
}