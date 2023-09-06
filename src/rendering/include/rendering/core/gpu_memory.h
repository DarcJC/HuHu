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
        virtual void init_render_thread();

        virtual void release();
        virtual void release_render_thread() noexcept;

    private:
        bool has_initialized = false;
    };

    template <typename DataType_>
    class TBufferResource : public GPUResource {
    public:
        typedef DataType_ DataType;
        typedef TBufferResource<DataType> Super;

        TBufferResource(const std::vector<DataType>& data, bool cpu_access = true, const char *debug_name = "Buffer") {
            m_data = data;
            m_cpu_access = cpu_access;
            m_debug_name = debug_name;

            if (m_data.size() == 0) {
                throw ResourceInitializationError("Vertex buffer size could not be zero.");
            }
        }

        TBufferResource(TBufferResource&& buffer_to_move) {
            m_data = std::move(buffer_to_move.m_data);
            m_cpu_access = buffer_to_move.m_cpu_access;
            m_debug_name = std::move(buffer_to_move.m_debug_name);
            m_descriptor = buffer_to_move.m_descriptor;
            m_buffer = std::move(buffer_to_move.m_buffer);
        }

        virtual ::wgpu::Buffer& get_buffer() {
            return m_buffer;
        }

    protected:
        std::vector<DataType> m_data;
        bool m_cpu_access = true;
        ::wgpu::BufferDescriptor m_descriptor;
        ::wgpu::Buffer m_buffer;
        CharString m_debug_name;
    };

    class BasicVertexBuffer : public TBufferResource<Vector> {
    public:
        using TBufferResource<Vector>::TBufferResource;

        BasicVertexBuffer(const std::vector<DataType>& data, bool cpu_access = true, const char *debug_name = "BasicVertexBuffer");

        void init_render_thread() override;

        void release_render_thread() noexcept override;
    };

    class BasicIndexBuffer : public TBufferResource<Vector> {
    public:
        BasicIndexBuffer(const std::vector<DataType>& data, bool cpu_access = true, const char *debug_name = "BasicIndexBuffer");

        void init_render_thread() override;

        void release_render_thread() noexcept override;
    };

}// namespace rendering