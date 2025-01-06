#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include <glad/glad.h>

class ShaderProgram {
public:
    // Costruttore: riceve i path ai file .vert e .frag
    ShaderProgram(const char* vertexPath, const char* fragmentPath);
    ~ShaderProgram();

    void use() const;

    GLuint getID() const { return programID; }

private:
    GLuint programID;

    std::string loadFile(const char* path);
    GLuint compileShader(GLenum type, const std::string& source);
};

#endif // SHADER_PROGRAM_H
