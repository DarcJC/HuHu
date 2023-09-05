#include "rendering/core.h"

#include "GLFW/glfw3.h"
#include "dawn/common/Assert.h"
#include "dawn/common/Log.h"
#include "dawn/common/Platform.h"
#include "dawn/common/SystemUtils.h"
#include "dawn/dawn_proc.h"
#include "dawn/native/DawnNative.h"
#include "dawn/utils/TerribleCommandBuffer.h"
#include "webgpu/webgpu_glfw.h"
#include "rendering/core/wgpu_traits.h"

#include <stdexcept>
#include <codecvt>
#include <locale>
#include <unordered_map>

using namespace rendering;

#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING 1

CharString tools::CStringHelper::unicode_to_utf8(const String &str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    return conv.to_bytes(str);
}

#undef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

void print_uncaptured_error(WGPUErrorType errorType, const char *message, void *) {
    const char *errorTypeName = "";
    switch (errorType) {
        case WGPUErrorType_Validation:
            errorTypeName = "Validation";
            break;
        case WGPUErrorType_OutOfMemory:
            errorTypeName = "Out of memory";
            break;
        case WGPUErrorType_Unknown:
            errorTypeName = "Unknown";
            break;
        case WGPUErrorType_DeviceLost:
            errorTypeName = "Device lost";
            break;
        default:
            UNREACHABLE();
            return;
    }
    dawn::ErrorLog() << errorTypeName << " error: " << message;
}

void print_device_log(WGPULoggingType type, const char *message, void *) {
    dawn::ErrorLog() << "Device log: " << message;
}

void print_device_lost(WGPUDeviceLostReason reason, const char *message, void *) {
    dawn::ErrorLog() << "Device lost: " << message;
}

dawn::native::Instance *rendering::wgpu::get_instance() {
    static TUniquePtr<dawn::native::Instance> GInstance = std::make_unique<dawn::native::Instance>();

    return GInstance.get();
}

void rendering::wgpu::init_cpp_dawn_device(GLFWwindow *glfw_window) {
    if (nullptr == glfw_window) {
        throw std::invalid_argument("[wgpu] create_cpp_dawn_device received a nullptr.");
    }

    dawn::ScopedEnvironmentVar angle_default_platform;
    if (dawn::GetEnvironmentVar("ANGLE_DEFAULT_PLATFORM").first.empty()) {
        angle_default_platform.Set("ANGLE_DEFAULT_PLATFORM", "swiftshader");
    }

    // Request device
    ::wgpu::Device *backend_device = get_device();

    // Create surface for our window
    ::wgpu::Surface *surface = get_surface(glfw_window);

    // Get function mapping table
    DawnProcTable &proc_table = get_proc_table();

    // Create swapchain for our window
    ::wgpu::SwapChain *swapchain = get_swapchain(glfw_window);

    dawnProcSetProcs(&proc_table);
    proc_table.deviceSetUncapturedErrorCallback(backend_device->Get(), print_uncaptured_error, nullptr);
    proc_table.deviceSetLoggingCallback(backend_device->Get(), print_device_log, nullptr);
    proc_table.deviceSetDeviceLostCallback(backend_device->Get(), print_device_lost, nullptr);
}

::wgpu::BackendType rendering::wgpu::get_backend_type() {
    static ::wgpu::BackendType GBackendType =
#ifdef DAWN_ENABLE_BACKEND_D3D12
            ::wgpu::BackendType::D3D12
#elif defined(DAWN_ENABLE_BACKEND_D3D11)
    ::wgpu::BackendType::D3D11
#elif defined(DAWN_ENABLE_BACKEND_METAL)
        ::wgpu::BackendType::Metal
#elif defined(DAWN_ENABLE_BACKEND_VULKAN)
        ::wgpu::BackendType::Vulkan
#elif defined(DAWN_ENABLE_BACKEND_OPENGLES)
        ::wgpu::BackendType::OpenGLES
#elif defined(DAWN_ENABLE_BACKEND_DESKTOP_GL)
        ::wgpu::BackendType::OpenGL
#else
#error No graphic backend selected.
#endif
    ;

    return GBackendType;
}

