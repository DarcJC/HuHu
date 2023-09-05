#pragma once

#include "webgpu/webgpu_cpp.h"

#include "type_traits.h"
#include <stdexcept>
#include <vector>

namespace rendering {
    class ResourceInitializationError : public std::exception {
    public:
        using _MyBase = std::exception;

        explicit ResourceInitializationError(const rendering::CharString &_Message);

        explicit ResourceInitializationError(const char *_Message);
    };

    class GPUResource {

    public:
        virtual void init();

        virtual void release() noexcept;

    private:
        bool has_initialized = false;
    };

    class BasicVertexBuffer : public GPUResource {
        using Super = GPUResource;
        using DataType = Vector;
    public:
        BasicVertexBuffer(const std::vector<DataType>& data, bool cpu_access = true, const char *debug_name = "BasicVertexBuffer");

        void init() override;

        void release() noexcept override;

    private:
        std::vector<DataType> m_data;
        bool m_cpu_access;
        ::wgpu::BufferDescriptor m_descriptor;
        ::wgpu::Buffer m_buffer;
    };
}// namespace rendering