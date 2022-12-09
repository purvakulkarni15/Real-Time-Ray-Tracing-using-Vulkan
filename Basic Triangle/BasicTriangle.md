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
2. Create a vulkan instance (VkInstance) by querying necessary extensions and enabling the validation layer.
3. Pick a supporting physical graphics device (VkPhysicalDevice) from the available list. Extensions such as "device support to RT" should be checked at this step.
4. Create a logical device (VkDevice) to select the features from physical device and submit them through queues (VkQueue)
### Key Learnings:

![Components flow chart](https://github.com/purvakulkarni15/Real-Time-Ray-Tracing-using-Vulkan/blob/main/Basic%20Triangle/Flowcharts/vulkan1.png)

#### Standard steps:
##### Create/Destroy an object
1. Fill a struct/s with information necessary for object creation.
   Struct struc = {}; This means either zero initialize or initialize every member.
3. Call vkCreate---(--,--,--, &object) and pass object as the out parameter.
4. Use vkDestroy---() to deallocate its memory.
##### Get list of available options
1. vKEnumerate----(--, &count, nullptr)
2. Create a DS to hold options: vector<VKType> vec(count)
3. vKEnumerate----(--,&count, vec.data())
   
#### 1. Extensions:
  1. A graphics card may have a unique functionality that is exposed to developers by Vulkan via extensions.
  2. Developer needs to query any such extensions/capabilities explicitly while creating VkInstance.
  3. Eg. NVIDIA's hardware support to Ray Tracing Pipeline can be added via VK_KHR_ray_tracing_pipeline and other extensions.
#### 2. Validation layers:
  1. Vulkan driver is designed to have minimal overhead and hence the error checking is left entirely to the user.
  2. Validation layers are functionality provided by Vulkan to additionally enable checking such nullptrs, resource allocation, thread safety, etc.
  3. Enabling a validation layer simply means calling a proxy function that does the error-checking before proceeding to the main function call.
#### 3. Queue families:
  1. Every operation in Vulkan,(drawing, uploading textures, memory transfer) requires commands to be submitted to a queue. 
  2. There are different types of queues coming from different queue families and each family of queues allows only a subset of commands.
  3. For example, a queue family that only allows memory transfer related commands.
### 4. Physical and Logical Devices:
  1. Physical Device (VKPhysicalDevice) gives us all the properties of a device. (Read only)
  2. Logical Device (VKDevice) is used to interact with the driver of the physical device chosen through VkQueues. (Repr of physical device. Vulkan's window into Graphics card)

## 2. Presentation
### Steps:
1. Create a connection of vulkan with windows system to present results via WSI extensions.(Done by GLFW internally)
2. Create a swap chain object to hold the image objects used for rendering by vulkan.
3. Create ImageView container to contain swap chain images. To view any image (VkImage) we need to access it via VkImageView.

### Key Learnings
   
![Presentation flow chart](https://github.com/purvakulkarni15/Real-Time-Ray-Tracing-using-Vulkan/blob/main/Basic%20Triangle/Flowcharts/vulkan2.png)

### 1. WSI Extensions
1. VK_KHR_surface exposes a VkSurfaceKHR object that represents an abstract type of surface to present rendered images. The surface in our program will be backed by the window created using GLFW. 
2. VK_KHR_SURFACE: This extension is included in list given by glfwGetRequiredInstanceExtensions. (Instance level extension)

### 2. Swap Chains
 1. Vulkan does not have default frame buffer to render and needs a structure called swap chain to store these buffers.
 2. The swap chain is a queue of images that are waiting to be presented to the screen. 
 3. The general purpose of the swap chain is to synchronize the presentation of images with the refresh rate of the screen.
 4. There are three kinds of properties we need to check to create a swap chain
    - Basic surface capabilities (min/max number of images in swap chain, min/max width and height of images)
    - Surface formats (pixel format, color space)
    - Available presentation modes(conditions for "swapping" images to the screen.
 5. VK_KHR_Swapchains: Device level extension
 6. Link to tutorial: https://www.youtube.com/watch?v=nSzQcyQTtRY
   
### 3. Presentation modes
1. VK_PRESENT_MODE_IMMEDIATE_KHR: Images produced by application are transferred directly to screen which may result in tearing.
2. VK_PRESENT_MODE_FIFO_KHR: Display takes an image from the front of the swap chain queue when the display is refreshed. Program inserts rendered images at the back of the queue. `vertical blank: The moment the display is refreshed.`
3. VK_PRESENT_MODE_FIFO_RELAXED_KHR: If the application is late and the queue was empty at the last vertical blank, the image is transferred directlyand may result in visible tearing.
4. VK_PRESENT_MODE_MAILBOX_KHR: Variation of the second mode. When the queue is full, the images that are already queued are replaced with the newer ones. 

### 4. Swap extent
1. The swap extent is the resolution of the swap chain images.
2. The range of the possible resolutions is defined in the VkSurfaceCapabilitiesKHR structure. 
3. The resolution of the window needs to be matched by setting the width and height in the currentExtent member. 
5. GLFW uses two units when measuring sizes: pixels and screen coordinates. 

### 5. Image Tearing
#### Problem
1. A graphics adapter has a pointer to a surface that represents the image being displayed on the monitor, called a front buffer. 
2. When monitor is refreshed, the graphics card sends the contents of the front buffer to the monitor to be displayed. 
3. The monitor's refresh rates are very slow in comparison to the rest of the computer. (60 Hz (60 times per second) to 100 Hz.)
4. If your application is updating the front buffer while the monitor is in the middle of a refresh, the image that is displayed will be cut in half with the upper half of the display containing the old image and the lower half containing the new image. 
5. This problem is referred to as tearing.
#### Solution - Back buffering 
1. vertical retrace (or vertical sync) operation: Monitor refreshes its image by moving a light pin horizontally, from the top left of the monitor and ending at the bottom right. When the light pin reaches the bottom, the monitor recalibrates the light pin by moving it back to the upper left so that the process can start again. This recalibration is called a vertical sync. 
2. During a vertical sync, the monitor is not drawing anything, so any update to the front buffer will not be seen until the monitor starts to draw again. The vertical sync is relatively slow; however, not slow enough to render a complex scene while waiting. What is needed to avoid tearing and be able to render complex scenes is a process called back buffering.

### 6. Image View
   1. Image has two parts - memory(data) and View. 
   2. Image view is a protocol used to access the image in vulkan.
   
## 3. Graphics Pipeline
### 1. Simplified overview:
1. Input assembler: collects the raw vertex/indices data from the specified buffers.
2. Vertex shader: Runs for every vertex and generally applies transformations to turn vertex positions from model space to screen space. It also passes per-vertex data down the pipeline.
3. Tessellation shader: Allows to subdivide geometry based on certain rules to increase the mesh quality.
4. Geometry shader: Runs on every primitive (triangle, line, point) and can discard it or output more primitives than came in. (Not used much in today's applications because the performance is not that good on most graphics cards except for Intel's integrated GPUs.)
5. Rasterization: Discretizes the primitives into fragments-pixel elements that they fill on the framebuffer. Any fragments that fall outside the screen are discarded and the attributes outputted by the vertex shader are interpolated across the fragments. Usually the fragments that are behind other primitive fragments are also discarded here because of depth testing.
6. Fragment shader: Invoked for every fragment that survives rasterization. Determines the color using the interpolated data from the vertex shader, which can include things like texture coordinates and normals for lighting.
7. Color blending: Applies operations to mix different fragments that map to the same pixel in the framebuffer. Fragments can simply overwrite each other, add up or be mixed based upon transparency.
   
### Steps
1. Write shaders and compile them using glslc to get bytecodes in .spv format.
2. Load the shader binaries and create a shader module.
3. Create pipeline stage objects each for every shader module   
   
   
### Key Learnings
1. SPIR-V: 
   1. Unlike OpenGL, shader code in vulkan has to be specified in bytecode format called as SPIR-V.
   2. Advantage: Compilers written by GPU vendors to turn shader code into native code are significantly less complex. With GLSL, some GPU vendors used their interpretation of the standard which might cause some vendor's drivers rejecting your code due to syntax errors. With a straightforward bytecode format like SPIR-V that will hopefully be avoided.
#### glslc: a command line compiler for GLSL/HLSL to SPIR-V
