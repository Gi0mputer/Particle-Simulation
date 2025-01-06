#include "shaderprogram.h"
#include <fstream>
#include <sstream>
#include <iostream>

ShaderProgram::ShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    // 1) Carica i sorgenti dai file
    std::string vertexCode   = loadFile(vertexPath);
    std::string fragmentCode = loadFile(fragmentPath);

    // 2) Compila il vertex shader
    GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexCode);

    // 3) Compila il fragment shader
    GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    // 4) Crea e linka il program
    programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID);

    // Controlla errori di link
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "ERRORE di link dello shader program:\n" << infoLog << std::endl;
    }

    // 5) Elimina gli shader intermedi
    glDetachShader(programID, vertex);
    glDetachShader(programID, fragment);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(programID);
}

void ShaderProgram::use() const
{
    glUseProgram(programID);
}

std::string ShaderProgram::loadFile(const char* path)
{
    std::ifstream file(path);
    std::stringstream buf;
    buf << file.rdbuf();
    return buf.str();
}

GLuint ShaderProgram::compileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Controlla errori di compilazione
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERRORE di compilazione dello shader:\n" << infoLog << std::endl;
    }

    return shader;
}
