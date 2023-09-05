#include "rendering/core/gpu_memory.h"
#include "rendering/core/wgpu_traits.h"

#include <stdexcept>

using namespace rendering;

ResourceInitializationError::ResourceInitializationError(const rendering::CharString &_Message) : _MyBase(_Message.c_str()) {}

ResourceInitializationError::ResourceInitializationError(const char *_Message) : _MyBase(_Message) {}

void GPUResource::init() {
    if (has_initialized) {
        throw ResourceInitializationError("Resource has been initialized!");
    }
    has_initialized = true;
}

void GPUResource::release() noexcept {}

void BasicVertexBuffer::init() {
    Super::init();

    m_descriptor.usage = ::wgpu::BufferUsage::MapWrite | ::wgpu::BufferUsage::Vertex;
    if (m_cpu_access) {
        m_descriptor.usage |= ::wgpu::BufferUsage::MapRead;
    }
    m_descriptor.size = m_data.size();

    m_buffer = wgpu::get_device()->CreateBuffer(&m_descriptor);

    size_t mapped_size = m_buffer.GetSize();
    DataType * mapped_memory = static_cast<DataType*>(m_buffer.GetMappedRange());

    std::memcpy(mapped_memory, m_data.data(), std::clamp<size_t>(m_data.size(), 0, mapped_size));

    if (!m_cpu_access) {
        m_data.resize(0);
    }
}

void BasicVertexBuffer::release() noexcept {
    m_data.clear();
    m_buffer.Destroy();
}

BasicVertexBuffer::BasicVertexBuffer(const std::vector<DataType> &data, bool cpu_access, const char *debug_name)
    : m_data(data)
    , m_cpu_access(cpu_access)
{
    m_descriptor.label = debug_name;
    m_descriptor.mappedAtCreation = true;

    if (m_data.size() == 0) {
        throw ResourceInitializationError("Vertex buffer size could not be zero.");
    }
}

