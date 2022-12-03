# Aim: To render a simple triangle using Vulkan

## Windows Setup:

1. Download LunarSDK( includes the headers and a loader for the Vulkan functions to look up the functions in the driver at runtime, similar to GLEW for OpenGL) for Vulkan.Link: https://vulkan.lunarg.com/sdk/home#windows
2. Install GLFW  for creating a window to display the rendered results. Link: https://www.glfw.org/
3. Install GLM for vector algebra operations. Link:https://github.com/g-truc/glm/releases
4. Add Include/ and lib/ paths as additional includes and libraries is VS Studio
5. Set the language compiler to C++17


## How to draw a triangle?
## 1. Setup
### Steps:
1. Create window and event handling loop using GLFW.
2. Create a vulkan instance (VkInstance) by adding necessary extensions and enabling the validation layer.
3. Pick a supporting physical graphics device (VkPhysicalDevice) from the available list.
4. Create a logical device (VkDevice) to select the features from physical device and submit them through queues (VkQueue)
### Key Learnings:
#### 1. Extensions:
  1. A graphics card may have a unique functionality that is exposed to developers by Vulkan via extensions.
  2. Developer needs to query any such extensions/capabilities explicitly while creating VkInstance.
  3. Eg. NVIDIA's hardware support to Ray Tracing Pipeline can be added via VK_KHR_ray_tracing_pipeline and other extensions.
#### 2. Validation layers:
  1. Vulkan driver is designed to have minimal overhead and hence the error checking is left entirely to the user.
  2. Validation layers are functionality provided by Vulkan to enable additionally checking such nullptrs, resource allocation, thread safety, etc.
  3. Enabling a validation layer simply means calling a proxy function that does the error-checking before proceeding to the main function call.
#### 3. Queue families:
  1. Every operation in Vulkan,(drawing, uploading textures, memory transfer) requires commands to be submitted to a queue. 
  2. There are different types of queues coming from different queue families and each family of queues allows only a subset of commands.
  3. For example, a queue family that only allows memory transfer related commands.

## 2. Presentation
