#pragma once

#include <glad/glad.h>
#include <vector>
#include <cstddef>

//mainly for binding VAO/VBO

//describes a single vertex attribute slot
struct VertexAttribute {
    GLuint   index;       // layout(location = N)
    GLint    size;        // number of components (1–4)
    GLenum   type;        // GL_FLOAT, GL_UNSIGNED_INT, etc.
    GLsizei  stride;      // total size of one vertex in bytes
    size_t   offset;      // byte offset of this attribute within the vertex
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    //upload geometry to the GPU
    void upload(const std::vector<float>&        vertices,
                const std::vector<unsigned int>& indices,
                const std::vector<VertexAttribute>& attributes);

    //draw the mesh
    void draw() const;
    void drawInstanced(GLsizei instanceCount) const;

    //bind/unbind manually if needed
    void bind() const;
    void unbind() const;

    //prevents copying (GPU resources aren't copyable)
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    //allow moving
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    GLsizei m_indexCount;

    void cleanup();
    
};