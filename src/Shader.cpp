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

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : Shader()
{
    load(vertexPath, fragmentPath);
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
    std::cout << "[Shader] Caricamento shader vertex e fragment: " << vertexPath << ", " << fragmentPath << std::endl;
    
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "[Shader] ERRORE: Impossibile leggere i file shader" << std::endl;
        throw std::runtime_error("Impossibile leggere i file shader");
    }
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    // Compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    
    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "[Shader] ERRORE: Compilazione vertex shader fallita\n" << infoLog << std::endl;
        throw std::runtime_error("Compilazione vertex shader fallita");
    }
    
    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "[Shader] ERRORE: Compilazione fragment shader fallita\n" << infoLog << std::endl;
        throw std::runtime_error("Compilazione fragment shader fallita");
    }
    
    // Shader Program
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertex);
    glAttachShader(m_programID, fragment);
    glLinkProgram(m_programID);
    
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_programID, 512, NULL, infoLog);
        std::cout << "[Shader] ERRORE: Linking shader program fallito\n" << infoLog << std::endl;
        throw std::runtime_error("Linking shader program fallito");
    }
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    std::cout << "[Shader] Shader vertex e fragment caricati con successo" << std::endl;
    
    m_isLoaded = true;
}

void Shader::use() const
{
    if (m_isLoaded)
    {
        glUseProgram(m_programID);
    }
}

void Shader::setInt(const std::string& name, GLint value) const
{
    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    if (loc >= 0)
        glUniform1i(loc, value);
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
