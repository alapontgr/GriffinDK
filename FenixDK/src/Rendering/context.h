#pragma once

#include "Rendering/DisplayList.h"
#include "Utilities/types.h"

#include "Utilities/VulkanInclude.h"
#include <vector>
#include "VK_Framebuffer.h"
#include "RenderPass.h"
#include "Shader.h"

namespace fdk {
namespace Rendering {

constexpr u32 kDesiredSwapChainImageCount = 3;

constexpr u32 kBufferCount = 3;

// Configure vertex shader
struct VertexDesc
{
  v4 position;
  v4 colour;
};

struct VertexBuffer 
{
  u32 size;
  VkBuffer stagingBuffer = VK_NULL_HANDLE;
  VkBuffer gpuVertexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory pStagingBufferMem;
  VkDeviceMemory pGpuVertexBufferMem;
};

struct IndexBuffer 
{
  u32 size;
  VkIndexType elementType;
  VkBuffer stagingBuffer = VK_NULL_HANDLE;
  VkBuffer gpuIndexBuffer = VK_NULL_HANDLE;
  VkDeviceMemory pStagingBufferMem;
  VkDeviceMemory pGpuIndexBufferMem;
  u32 elementCount;
};

struct FragShaderUniforms 
{
  v4 colour;
};

struct TMPBuffer 
{
  u32 size;
  VkBuffer pBuffer;
  VkDeviceMemory pMemory;
};

struct VirtualFrame 
{
  VkFramebuffer frameBuffer = VK_NULL_HANDLE;
  VkSemaphore imageAvailable = VK_NULL_HANDLE; // Signaled by the presentation engine with the image is available
  VkSemaphore finishedRendering = VK_NULL_HANDLE; // Signaled when a queue has finished being processed
  VkCommandBuffer cmdBuffer = VK_NULL_HANDLE; // Command buffer
  VkFence cmdBufferReady = VK_NULL_HANDLE; // Use to sync the command buffer and don't override memory
};

class Context {
public:
  Context() {}

  void init();
  
  void clean_cmd_buffers();
  
  void release();

  void clean_materials();

  void begin_scene(const v4 &clearColor);

  void test_render(const f64 time);

  void end_scene();

  VkPhysicalDevice physical_device() const { return m_physicalDevice; }

  VkDevice device() const { return m_device; }

  VkFormat swap_chain_images_format() const { return m_swapChainFormat.format; }

  void on_resize_window();

private:
  using PipelineLayoutDeleter = AutoDeleter<VkPipelineLayout, PFN_vkDestroyPipelineLayout>;

  bool check_physical_device_properties(VkPhysicalDevice device,
                                        u32 &selectedFamilyIndex,
                                        u32 &swapChainFamilyIndex);
  void check_instance_available_extensions();
  void create_instance();

  void check_validation_layer_support();

  void check_device_available_extensions(VkPhysicalDevice pPhysicalDevice);
  VkSurfaceFormatKHR select_swap_chain_format();
  VkImageUsageFlags select_swap_chain_flags();
  VkSurfaceTransformFlagBitsKHR select_swap_chain_transform();
  VkPresentModeKHR select_swap_chain_present_mode();
  void create_device();
  void get_queue();
  void create_surface();
  void create_sync_objects();
  void create_swap_chain();
  void check_swap_chain_images();
  void create_cmd_pool();
  void allocate_cmd_buffers();
  void init_render_pass();
  // Pipeline
  void create_pipeline_layout();
  void create_pipeline();
  // Uniforms
  void create_uniform_descriptor_layout();
  void create_uniform_descriptor_pool();
  void create_uniform_descriptor_set();
  void update_uniforms(const f64 totalTime);

  // Mesh
  u32 find_memory_type_index(const u32 typeFilter, VkMemoryPropertyFlags memProperties);
  void create_buffer(
    VkDeviceSize bufferSize,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryProperties,
    VkBuffer& rOutBuffer,
    VkDeviceMemory& rOutMemory);
  void copy_buffer(VkBuffer& rFrom, VkBuffer& rTo, const u32 size);
  void create_vertex_buffer();
  void create_index_buffer();
  void create_mesh();
  void release_mesh();

  // Shader uniforms
  void create_uniform_buffers();

  // Render
  void acquire_next_free_image();
  void recreate_framebuffer(VirtualFrame& frame);
  void record_cmd_buffer();
  void submit_cmd_buffer();

  VkInstance m_instance = nullptr;

  VkPhysicalDevice m_physicalDevice = nullptr;
  VkDevice m_device = nullptr;
  u32 m_graphicsFamilyIndex;
  u32 m_presentFamilyIndex;
  // Queues
  VkQueue m_graphicsQueue = nullptr;
  VkQueue m_presentQueue = nullptr;
  // Synchronization
  //VkSemaphore m_imageReadySemaphore;
  //VkSemaphore m_endRenderingSemaphore;

  // Swap chain and surface
  VkSurfaceKHR m_surface;
  VkSwapchainKHR m_swapChain;
  VkSurfaceFormatKHR m_swapChainFormat;
  std::vector<VkImage> m_swapChainImages;
  std::vector<VkImageView> m_swapChainImageView;
  RenderPass m_renderPass;
  u32 m_bufferCount;
  // CMD buffers
  VkCommandPool m_cmdPool;
  std::vector<VirtualFrame> m_cmdBuffers;
  u32 m_currentCmd;
  u32 m_currentImageIndex;

  // Graphics pipeline resources
  // We should never save a wrapper of Vulkan objects like this (be sure that everything is released before we destroy the device)
  Shader m_dummyVS;
  Shader m_dummyPS;
  VkDescriptorSetLayout m_fragUniformLayout;
  VkDescriptorPool m_fragDescPool;
  VkDescriptorSet m_fragDescSet;
  VkPipelineLayout m_pipelineLayout;
  VkPipeline m_pipeline = VK_NULL_HANDLE; // Example of a graphics pipeline
  TMPBuffer m_fragUniformBuffer;

  // Mesh
  VertexBuffer m_vertexBuffer;
  IndexBuffer m_indexBuffer;

  // Settings
  VkSurfaceCapabilitiesKHR m_Capabilities;
  std::vector<VkSurfaceFormatKHR> m_supportedFormats;
  std::vector<VkPresentModeKHR> m_supportedPresentModes;
};
}
}