#include <iostream>
#include "rendering/rendering.h"
#include "GLFW/glfw3.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

    rendering::Window new_window;
    while (!new_window.is_closed()) {
        new_window.poll();
    }

    return 0;
}
