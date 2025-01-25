#include "ShaderProgram.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

ShaderProgram::ShaderProgram()
    : m_programID(0)
    , m_isLoaded(false)
{
}

ShaderProgram::~ShaderProgram()
{
    if (m_isLoaded) {
        glDeleteProgram(m_programID);
    }
}

void ShaderProgram::load(const std::string& vertexPath, const std::string& fragmentPath)
{
    // Leggi file
    std::string vertSource = readFile(vertexPath);
    std::string fragSource = readFile(fragmentPath);

    GLuint vertID = compileShader(vertSource, GL_VERTEX_SHADER);
    GLuint fragID = compileShader(fragSource, GL_FRAGMENT_SHADER);

    // Crea program e linka
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertID);
    glAttachShader(m_programID, fragID);
    glLinkProgram(m_programID);

    // Controlla errori di link
    GLint success;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Program link error: ") + infoLog);
    }

    // Ora puoi rimuovere gli shader
    glDeleteShader(vertID);
    glDeleteShader(fragID);

    m_isLoaded = true;
}

void ShaderProgram::use() const
{
    if (m_isLoaded) {
        glUseProgram(m_programID);
    }
}

GLuint ShaderProgram::compileShader(const std::string& source, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Controlla errori
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        throw std::runtime_error(std::string("Shader compile error: ") + infoLog);
    }
    return shader;
}

std::string ShaderProgram::readFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


//vecchia implementazione per domande:

/*

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



// ShaderProgram.cpp

*/