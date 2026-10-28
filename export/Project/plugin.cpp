#include <Root.hpp>

extern "C" eng::core::Node* engine_create_root()
{
	return new Root();
}