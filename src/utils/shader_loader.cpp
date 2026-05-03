#include "utils/shader_loader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>


GLuint ShaderLoader::load(const std::string& vertexPath, const std::string& fragmentPath) {

    std::string vertSrc = readFile(vertexPath);
    std::string fragSrc = readFile(fragmentPath);

    GLuint vert = compile(vertSrc, GL_VERTEX_SHADER);
    GLuint frag = compile(fragSrc, GL_FRAGMENT_SHADER);
    GLuint prog = link(vert, frag);

    // Shaders are linked into the program — no longer needed
    glDeleteShader(vert);
    glDeleteShader(frag);

    return prog;

}//end load

GLuint ShaderLoader::loadWithGeometry(const std::string& vertexPath, const std::string& geometryPath, const std::string& fragmentPath) {
    std::string vertSrc = readFile(vertexPath);
    std::string geomSrc = readFile(geometryPath);
    std::string fragSrc = readFile(fragmentPath);

    GLuint vert = compile(vertSrc, GL_VERTEX_SHADER);
    GLuint geom = compile(geomSrc, GL_GEOMETRY_SHADER);
    GLuint frag = compile(fragSrc, GL_FRAGMENT_SHADER);
    GLuint prog = link(vert, geom, frag);

    glDeleteShader(vert);
    glDeleteShader(geom);
    glDeleteShader(frag);

    return prog;

}//end loadWithGeometry



std::string ShaderLoader::readFile(const std::string& filepath) {
    std::ifstream file(filepath);

    if (!file.is_open())
        throw std::runtime_error("[ShaderLoader] Cannot open file: " + filepath);

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();

}//end readFile

GLuint ShaderLoader::compile(const std::string& source, GLenum type) {

    const char* src = source.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    //label for error messages
    std::string label;
    switch (type) {

        case GL_VERTEX_SHADER: label = "VERTEX";   break;
        case GL_FRAGMENT_SHADER: label = "FRAGMENT"; break;
        case GL_GEOMETRY_SHADER: label = "GEOMETRY"; break;
        default: label = "UNKNOWN";  break;

    }//end switch

    checkCompileErrors(shader, label);
    return shader;

}//end compile

GLuint ShaderLoader::link(GLuint vertex, GLuint fragment) {

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    checkLinkErrors(program);
    return program;

}//end link

GLuint ShaderLoader::link(GLuint vertex, GLuint geometry, GLuint fragment) {

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, geometry);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    checkLinkErrors(program);
    return program;

}//end link overload

void ShaderLoader::checkCompileErrors(GLuint shader, const std::string& label) {
    GLint  success;
    GLchar log[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {

        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        throw std::runtime_error(
            "[ShaderLoader] Compile error (" + label + "):\n" + log
        );

    }//end if

}//end checkCompileErrors

void ShaderLoader::checkLinkErrors(GLuint program) {

    GLint  success;
    GLchar log[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        throw std::runtime_error(
            std::string("[ShaderLoader] Link error:\n") + log
        );
    }//end if

}//end checkLinkErrors