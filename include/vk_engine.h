// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "vk_types.h"
#include "vk_descriptors.h"
#include "vk_loader.h"
#include "vk_pipelines.h"
#include "camera.h"

#include <vk_mem_alloc.h>

#include <span>
#include <string>
#include <vector>
#include <deque>
#include <functional>
#include <unordered_map>

struct EngineStats {
	float frame_time;
	int triangle_count;
	int draw_call_count;
	float scene_update_time;
	float mesh_draw_time;
	glm::vec3 camera_location;
};

struct ComputePushConstants {
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};

struct ComputeEffect {
	const char* name;

	VkPipeline pipeline;
	VkPipelineLayout layout;

	ComputePushConstants data;
};

struct DeletionQueue {
	
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function) // inefficient at scale... store arrays of vulkan handles of various types, then delete from loop
	{
		deletors.push_back(function);
	}

	void flush()
	{
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
		{
			(*it)();
		}

		deletors.clear();
	}
};

struct FrameData {

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;
	VkSemaphore _swapchainSemaphore;
	VkSemaphore _renderSemaphore;
	VkFence _renderFence;
	DeletionQueue _deletionQueue;
	DescriptorAllocatorGrowable _frameDescriptors;
};

struct MeshNode : public Node {

	std::shared_ptr<MeshAsset> mesh;

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
};

struct RenderObject {
	uint32_t indexCount;
	uint32_t firstIndex;
	VkBuffer indexBuffer;

	MaterialInstance* material;
	Bounds bounds;
	glm::mat4 transform;
	VkDeviceAddress vertexBufferAddress;
};

struct DrawContext {
	std::vector<RenderObject> OpaqueSurfaces;
	std::vector<RenderObject> TransparentSurfaces;
};

struct GLTFMetallic_Roughness {
	MaterialPipeline opaquePipeline;
	MaterialPipeline transparentPipeline;
	VkPipelineLayout gltfPipelineLayout;

	VkDescriptorSetLayout materialLayout;

	struct MaterialConstants {
		glm::vec4 colorFactors;
		glm::vec4 metal_rough_factors;
		//padding, we need it anyway for uniform buffers.. struct should be 256 bytes for gpu alignment
		glm::vec4 extra[14];
	};

	struct MaterialResources {
		AllocatedImage colorImage;
		VkSampler colorSampler;
		AllocatedImage metalRoughImage;
		VkSampler metalRoughSampler;
		AllocatedImage normalImage;
		VkSampler normalSampler;
		VkBuffer dataBuffer;
		uint32_t dataBufferOffset;
	};

	DescriptorWriter writer;

	void build_pipelines(VulkanEngine* engine);
	void clear_resources(VkDevice device);

	MaterialInstance write_material(VkDevice device, MaterialPass pass, const MaterialResources& resources, DescriptorAllocatorGrowable& descriptorAllocator);
};

constexpr unsigned int FRAME_OVERLAP = 2;

class VulkanEngine {
public:

	bool _isInitialized{ false };
	int _frameNumber {0};
	bool _freeze_rendering{ false };
	bool _resize_requested;
	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr }; 

	static VulkanEngine& Get();

	FrameData& get_current_frame() { return _frames[_frameNumber % FRAME_OVERLAP]; };
 
	//initializes everything in the engine
	void init(); 
	//shuts down the engine
	void cleanup(); 
	//draw loop
	void draw(); 
	//run main loop
	void run();

	void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function);
	GPUMeshBuffers uploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);

	VkInstance _instance;
	VkDebugUtilsMessengerEXT _debug_messenger;
	VkPhysicalDevice _chosenGPU;
	VkDevice _device;
	VkSurfaceKHR _surface;
	VkPhysicalDeviceProperties _gpuProperties;
	
	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;

	std::vector<VkImage> _swapchainImages;
	std::vector<VkImageView> _swapchainImageViews;
	VkExtent2D _swapchainExtent;

	FrameData _frames[FRAME_OVERLAP];
	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;
	DeletionQueue _mainDeletionQueue;
	VmaAllocator _allocator;

	AllocatedImage _drawImage;
	AllocatedImage _depthImage;
	VkExtent2D _drawExtent;
	float _renderScale = 1.0f;
	
	DescriptorAllocatorGrowable _globalDescriptorAllocator;

	VkDescriptorSet _drawImageDescriptors;
	VkDescriptorSetLayout _drawImageDescriptorLayout;

	VkPipeline _gradientPipeline;
	VkPipelineLayout _gradientPipelineLayout;

	VkFence _immFence;
	VkCommandBuffer _immCommandBuffer;
	VkCommandPool _immCommandPool;

	std::vector<ComputeEffect> _backgroundEffects;
	int _currentBackgroundEffect{ 1 };
 
	GPUSceneData _sceneData;

	VkDescriptorSetLayout _gpuSceneDataDescriptorLayout;

	VkDescriptorSetLayout _postProcessingDescriptorLayout;
	VkPipelineLayout _postProcessingPipelineLayout;
	VkPipeline _postProcessingPipeline;
	AllocatedImage _postProcessingImage;
	VkDescriptorSet _postProcessingDescriptors;

	AllocatedImage _whiteImage;
	AllocatedImage _blackImage;
	AllocatedImage _greyImage;
	AllocatedImage _errorCheckerboardImage;

	VkSampler _defaultSamplerLinear;
	VkSampler _defaultSamplerNearest;
 
	GLTFMetallic_Roughness _metalRoughMaterial;

	DrawContext _mainDrawContext;

	Camera _mainCamera;

	std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> _loadedScenes;

	EngineStats _stats;

	AllocatedBuffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	void destroy_buffer(const AllocatedBuffer &buffer); 
	AllocatedImage create_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
	AllocatedImage create_image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
	void destroy_image(const AllocatedImage& img);

private:
	
	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_sync_structures();
	void init_descriptors();
	void init_pipelines();
	void init_background_pipelines();
	void init_imgui();
	void init_default_data();
	void init_renderables();
	void init_post_process_pipelines();

	void create_swapchain(uint32_t width, uint32_t hegiht);
	void destroy_swapchain();
	void resize_swapchain();
	
	void draw_main(VkCommandBuffer cmd);
	void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);
	void draw_geometry(VkCommandBuffer cmd);


	void update_scene();
};
namespace vkutil {
	bool is_visible(const RenderObject& obj, const glm::mat4& viewProj);
}
