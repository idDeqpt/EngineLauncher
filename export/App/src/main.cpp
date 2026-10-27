#include <Engine/Core/Engine.hpp>
#include <Engine/Core/Node.hpp>
#include <dlfcn.h>
#include <cstdio>

using create_root_fn  = eng::core::Node*(*)();
using destroy_root_fn = void(*)(eng::core::Node*);

int main(int argc, char** argv) {
    const char* plugin_path = argc > 1 ? argv[1] : "lib/libApp.so";

    void* handle = dlopen(plugin_path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) { std::fprintf(stderr, "dlopen: %s\n", dlerror()); return 1; }

    auto create  = (create_root_fn) dlsym(handle, "engine_create_root");
    if (!create) { std::fprintf(stderr, "factory symbols missing\n"); return 2; }

    eng::core::Engine engine{std::unique_ptr<eng::core::Node>(create())};
    engine.setup();
    engine.mainLoop();

    dlclose(handle);
}