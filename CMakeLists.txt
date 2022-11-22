cmake_minimum_required (VERSION 3.22)
project (VulkanImageSizeReproducer VERSION 2022011 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(Vulkan REQUIRED)

include_directories(${Vulkan_INCLUDE_DIRS})
link_directories($ENV{VULKAN_SDK}/Lib)

add_executable(VulkanImageMaxSizeReproducer VulkanImageMaxSizeReproducer.cpp)
add_executable(VulkanImageMemoryLeakReproducer VulkanImageMemoryLeakReproducer.cpp)
