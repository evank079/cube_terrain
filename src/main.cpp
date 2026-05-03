#include "core/graphics_core.hpp"
#include "render/shader.hpp"
#include "render/mesh.hpp"
#include "core/cubo.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cmath>
#include <iostream>
#include <vector>


//Theses things should be in their own class, will fix later time permitting
enum class Turn { None, PitchUp, PitchDown, YawLeft, YawRight };

struct Camera {

    glm::vec3 pos = {0.5f, 0.5f, 0.5f};   // start at a cube center
    glm::ivec3 forward = {0, 0, -1};
    glm::ivec3 up = {0, 1, 0};
    Turn queued = Turn::None;

    //Smoothing: integer forward/up are the logical state || quats are the rendered state
    glm::quat  fromOrient = glm::quat(1, 0, 0, 0);
    glm::quat  toOrient   = glm::quat(1, 0, 0, 0);
    float turnT      = 1.0f;   //1 = settled

};

static glm::quat orientFromFU(glm::vec3 fwd, glm::vec3 up) {

    glm::vec3 f = glm::normalize(fwd);
    glm::vec3 r = glm::normalize(glm::cross(f, glm::normalize(up)));
    glm::vec3 u = glm::cross(r, f);
    glm::mat3 m;
    m[0] = r;
    m[1] = u;
    m[2] = -f;
    return glm::quat_cast(m);

}//end orientFromFU

static void applyTurn(Camera& c) {
    glm::ivec3 right = glm::ivec3(glm::cross(glm::vec3(c.forward), glm::vec3(c.up)));

    switch (c.queued) {

        case Turn::PitchUp: { auto f = c.forward; c.forward =  c.up;  c.up = -f; break; }
        case Turn::PitchDown: { auto f = c.forward; c.forward = -c.up;  c.up =  f; break; }
        case Turn::YawLeft: c.forward = -right; break;
        case Turn::YawRight: c.forward =  right; break;
        default: break;

    }//end switch

    c.queued = Turn::None;

}//end applyTurn

