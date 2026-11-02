#include <Engine/Core/PluginHost.hpp>
#include <Engine/Core/Plugin.hpp>
#include <memory>

#include <Root.hpp>

class AppPlugin : public eng::core::Plugin
{
public:
	void onSetup(const eng::core::PluginHost& host) override
	{
		host.root.addChild(std::make_unique<Root>(), "game");
	}
};

extern "C" eng::core::Plugin* engine_plugin_create()
{
	return new AppPlugin();
}

extern "C" void engine_plugin_destroy(eng::core::Plugin* plugin)
{
	delete plugin;
}