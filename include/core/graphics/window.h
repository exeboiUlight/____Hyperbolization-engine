#pragma once

#include <GLFW/glfw3.h>

namespace Engine {
    class Window {
        private:
            GLFWwindow* window;
        
        public:
            Window(int width, int height, const char* title) {
                window = glfwCreateWindow(width, height, title, NULL, NULL);
            }
            
            ~Window() {
                if (window) {
                    glfwDestroyWindow(window);
                }
            }
            
            bool shouldClose() const {
                return glfwWindowShouldClose(window);
            }
            
            void swapBuffers() {
                glfwSwapBuffers(window);
            }
            
            void pollEvents() {
                glfwPollEvents();
            }
            
            GLFWwindow* getGLFWWindow() const {
                return window;
            }
    };
}