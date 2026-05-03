#include "render/mesh.hpp"
#include <iostream>

//constructor/destructor 

Mesh::Mesh(): m_vao(0), m_vbo(0), m_ebo(0), m_indexCount(0){

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

}//end constructor

Mesh::~Mesh() {

    cleanup();

}//end destructor

//Moving one mesh to another

Mesh::Mesh(Mesh&& other) noexcept: m_vao(other.m_vao), m_vbo(other.m_vbo), m_ebo(other.m_ebo), m_indexCount(other.m_indexCount){
    
    other.m_vao = other.m_vbo = other.m_ebo = 0;
    other.m_indexCount = 0;

}//end Mesh

Mesh& Mesh::operator=(Mesh&& other) noexcept {

    if (this != &other) {

        cleanup();
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_indexCount = other.m_indexCount;
        other.m_vao = other.m_vbo = other.m_ebo = 0;
        other.m_indexCount = 0;

    }//end if

    return *this;

}

//uploading the data to the GPU
void Mesh::upload(const std::vector<float>& vertices, const std::vector<unsigned int>& indices, const std::vector<VertexAttribute>& attributes)
{
    m_indexCount = static_cast<GLsizei>(indices.size());

    glBindVertexArray(m_vao);

    //upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    //upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    //define vertex attributes
    for (const auto& attr : attributes) {

        glEnableVertexAttribArray(attr.index);
        glVertexAttribPointer(attr.index, attr.size, attr.type, GL_FALSE, attr.stride, reinterpret_cast<void*>(attr.offset));

    }//end for

    glBindVertexArray(0);   // remember to unbind when done setting up

}//end upload

//draw!!!!

void Mesh::draw() const {
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}//end draw

void Mesh::drawInstanced(GLsizei instanceCount) const {
    glBindVertexArray(m_vao);
    glDrawElementsInstanced(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr, instanceCount);
    glBindVertexArray(0);
}//end drawInstanced

//bind/unbind

void Mesh::bind()   const { glBindVertexArray(m_vao); }
void Mesh::unbind() const { glBindVertexArray(0); }

//cleanup

void Mesh::cleanup() {

    if (m_vao) glDeleteVertexArrays(1, &m_vao);
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    m_vao = m_vbo = m_ebo = 0;

}//end clean up