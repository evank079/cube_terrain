#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader {
public:

    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    Shader(const std::string& vertexPath, const std::string& geometryPath, const std::string& fragmentPath);

    ~Shader();

    // Bind / unbind
    void bind()   const;
    void unbind() const;

    // Uniform setters
    void setBool (const std::string& name, bool value) const;
    void setInt (const std::string& name, int value) const;
    void setFloat (const std::string& name, float value) const;
    void setVec2 (const std::string& name, const glm::vec2& value) const;
    void setVec3 (const std::string& name, const glm::vec3& value) const;
    void setVec4 (const std::string& name, const glm::vec4& value) const;
    void setMat3 (const std::string& name, const glm::mat3& value) const;
    void setMat4 (const std::string& name, const glm::mat4& value) const;

    GLuint getID() const;

    //no copying
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    //allow moving
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

private:

    GLuint m_programID;

    GLint getUniformLocation(const std::string& name) const;

};