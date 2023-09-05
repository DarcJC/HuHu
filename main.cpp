#include "rendering/rendering.h"

#include "dawn/utils/ComboRenderPipelineDescriptor.h"
#include "dawn/utils/WGPUHelpers.h"

int main() {

    rendering::Window new_window;

    rendering::wgpu::init_cpp_dawn_device(new_window.raw_ptr());

    // The hacky pipeline to render a triangle.
    wgpu::Device device = *rendering::wgpu::get_device();
    dawn::utils::ComboRenderPipelineDescriptor pipeline_desc;
    pipeline_desc.vertex.module = dawn::utils::CreateShaderModule(device, R"(
        @vertex fn main(@builtin(vertex_index) VertexIndex : u32)
                            -> @builtin(position) vec4f {
            var pos = array(
                vec2f( 0.0,  0.5),
                vec2f(-0.5, -0.5),
                vec2f( 0.5, -0.5)
            );
            return vec4f(pos[VertexIndex], 0.0, 1.0);
        })");
    pipeline_desc.cFragment.module = dawn::utils::CreateShaderModule(device, R"(
        @fragment fn main() -> @location(0) vec4f {
            return vec4f(1.0, 0.0, 0.0, 1.0);
        })");
    // BGRA shouldn't be hardcoded. Consider having a map[format -> pipeline].
    pipeline_desc.cTargets[0].format = wgpu::TextureFormat::BGRA8Unorm;
    wgpu::RenderPipeline triangle_pipeline = device.CreateRenderPipeline(&pipeline_desc);

    wgpu::Queue queue = device.GetQueue();

    while (!new_window.is_closed()) {
        new_window.poll();

        wgpu::TextureView view = rendering::wgpu::get_swapchain(new_window.raw_ptr())->GetCurrentTextureView();
        wgpu::CommandEncoder encoder = rendering::wgpu::get_device()->CreateCommandEncoder();
        dawn::utils::ComboRenderPassDescriptor desc{{view}};
        desc.cColorAttachments[0].loadOp = wgpu::LoadOp::Load;

        wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&desc);
        pass.SetPipeline(triangle_pipeline);
        pass.Draw(3);
        pass.End();

        wgpu::CommandBuffer commands = encoder.Finish();
        queue.Submit(1, &commands);

        rendering::wgpu::get_swapchain(new_window.raw_ptr())->Present();
    }

    return 0;
}
