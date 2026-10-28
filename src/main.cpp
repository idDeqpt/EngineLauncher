#include <filesystem>
#include <iostream>
#include <utility>
#include <cstdlib>

template <typename... Args>
void exec(Args&&... args)
{
	std::string cmd;

	auto append = [&cmd](const auto& arg) {
		if (!cmd.empty())
			cmd.push_back(' ');
		cmd += arg;
	};

	(append(std::forward<Args>(args)), ...);

	std::cout << "> " << cmd << std::endl;

	(void)std::system(cmd.c_str());
}


int main(int argc, char* argv[])
{
	for (unsigned int i = 0; i < argc; i++)
		std::cout << argv[i] << std::endl;

	if (argc < 3) return 1;

	if (std::string(argv[1]) == "build")
	{
		std::filesystem::path proj_path = std::filesystem::canonical(argv[2]);
		std::filesystem::path root = std::filesystem::canonical(argv[0]).parent_path();
		std::filesystem::path work = root / "work";
		std::filesystem::path out  = root / "runtime";
		std::string cfg = "Release";
		std::string engine_install = (work/"Engine/install").string();
		std::string DCMAKE_BUILD_TYPE = "-DCMAKE_BUILD_TYPE=" + cfg;
		std::string DCMAKE_PREFIX_PATH = "-DCMAKE_PREFIX_PATH=" + engine_install;
		std::string DCMAKE_INSTALL_PREFIX = "-DCMAKE_INSTALL_PREFIX=" + engine_install;

		std::cout << "proj_path: " << proj_path.string() << std::endl;
		std::cout << "root: " << root.string() << std::endl;
		std::cout << "work: " << work.string() << std::endl;
		std::cout << "out: " << out.string() << std::endl;
		std::cout << std::endl;

		//Engine
		exec("cmake", "-DENGINE_DIR=" + (work/"Engine").string(), "-P", (root/"export/Engine/fetch.cmake").string());
		exec("cmake", "-S", (work/"Engine").string(), "-B", (work/"Engine/build").string(), DCMAKE_INSTALL_PREFIX, DCMAKE_BUILD_TYPE);
		exec("cmake", "--build", (work/"Engine/build").string(), "--config", cfg);
		exec("cmake", "--install", (work/"Engine/build").string(), "--config", cfg);

		//App
		exec("cmake", "-S", (root/"export/App").string(), "-B", (work/"App").string(), DCMAKE_PREFIX_PATH, DCMAKE_BUILD_TYPE);
		exec("cmake", "--build",   (work/"App").string(), "--config", cfg);
		exec("cmake", "--install", (work/"App").string(), "--config", cfg, "--prefix", out.string());

		//Project
		exec("cmake", "-S", (root/"export/Project").string(), "-B", (work/"Project").string(), DCMAKE_PREFIX_PATH, DCMAKE_BUILD_TYPE, "-DPROJECT_DIR=" + proj_path.string());
		exec("cmake", "--build",   (work/"Project").string(), "--config", cfg);
		exec("cmake", "--install", (work/"Project").string(), "--config", cfg, "--prefix", out.string());

		//Runtime
		exec("cmake", "--install", (work/"Engine/build").string(), "--config", cfg, "--prefix", out.string(), "--component", "runtime");
	}

	return 0;
}