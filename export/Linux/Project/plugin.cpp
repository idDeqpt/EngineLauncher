#include <Engine/Core/PluginHost.hpp>
#include <Engine/Core/Plugin.hpp>
#include <memory>

#include <plugin_config.hpp>

class AppPlugin : public eng::core::Plugin
{
public:
	void onSetup(const eng::core::PluginHost& host) override
	{
		host.root.addChild(std::make_unique<PLUGIN_ROOT_CLASS>(), "game");
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