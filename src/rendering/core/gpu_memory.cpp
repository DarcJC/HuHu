#include "rendering/core/gpu_memory.h"
#include "rendering/core/wgpu_traits.h"

#include <stdexcept>

using namespace rendering;

ResourceInitializationError::ResourceInitializationError(const rendering::CharString &_Message) : _MyBase(_Message.c_str()) {}

ResourceInitializationError::ResourceInitializationError(const char *_Message) : _MyBase(_Message) {}

void GPUResource::init_render_thread() {
    if (has_initialized) {
        throw ResourceInitializationError("Resource has been initialized!");
    }
    has_initialized = true;
}

void GPUResource::release_render_thread() noexcept {}

void GPUResource::init() {}

void GPUResource::release() {}

BasicVertexBuffer::BasicVertexBuffer(const std::vector<DataType> &data, bool cpu_access, const char *debug_name) : TBufferResource<Vector>(data, cpu_access, debug_name) {}

void BasicVertexBuffer::init_render_thread() {
    Super::init_render_thread();

    m_descriptor.usage = ::wgpu::BufferUsage::MapWrite | ::wgpu::BufferUsage::Vertex;
    m_descriptor.size = m_data.size() * sizeof(DataType);
    m_descriptor.label = m_debug_name.c_str();
    m_descriptor.mappedAtCreation = true;
    if (m_cpu_access) {
        m_descriptor.usage |= ::wgpu::BufferUsage::MapRead;
    }

    m_buffer = wgpu::get_device()->CreateBuffer(&m_descriptor);

    size_t mapped_size = m_buffer.GetSize();
    DataType * mapped_memory = static_cast<DataType*>(m_buffer.GetMappedRange());

    std::memcpy(mapped_memory, m_data.data(), std::clamp<size_t>(m_data.size() * sizeof(DataType), 0, mapped_size));

    m_buffer.Unmap();
}

void BasicVertexBuffer::release_render_thread() noexcept {
    m_data.resize(0);
    m_buffer.Destroy();
}

BasicIndexBuffer::BasicIndexBuffer(const std::vector<DataType> &data, bool cpu_access, const char *debug_name) : TBufferResource<Vector>(data, cpu_access, debug_name) {}

void BasicIndexBuffer::init_render_thread() {
    Super::init_render_thread();

    m_descriptor.usage = ::wgpu::BufferUsage::MapWrite | ::wgpu::BufferUsage::Index;
    m_descriptor.size = m_data.size() * sizeof(DataType);
    m_descriptor.label = m_debug_name.c_str();
    m_descriptor.mappedAtCreation = true;
    if (m_cpu_access) {
        m_descriptor.usage |= ::wgpu::BufferUsage::MapRead;
    }

    m_buffer = wgpu::get_device()->CreateBuffer(&m_descriptor);

    size_t mapped_size = m_buffer.GetSize();
    DataType * mapped_memory = static_cast<DataType*>(m_buffer.GetMappedRange());

    std::memcpy(mapped_memory, m_data.data(), std::clamp<size_t>(m_data.size() * sizeof(DataType), 0, mapped_size));

    m_buffer.Unmap();
}

void BasicIndexBuffer::release_render_thread() noexcept {
    m_data.resize(0);
    m_buffer.Destroy();
}

