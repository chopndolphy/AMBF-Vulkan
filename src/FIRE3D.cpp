#include <vk_engine.h>

int main(int argc, char* argv[])
{
	// Optional first argument: path to a glTF/glb scene to load.
	// When omitted, the engine loads its default sample scene.
	std::string scenePath = (argc > 1) ? argv[1] : "";

	VulkanEngine engine;

	engine.init(scenePath);
	
	engine.run();	

	engine.cleanup();	

	return 0;
}
