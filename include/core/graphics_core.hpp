#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class GraphicsCore {
public:

    GraphicsCore(int width, int height, const std::string& title);
    ~GraphicsCore();

    //startup/shutdown
    bool init();
    void shutdown();
    bool shouldClose() const;

    //frames
    void beginFrame();
    void endFrame();
    void pollEvents();

    //get methods
    GLFWwindow* getWindow() const;
    int getWidth() const;
    int getHeight() const;
    float getAspectRatio() const;
    double getTime() const;
    float getDeltaTime() const;

    //settings
    void setVSync(bool enabled);
    void setClearColor(float r, float g, float b, float a = 1.0f);
    void setWindowTitle(const std::string& title);

    //other
    GraphicsCore(const GraphicsCore&) = delete;
    GraphicsCore& operator=(const GraphicsCore&) = delete;

private:

    //GLFW callbacks (static so GLFW can call them)
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void errorCallback(int error, const char* description);

    GLFWwindow* m_window;
    int m_width;
    int m_height;
    std::string m_title;

    float  m_deltaTime;
    double m_lastFrameTime;

};