#include <Engine/System/DynamicLibrary.hpp>
#include <Engine/Core/Engine.hpp>
#include <Engine/Core/Node.hpp>
#include <filesystem>
#include <cstdio>

using create_root_fn  = eng::core::Node*(*)();

int main(int argc, char** argv)
{
	std::filesystem::path root = std::filesystem::canonical(argv[0]).parent_path();

	eng::sys::DynamicLibrary library;
	if (!library.open((root/"lib").string(), "App"))
	{
		std::fprintf(stderr, library.getLastError().c_str());
		return 1;
	}

	auto create  = reinterpret_cast<create_root_fn>(library.getSymbol("engine_create_root"));
	if (!create)
	{
		std::fprintf(stderr, library.getLastError().c_str());
		return 2;
	}

	{
		eng::core::Engine engine{std::unique_ptr<eng::core::Node>(create())};
		engine.setup();
		engine.mainLoop();
	}

	return 0;
}