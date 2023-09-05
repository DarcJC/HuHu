#pragma once

#include "type_traits.h"

struct GLFWwindow;

namespace rendering {
    struct GLFWInitHelper final {
        GLFWInitHelper();
        ~GLFWInitHelper();

        static TSharedPtr<GLFWInitHelper> get_or_create();
    };

    class Window {
    public:
        explicit Window(String in_title = L"你好，世界！", size_t width = 640, size_t height = 480);
        virtual ~Window();

        void set_title(const String& new_title);

        void poll();

        NODISCARD bool is_closed() const;

    private:
        NODISCARD bool is_valid() const;

    public:

    private:
        GLFWwindow* m_internal_window = nullptr;
        String m_window_title;
        TSharedPtr<GLFWInitHelper> m_glfw_init_helper;
    };

}// namespace rendering
