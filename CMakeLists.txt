cmake_minimum_required(VERSION 3.13)
project(Blast)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(Blast STATIC)
target_include_directories(Blast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)
target_sources(Blast PUBLIC
        Source/Blast/Gfx/GfxContext.cpp
        Source/Blast/Gfx/GfxBuffer.cpp
        Source/Blast/Gfx/GfxTexture.cpp
        Source/Blast/Gfx/GfxRenderTarget.cpp
        Source/Blast/Gfx/GfxSwapchain.cpp
        Source/Blast/Gfx/GfxCommandBuffer.cpp
        Source/Blast/Gfx/GfxPipeline.cpp
        Source/Blast/Gfx/GfxShader.cpp
        Source/Blast/Gfx/GfxSampler.cpp
        Source/Blast/Gfx/Vulkan/VulkanDefine.cpp
        Source/Blast/Gfx/Vulkan/VulkanContext.cpp
        Source/Blast/Gfx/Vulkan/VulkanBuffer.cpp
        Source/Blast/Gfx/Vulkan/VulkanTexture.cpp
        Source/Blast/Gfx/Vulkan/VulkanRenderTarget.cpp
        Source/Blast/Gfx/Vulkan/VulkanSwapchain.cpp
        Source/Blast/Gfx/Vulkan/VulkanCommandBuffer.cpp
        Source/Blast/Gfx/Vulkan/VulkanPipeline.cpp
        Source/Blast/Gfx/Vulkan/VulkanShader.cpp
        Source/Blast/Gfx/Vulkan/VulkanSampler.cpp
        Source/Blast/Utility/VulkanShaderCompiler.cpp)

# 默认启动验证层
target_compile_definitions(Blast PUBLIC VULKAN_DEBUG)

# volk
add_library(volk STATIC External/volk/volk.c External/volk/volk.h)
target_include_directories(volk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/External/volk)
target_include_directories(volk PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/External/Vulkan-headers/include)
if (WIN32)
    target_compile_definitions(volk PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()
target_link_libraries(Blast PUBLIC volk)

# glslang
set(BUILD_EXTERNAL OFF)
target_include_directories(Blast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/External/glslang)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/External/glslang EXCLUDE_FROM_ALL)
target_link_libraries(Blast PUBLIC glslang)
target_link_libraries(Blast PUBLIC HLSL)
target_link_libraries(Blast PUBLIC OGLCompiler)
target_link_libraries(Blast PUBLIC OSDependent)
target_link_libraries(Blast PUBLIC SPIRV)
target_link_libraries(Blast PUBLIC SPVRemapper)
target_link_libraries(Blast PUBLIC glslang-default-resource-limits)

set(SPIRV_CROSS_ENABLE_TESTS OFF)
target_include_directories(Blast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/External/SPIRV-Cross)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/External/SPIRV-Cross EXCLUDE_FROM_ALL)
target_link_libraries(Blast PUBLIC spirv-cross-c)
target_link_libraries(Blast PUBLIC spirv-cross-core)
target_link_libraries(Blast PUBLIC spirv-cross-cpp)
target_link_libraries(Blast PUBLIC spirv-cross-glsl)
target_link_libraries(Blast PUBLIC spirv-cross-hlsl)
target_link_libraries(Blast PUBLIC spirv-cross-msl)
target_link_libraries(Blast PUBLIC spirv-cross-reflect)
target_link_libraries(Blast PUBLIC spirv-cross-util)