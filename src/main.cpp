#include <iostream>


void exec(std::string command)
{
	std::cout << "> " << command << std::endl;
	system(command.c_str());
}


int main(int argc, char* argv[])
{
	for (unsigned int i = 0; i < argc; i++)
		std::cout << argv[i] << std::endl;

	if (argc < 3) return 1;

	if (std::string(argv[1]) == "build")
	{
		std::string proj_path = argv[2];
		std::cout << proj_path << std::endl;
		
		//Engine
		exec("cmake -P ./export/download_engine.cmake");
		exec("cmake -S ./export/Engine -B ./export/Engine/build -DCMAKE_INSTALL_PREFIX=./export/Engine/install");
		exec("cmake --build ./export/Engine/build");
		exec("cmake --install ./export/Engine/build");

		//App
		exec("cmake -S ./export/App -B ./export/App/build -DCMAKE_INSTALL_PREFIX=./export/Engine/install");
		exec("cmake --build ./export/App/build");

		//Project
		exec("cmake -S ./export/Project -B ./export/Project/build -DCMAKE_INSTALL_PREFIX=./export/Engine/install -DPROJECT_DIR=\"" + proj_path + "\"");
		exec("cmake --build ./export/Project/build");

		//move files
		exec("mkdir dist");
		exec("mkdir dist/lib");
		exec("mv ./export/App/build/Release/AppName ./dist/AppName");
		exec("mv ./export/Engine/build/Release/libEngine.so ./dist/lib/libEngine.so");
		exec("mv ./export/Project/build/Release/libApp.so ./dist/lib/libApp.so");
		exec("cp -r \"" + proj_path + "/res\" ./dist/res");
	}

	return 0;
}