int main() {

    //Window
    GraphicsCore gfx(1280, 720, "cube_terrain");

    if (!gfx.init()) {

        std::cerr << "[Main] Failed to initialize graphics core\n";
        return -1;

    }//end if

    gfx.setClearColor(0.1f, 0.1f, 0.15f);

    //shader
    Shader shader("shaders/basic.vert","shaders/basic.frag");

    //geometry generation, one subdivided cube reused for every lattice instance
    constexpr int   kSubdiv    = 69;
    constexpr float kMinHeight = 0.0f;
    constexpr float kMaxHeight = 0.30f;   //inward displacement, max safe value is < 0.5 

    cubo c(0, 0, 0, kSubdiv, 5);
    std::vector<cubo::Vertex>  cubeVerts;
    std::vector<unsigned int>  indices;
    c.buildCube(cubeVerts, indices, kSubdiv, kMinHeight, kMaxHeight);

    std::vector<float> vertices;
    vertices.reserve(cubeVerts.size() * 7);
    for (const auto& v : cubeVerts) {

        vertices.push_back(v.position.x); vertices.push_back(v.position.y); vertices.push_back(v.position.z);
        vertices.push_back(v.normal.x);   vertices.push_back(v.normal.y);   vertices.push_back(v.normal.z);
        vertices.push_back(v.displacement);

    }//end for loop

    constexpr GLsizei kStride = 7 * sizeof(float);
    std::vector<VertexAttribute> attributes = {

        { 0, 3, GL_FLOAT, kStride, 0 },
        { 1, 3, GL_FLOAT, kStride, 3 * sizeof(float) },
        { 2, 1, GL_FLOAT, kStride, 6 * sizeof(float) },

    };

    Mesh mesh;
    mesh.upload(vertices, indices, attributes);

    //camera + lattice settings
    Camera cam;
    constexpr float kSpeed = 0.5f; // cubes per second
    constexpr int   kRadius = 8; // (2*kRadius+1)^3 cubes drawn each frame
    constexpr float kCullDistance = (float)kRadius; // cubes farther than this from the camera are skipped
    constexpr float kCullDistSq = kCullDistance * kCullDistance;
    constexpr float kTurnDuration = 0.5f; // seconds to slerp through a 90° turn

    glm::mat4 projection = glm::perspective(glm::radians(60.0f), gfx.getAspectRatio(), 0.1f, 100.0f);

    GLFWwindow* win = gfx.getWindow();
    int pW = GLFW_RELEASE, pS = GLFW_RELEASE, pA = GLFW_RELEASE, pD = GLFW_RELEASE;

    //Render Loop
    while (!gfx.shouldClose()) {

        gfx.pollEvents();
        gfx.beginFrame();

        float dt = gfx.getDeltaTime();

        //movement, Held key only queues one turn
        int w = glfwGetKey(win, GLFW_KEY_W);
        int s = glfwGetKey(win, GLFW_KEY_S);
        int a = glfwGetKey(win, GLFW_KEY_A);
        int d = glfwGetKey(win, GLFW_KEY_D);
        if (w == GLFW_PRESS && pW == GLFW_RELEASE) cam.queued = Turn::PitchUp;
        if (s == GLFW_PRESS && pS == GLFW_RELEASE) cam.queued = Turn::PitchDown;
        if (a == GLFW_PRESS && pA == GLFW_RELEASE) cam.queued = Turn::YawLeft;
        if (d == GLFW_PRESS && pD == GLFW_RELEASE) cam.queued = Turn::YawRight;
        pW = w; pS = s; pA = a; pD = d;

        //apply any queued turn the moment we cross a cube center
        int axis = cam.forward.x ? 0 : (cam.forward.y ? 1 : 2);
        int beforeCell = (int)std::floor(cam.pos[axis] - 0.5f);
        cam.pos += kSpeed * dt * glm::vec3(cam.forward);
        int afterCell  = (int)std::floor(cam.pos[axis] - 0.5f);

        if (beforeCell != afterCell && cam.queued != Turn::None) {

            cam.pos[axis] = (float)afterCell + 0.5f;
            //capture in-progress orientation as the new "from" so interrupted turns blend smoothly
            cam.fromOrient = glm::slerp(cam.fromOrient, cam.toOrient, cam.turnT);
            applyTurn(cam);
            cam.toOrient = orientFromFU(glm::vec3(cam.forward), glm::vec3(cam.up));
            cam.turnT    = 0.0f;
            
        }//end if

        cam.turnT = std::min(1.0f, cam.turnT + dt / kTurnDuration);
        glm::quat smoothed = glm::slerp(cam.fromOrient, cam.toOrient, cam.turnT);
        glm::vec3 fwd      = smoothed * glm::vec3(0, 0, -1);
        glm::vec3 up       = smoothed * glm::vec3(0, 1,  0);
        glm::mat4 view     = glm::lookAt(cam.pos, cam.pos + fwd, up);

        //bind shader and set per-frame uniforms
        shader.bind();
        shader.setMat4 ("view",       view);
        shader.setMat4 ("projection", projection);
        shader.setFloat("uMinHeight", kMinHeight);
        shader.setFloat("uMaxHeight", kMaxHeight);

        //draw a re-centered cube lattice around the camera
        glm::ivec3 anchor = glm::ivec3(glm::floor(cam.pos));
        for (int dx = -kRadius; dx <= kRadius; ++dx)
        for (int dy = -kRadius; dy <= kRadius; ++dy)
        for (int dz = -kRadius; dz <= kRadius; ++dz) {

            glm::ivec3 cell       = anchor + glm::ivec3(dx, dy, dz);
            glm::vec3  cubeCenter = glm::vec3(cell) + 0.5f;
            glm::vec3  delta      = cubeCenter - cam.pos;
            if (glm::dot(delta, delta) > kCullDistSq) continue;

            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(cell));
            shader.setMat4("model", model);
            mesh.draw();

        }//end for loop

        shader.unbind();
        gfx.endFrame();

    }//end while loop

    return 0;   //destructors called

}//end main
