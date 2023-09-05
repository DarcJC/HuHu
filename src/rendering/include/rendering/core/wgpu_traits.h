#pragma once

#include "dawn/webgpu.h"
#include "dawn/webgpu_cpp.h"

struct GLFWwindow;

struct DawnProcTable;

namespace dawn::native {
    class Instance;
    class Adapter;
}

namespace rendering::wgpu {

    ::wgpu::BackendType get_backend_type();

    ::wgpu::TextureFormat get_preferred_swapchain_texture_format();

    dawn::native::Adapter get_adapter();

    DawnProcTable& get_proc_table();

    dawn::native::Instance* get_instance();

    ::wgpu::Device* get_device();

    ::wgpu::Surface* get_surface(GLFWwindow* glfw_window);

    ::wgpu::SwapChain* get_swapchain(GLFWwindow* glfw_window, bool force_update = false);

    void init_cpp_dawn_device(GLFWwindow* glfw_window);

}// namespace rendering::wgpu
