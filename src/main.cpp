#include <filesystem>
#include <iostream>
#include <utility>
#include <cstdlib>

#include <CLI/Options.hpp>
#include <CLI/Tools.hpp>

#ifdef _WIN32
	static const std::string DEFAULT_PLATFORM = "Windows";
#else
	static const std::string DEFAULT_PLATFORM = "Linux";
#endif

template <typename... Args>
int exec(Args&&... args)
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


class Command
{
public:
	Command(const std::string& name, const std::vector<cli::Option>& options):
		m_name(name),
		m_options(options) {}

	virtual ~Command() = default;

	virtual int callback(const cli::Options& opt, const std::string& self_path) = 0;

	cli::Command toCLI() const
	{
		return cli::Command{m_name, m_options};
	}

	const std::string& getName() const
	{
		return m_name;
	}

protected:
	std::string m_name;
	std::vector<cli::Option> m_options;
};

class BuildCommand : public Command
{
public:
	BuildCommand():
		Command("build", {
			{cli::Option::Name('p', "project"), cli::Option::Type::ONE_ARGUMENT},
			{cli::Option::Name('o', "output"),  cli::Option::Type::ONE_ARGUMENT},
			{cli::Option::Name('c', "config"),  cli::Option::Type::ONE_ARGUMENT},
			{cli::Option::Name("platform"),     cli::Option::Type::ONE_ARGUMENT}
		}) {}
	
	cli::Option::Name project  = m_options[0].name;
	cli::Option::Name output   = m_options[1].name;
	cli::Option::Name config   = m_options[2].name;
	cli::Option::Name platform = m_options[3].name;

	int callback(const cli::Options& opt, const std::string& self_path) override
	{
		auto project_arg  = opt.getArgument(this->project);
		auto output_arg   = opt.getArgument(this->output);
		auto config_arg   = opt.getArgument(this->config);
		auto platform_arg = opt.getArgument(this->platform);

		if (!project_arg)
		{
			std::cout << "No project path" << std::endl;
			return 1;
		}

		std::string platform = platform_arg ? *platform_arg : DEFAULT_PLATFORM;
		std::filesystem::path proj_path = std::filesystem::canonical(*project_arg);
		std::filesystem::path root = std::filesystem::canonical(self_path).parent_path();
		std::filesystem::path work = root / "work";
		std::filesystem::path exportd = root / "export";
		std::filesystem::path out  = output_arg ? std::filesystem::weakly_canonical(*output_arg) : (root/("runtime"));
		std::filesystem::path platform_export = exportd / platform;
		std::string cfg = config_arg ? *config_arg : "Release";
		std::string engine_install = (work/"Engine/install").string();
		std::string DCMAKE_BUILD_TYPE = "-DCMAKE_BUILD_TYPE=" + cfg;
		std::string DCMAKE_PREFIX_PATH = "-DCMAKE_PREFIX_PATH=" + engine_install;
		std::string DCMAKE_INSTALL_PREFIX = "-DCMAKE_INSTALL_PREFIX=" + engine_install;

		std::cout << "proj_path: " << proj_path.string() << std::endl;
		std::cout << "root: " << root.string() << std::endl;
		std::cout << "work: " << work.string() << std::endl;
		std::cout << "out: " << out.string() << std::endl;
		std::cout << "config: " << cfg << std::endl;
		std::cout << "platform: " << platform << std::endl;
		std::cout << std::endl;

		if (!std::filesystem::exists(platform_export))
		{
			std::cout << "Wrong platform" << std::endl;
			return 1;
		}

		//Engine
		exec("cmake", "-DENGINE_SOURCE_DIR=" + (work/"Engine").string(), "-P", (platform_export/"Engine/fetch.cmake").string());
		exec("cmake", "-S",        (work/"Engine").string(),       "-B", (work/"Engine/build").string(), DCMAKE_INSTALL_PREFIX, DCMAKE_BUILD_TYPE);
		exec("cmake", "--build",   (work/"Engine/build").string(), "--config", cfg);
		exec("cmake", "--install", (work/"Engine/build").string(), "--config", cfg);

		//App
		exec("cmake", "-S", (platform_export/"App").string(), "-B", (work/"App").string(), DCMAKE_PREFIX_PATH, DCMAKE_BUILD_TYPE);
		exec("cmake", "--build",       (work/"App").string(), "--config", cfg);
		exec("cmake", "--install",     (work/"App").string(), "--config", cfg, "--prefix", out.string());

		//Project
		exec("cmake", "-S", (platform_export/"Project").string(), "-B", (work/"Project").string(), DCMAKE_PREFIX_PATH, DCMAKE_BUILD_TYPE, "-DPROJECT_DIR=" + proj_path.string());
		exec("cmake", "--build",       (work/"Project").string(), "--config", cfg);
		exec("cmake", "--install",     (work/"Project").string(), "--config", cfg, "--prefix", out.string());

		//Runtime
		exec("cmake", "--install", (work/"Engine/build").string(), "--config", cfg, "--prefix", out.string(), "--component", "runtime");

		return 0;
	}
};


int main(int argc, const char* argv[])
{
	BuildCommand build_command;
	std::vector<cli::Command> commands = {build_command.toCLI()};

	cli::Options opt;
	bool res = opt.parse(cli::tools::extractArguments(argv, argc), commands);
	if (!res)
	{
		std::cout << "bad arguments" << std::endl;
		return 1;
	}

	if (opt.getCalledCommandName() == "build")
		return build_command.callback(opt, std::string(argv[0]));

	return 0;
}