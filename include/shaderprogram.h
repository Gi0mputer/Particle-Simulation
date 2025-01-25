#pragma once
#include <string>
#include <glad/glad.h>

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    // Carica, compila e linka i due shader (vertex + fragment)
    void load(const std::string& vertexPath, const std::string& fragmentPath);

    // Attiva il programma
    void use() const;

    // Restituisce l’ID del program (se serve)
    GLuint getID() const { return m_programID; }

private:
    GLuint m_programID;
    bool m_isLoaded;

    // Se vuoi funzioni private per compilare e linkare
    GLuint compileShader(const std::string& source, GLenum shaderType);
    std::string readFile(const std::string& filePath);
};