#include <iostream>
#include "rendering/rendering.h"
#include "GLFW/glfw3.h"

int main() {

    rendering::Window new_window;

    rendering::wgpu::init_cpp_dawn_device(new_window.raw_ptr());

    while (!new_window.is_closed()) {
        new_window.poll();
    }

    return 0;
}
