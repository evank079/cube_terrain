#include "core/graphics_core.hpp"
#include <iostream>
#include <stdexcept>

//constructor/destructor 

GraphicsCore::GraphicsCore(int width, int height, const std::string& title)

    : m_window(nullptr)
    , m_width(width)
    , m_height(height)
    , m_title(title)
    , m_deltaTime(0.0f)
    , m_lastFrameTime(0.0)

{}//end constructor

GraphicsCore::~GraphicsCore() {

    shutdown();
    
}//end destructor

//main stuff
bool GraphicsCore::init() {

    //Initialize GLFW
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        std::cerr << "[GraphicsCore] Failed to initialize GLFW\n";
        return false;
    }

    //OpenGL version hints (4.1 core, safe for macOS gamers, i honestly dont know if
    //building will work on apple lol)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //Create the window
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);

    if (!m_window) {

        std::cerr << "[GraphicsCore] Failed to create GLFW window\n";
        glfwTerminate();
        return false;

    }//end if

    glfwMakeContextCurrent(m_window);

    //store pointer to this instance for callbacks
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);

    //load OpenGL via GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {

        std::cerr << "[GraphicsCore] Failed to initialize GLAD\n";
        return false;

    }//end if

    //default GL state
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, m_width, m_height);
    setVSync(true);

    std::cout << "[GraphicsCore] OpenGL " << glGetString(GL_VERSION) << " | " << glGetString(GL_RENDERER) << "\n";

    return true;

}//end init

void GraphicsCore::shutdown() {

    if (m_window) {

        glfwDestroyWindow(m_window);
        m_window = nullptr;

    }//end if

    glfwTerminate();

}//end shutdown

//frame gen

bool GraphicsCore::shouldClose() const {

    return glfwWindowShouldClose(m_window);

}//end shouldClose

void GraphicsCore::pollEvents() {

    glfwPollEvents();

}//end pollEvents

void GraphicsCore::beginFrame() {

    double now   = glfwGetTime();
    m_deltaTime  = static_cast<float>(now - m_lastFrameTime);
    m_lastFrameTime = now;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}//end beginFrame

void GraphicsCore::endFrame() {

    glfwSwapBuffers(m_window);

}//end endFrame

//get functions for private shit

GLFWwindow* GraphicsCore::getWindow() const { return m_window; }
int GraphicsCore::getWidth() const { return m_width; }
int GraphicsCore::getHeight() const { return m_height; }
float GraphicsCore::getAspectRatio() const { return static_cast<float>(m_width) / m_height; }
double GraphicsCore::getTime() const { return glfwGetTime(); }
float  GraphicsCore::getDeltaTime() const { return m_deltaTime; }

//settings

void GraphicsCore::setVSync(bool enabled) {

    glfwSwapInterval(enabled ? 1 : 0);

}//end setVSync

void GraphicsCore::setClearColor(float r, float g, float b, float a) {

    glClearColor(r, g, b, a);

}//end setClearColor

void GraphicsCore::setWindowTitle(const std::string& title) {

    m_title = title;
    glfwSetWindowTitle(m_window, title.c_str());

}//end setWindowTitle

//callbacks

void GraphicsCore::framebufferResizeCallback(GLFWwindow* window, int width, int height) {

    auto* core = static_cast<GraphicsCore*>(glfwGetWindowUserPointer(window));

    if (core) {

        core->m_width  = width;
        core->m_height = height;
        glViewport(0, 0, width, height);

    }//end if

}//end framebufferResizeCallback

void GraphicsCore::errorCallback(int error, const char* description) {

    std::cerr << "[GLFW Error " << error << "] " << description << "\n";

}//end errorCallback