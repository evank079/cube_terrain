#pragma once

#include <glad/glad.h>
#include <string>

class ShaderLoader {
public:

    //compile & link a vertex + fragment shader into a program
    static GLuint load(const std::string& vertexPath, const std::string& fragmentPath);

    //optional extras
    static GLuint loadWithGeometry(const std::string& vertexPath, const std::string& geometryPath, const std::string& fragmentPath);

private:

    static std::string  readFile(const std::string& filepath);
    static GLuint compile(const std::string& source, GLenum type);
    static GLuint link(GLuint vertex, GLuint fragment);
    static GLuint link(GLuint vertex, GLuint geometry, GLuint fragment);
    static void checkCompileErrors(GLuint shader, const std::string& label);
    static void checkLinkErrors(GLuint program);

};