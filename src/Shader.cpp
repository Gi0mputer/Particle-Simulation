#include "Shader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

Shader::Shader()
    : m_programID(0)
    , m_isLoaded(false)
{
}

Shader::~Shader()
{
    if (m_isLoaded)
    {
        glDeleteProgram(m_programID);
    }
}

void Shader::load(const std::string& vertexPath, const std::string& fragmentPath)
{
    // Legge i sorgenti dai file
    std::string vertexCode   = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    // Compila i due shader
    GLuint vertID = compileShader(vertexCode,   GL_VERTEX_SHADER);
    GLuint fragID = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

    // Crea e linka il program
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertID);
    glAttachShader(m_programID, fragID);
    glLinkProgram(m_programID);

    // Controlla errori di link
    GLint success;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Shader Program link error:\n") + infoLog);
    }

    // Ora possiamo eliminare gli shader intermedi
    glDeleteShader(vertID);
    glDeleteShader(fragID);

    m_isLoaded = true;
}

void Shader::use() const
{
    if (m_isLoaded)
    {
        glUseProgram(m_programID);
    }
}

std::string Shader::readFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open shader file: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint Shader::compileShader(const std::string& source, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Controlla errori di compilazione
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        std::string shaderTypeStr;
        switch(shaderType)
        {
            case GL_VERTEX_SHADER:   shaderTypeStr = "VERTEX";   break;
            case GL_FRAGMENT_SHADER: shaderTypeStr = "FRAGMENT"; break;
            default:                 shaderTypeStr = "UNKNOWN";  break;
        }

        throw std::runtime_error("Shader compile error (" + shaderTypeStr + "):\n" + infoLog);
    }

    return shader;
}
