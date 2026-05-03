#include "render/shader.hpp"
#include "utils/shader_loader.hpp"

//constructors/destructor

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : m_programID(ShaderLoader::load(vertexPath, fragmentPath)) {}

Shader::Shader(const std::string& vertexPath, const std::string& geometryPath, const std::string& fragmentPath)
    : m_programID(ShaderLoader::loadWithGeometry(vertexPath, geometryPath, fragmentPath)) {}

Shader::~Shader() {

    if (m_programID) glDeleteProgram(m_programID);

}//end destructor

//move
Shader::Shader(Shader&& other) noexcept : m_programID(other.m_programID) {

    other.m_programID = 0;

}//end move

Shader& Shader::operator=(Shader&& other) noexcept {

    if (this != &other) {

        if (m_programID) glDeleteProgram(m_programID);
        m_programID = other.m_programID;
        other.m_programID = 0;

    }//end if

    return *this;

}//end move assign

//Bind / Unbind

void Shader::bind() const { glUseProgram(m_programID); }
void Shader::unbind()const { glUseProgram(0); }

//Uniform setters

void Shader::setBool (const std::string& name, bool value) const { glUniform1i(getUniformLocation(name), value ? 1 : 0); }
void Shader::setInt  (const std::string& name, int value) const { glUniform1i(getUniformLocation(name), value); }
void Shader::setFloat(const std::string& name, float value) const { glUniform1f(getUniformLocation(name), value); }

void Shader::setVec2(const std::string& name, const glm::vec2& v) const { glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(v)); }
void Shader::setVec3(const std::string& name, const glm::vec3& v) const { glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(v)); }
void Shader::setVec4(const std::string& name, const glm::vec4& v) const { glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(v)); }

void Shader::setMat3(const std::string& name, const glm::mat3& m) const { glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m)); }
void Shader::setMat4(const std::string& name, const glm::mat4& m) const { glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(m)); }

GLuint Shader::getID() const { return m_programID; }

GLint Shader::getUniformLocation(const std::string& name) const {

    return glGetUniformLocation(m_programID, name.c_str());

}//end getUniformLocation
