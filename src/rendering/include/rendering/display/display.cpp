#pragma once

#include "rendering/display.h"
#include <memory>
#include <mutex>
#include "GLFW/glfw3.h"

using namespace rendering;

TSharedPtr<GLFWInitHelper> rendering::GLFWInitHelper::get_or_create() {
    static std::weak_ptr<GLFWInitHelper> SGlobalHelper;
    static std::mutex SLock;

    std::scoped_lock init_lock(SLock);
    if (SGlobalHelper.expired()) {
        TSharedPtr<GLFWInitHelper> new_inst = std::make_shared<GLFWInitHelper>();
        SGlobalHelper = new_inst;
        return new_inst;
    }
    return SGlobalHelper.lock();
}

GLFWInitHelper::GLFWInitHelper() {
    if (!glfwInit()) {
        throw std::runtime_error("[Display] Failed to initialize GLFW!");
    }
}

GLFWInitHelper::~GLFWInitHelper() {
    glfwTerminate();
}

Window::Window(String in_title, size_t width, size_t height) : m_window_title(std::move(in_title)), m_glfw_init_helper(GLFWInitHelper::get_or_create()) {
    CharString new_string = tools::CStringHelper::unicode_to_utf8(m_window_title);

    if (m_internal_window = glfwCreateWindow(int(width), int(height), new_string.c_str(), nullptr, nullptr); !m_internal_window) {
        throw std::runtime_error("[Display] Failed to create window.");
    }
}

void Window::set_title(const String &new_title) {
    if (is_valid()) LIKELY_BRANCH {
        m_window_title = new_title;
        CharString new_string = tools::CStringHelper::unicode_to_utf8(m_window_title);
        glfwSetWindowTitle(m_internal_window, new_string.c_str());
    }
}

bool Window::is_valid() const {
    return nullptr != m_internal_window;
}

void Window::poll() {
    if (is_valid()) LIKELY_BRANCH {
        glfwMakeContextCurrent(m_internal_window);

        glfwPollEvents();
    }
}

Window::~Window() {
    glfwDestroyWindow(m_internal_window);
    m_internal_window = nullptr;
}

bool Window::is_closed() const {
    return glfwWindowShouldClose(m_internal_window);
}