::wgpu::TextureFormat rendering::wgpu::get_preferred_swapchain_texture_format() {
    return ::wgpu::TextureFormat::RG32Float;
}

dawn::native::Adapter rendering::wgpu::get_adapter() {
    ::wgpu::RequestAdapterOptions options{
            .powerPreference = ::wgpu::PowerPreference::HighPerformance,
            .backendType = get_backend_type(),
    };

    return get_instance()->EnumerateAdapters(&options)[0];
}

DawnProcTable &rendering::wgpu::get_proc_table() {
    static DawnProcTable SProc = dawn::native::GetProcs();

    return SProc;
}

::wgpu::Device *rendering::wgpu::get_device() {
    static auto backend_adapter = get_adapter();

    static ::wgpu::DeviceDescriptor device_descriptor{};
    static TUniquePtr<::wgpu::Device> backend_device = std::make_unique<::wgpu::Device>(
            ::wgpu::Device::Acquire(backend_adapter.CreateDevice(&device_descriptor)));

    return backend_device.get();
}

::wgpu::Surface *rendering::wgpu::get_surface(GLFWwindow *glfw_window) {
    if (nullptr == glfw_window) {
        throw std::invalid_argument("[wgpu] get_surface received a nullptr.");
    }

    static std::unordered_map<GLFWwindow *, std::unique_ptr<::wgpu::Surface>> GWindow2SurfaceMap;

    if (!GWindow2SurfaceMap.count(glfw_window)) {
        auto surface_chained_descriptor = ::wgpu::glfw::SetupWindowAndGetSurfaceDescriptor(glfw_window);
        WGPUSurfaceDescriptor surface_descriptor{};
        surface_descriptor.nextInChain = reinterpret_cast<WGPUChainedStruct *>(surface_chained_descriptor.get());
        WGPUSurface surface = get_proc_table().instanceCreateSurface(get_instance()->Get(), &surface_descriptor);
        GWindow2SurfaceMap[glfw_window] = std::make_unique<::wgpu::Surface>(::wgpu::Surface::Acquire(surface));
    }

    return GWindow2SurfaceMap[glfw_window].get();
}

::wgpu::SwapChain *rendering::wgpu::get_swapchain(GLFWwindow *glfw_window) {
    if (nullptr == glfw_window) {
        throw std::invalid_argument("[wgpu] get_swapchain received a nullptr.");
    }

    static std::unordered_map<GLFWwindow *, std::unique_ptr<::wgpu::SwapChain>> GWindow2SwapchainMap;

    if (!GWindow2SwapchainMap.count(glfw_window)) {
        int window_height = 0, window_width = 0;
        glfwGetWindowSize(glfw_window, &window_width, &window_height);

        WGPUSwapChainDescriptor swapchain_descriptor{};
        swapchain_descriptor.usage = WGPUTextureUsage_RenderAttachment;
        swapchain_descriptor.format = static_cast<WGPUTextureFormat>(get_preferred_swapchain_texture_format());
        swapchain_descriptor.width = window_width;
        swapchain_descriptor.height = window_height;
        swapchain_descriptor.presentMode = WGPUPresentMode_Fifo;
        WGPUSwapChain backend_swapchain = get_proc_table().deviceCreateSwapChain(get_device()->Get(),
                                                                                 get_surface(glfw_window)->Get(),
                                                                                 &swapchain_descriptor);
        GWindow2SwapchainMap[glfw_window] = std::make_unique<::wgpu::SwapChain>(
                ::wgpu::SwapChain::Acquire(backend_swapchain));
    }

    return GWindow2SwapchainMap[glfw_window].get();
}
