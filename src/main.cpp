#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>
#include "Shader.h"

struct Particle {
    float position[2];
    float angle;
    float speed;
    // Per estendere a 3D: usa position[3] e un vettore direction[3] invece di angle.
};

static std::string readFile(const char* path) {
    std::ifstream file(path);
    if(!file.is_open()) throw std::runtime_error(std::string("Cannot open ")+path);
    std::stringstream ss; ss << file.rdbuf();
    return ss.str();
}

static GLuint createComputeProgram(const char* path) {
    std::string src = readFile(path);
    const char* csrc = src.c_str();
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &csrc, nullptr);
    glCompileShader(shader);
    GLint ok; glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if(!ok){
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        throw std::runtime_error(std::string("Compute compile error:\n") + log);
    }
    GLuint prog = glCreateProgram();
    glAttachShader(prog, shader);
    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if(!ok){
        char log[512];
        glGetProgramInfoLog(prog, 512, nullptr, log);
        throw std::runtime_error(std::string("Program link error:\n") + log);
    }
    glDeleteShader(shader);
    return prog;
}

static GLuint createTextureR32F(int w, int h){
    GLuint tex; glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, w, h);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

int main(){
    const int SIM_WIDTH  = 1024;
    const int SIM_HEIGHT = 768;
    const int PARTICLES  = 50000;

    if(!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SIM_WIDTH, SIM_HEIGHT, "GPU Particles", nullptr, nullptr);
    if(!window){ glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to init GLAD" << std::endl;
        return -1;
    }

    GLuint updateProg  = createComputeProgram("shaders/update.comp");
    GLuint diffuseProg = createComputeProgram("shaders/diffuse.comp");

    Shader quadShader;
    quadShader.load("shaders/final.vert", "shaders/final.frag");
    GLint uTrailLoc = glGetUniformLocation(quadShader.getID(), "uTrail");

    GLuint trailTex[2];
    trailTex[0] = createTextureR32F(SIM_WIDTH, SIM_HEIGHT);
    trailTex[1] = createTextureR32F(SIM_WIDTH, SIM_HEIGHT);
    int currentTrail = 0;

    GLuint ssbo[2];
    glGenBuffers(2, ssbo);
    size_t bufSize = PARTICLES * sizeof(Particle);
    for(int i=0;i<2;++i){
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[i]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, nullptr, GL_DYNAMIC_COPY);
    }

    std::mt19937 rng(static_cast<unsigned int>(glfwGetTime()));
    std::uniform_real_distribution<float> distX(0.0f, (float)SIM_WIDTH);
    std::uniform_real_distribution<float> distY(0.0f, (float)SIM_HEIGHT);
    std::uniform_real_distribution<float> distA(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> distS(20.0f, 80.0f);
    std::vector<Particle> init(PARTICLES);
    for(int i=0;i<PARTICLES;++i){
        init[i].position[0] = distX(rng);
        init[i].position[1] = distY(rng);
        init[i].angle = distA(rng);
        init[i].speed = distS(rng);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo[0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufSize, init.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    int currentSSBO = 0;

    GLuint vao; glGenVertexArrays(1, &vao);

    auto lastTime = std::chrono::high_resolution_clock::now();
    while(!glfwWindowShouldClose(window)){
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;

        glUseProgram(updateProg);
        glUniform1i(glGetUniformLocation(updateProg, "uParticleCount"), PARTICLES);
        glUniform1f(glGetUniformLocation(updateProg, "uDt"), dt);
        glUniform2f(glGetUniformLocation(updateProg, "uSimSize"), (float)SIM_WIDTH, (float)SIM_HEIGHT);
        glUniform1f(glGetUniformLocation(updateProg, "uSensorDistance"), 5.0f);
        glUniform1f(glGetUniformLocation(updateProg, "uRandomWeight"), 0.2f);

        int nextSSBO = 1 - currentSSBO;
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo[currentSSBO]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo[nextSSBO]);
        glBindImageTexture(2, trailTex[currentTrail], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
        GLuint groups = (PARTICLES + 127) / 128;
        glDispatchCompute(groups, 1, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
        currentSSBO = nextSSBO;

        glUseProgram(diffuseProg);
        glBindTextureUnit(0, trailTex[currentTrail]);
        int nextTrail = 1 - currentTrail;
        glBindImageTexture(1, trailTex[nextTrail], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        glUniform1f(glGetUniformLocation(diffuseProg, "uDecay"), 0.98f);
        glUniform1f(glGetUniformLocation(diffuseProg, "uDiffuse"), 0.25f);
        glDispatchCompute((SIM_WIDTH+15)/16, (SIM_HEIGHT+15)/16, 1);
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        currentTrail = nextTrail;

        glViewport(0,0,SIM_WIDTH,SIM_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
        quadShader.use();
        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, trailTex[currentTrail]);
        if(uTrailLoc >= 0) glUniform1i(uTrailLoc, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(updateProg);
    glDeleteProgram(diffuseProg);
    glDeleteTextures(2, trailTex);
    glDeleteBuffers(2, ssbo);
    glDeleteVertexArrays(1, &vao);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
