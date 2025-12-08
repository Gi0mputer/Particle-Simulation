#include <iostream>
#include <stdexcept>
#include <chrono>
#include <string>

// ImGui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// GLAD/GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Project Headers
#include "WindowManager.h"
#include "RenderPipeline.h"
#include "SimulationGPU.h"
#include "Utils.h"
#include "InputHandler.h"

int main(int argc, char **argv)
{
    try
    {
        //// 1. WINDOW CONTEXT
        WindowManager windowManager("Particle Simulation");
        GLFWwindow *window = windowManager.getWindow();
        
        InputHandler inputHandler(window);
        Utils::FPSCounter fpsCounter;
        
        if (!Utils::initializeGLAD()){
            throw std::runtime_error("Cannot initialize GLAD.");
        }
        Utils::configureOpenGL();

        //// 2. SETUP IMGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        // Custom Dark Style
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 8.0f;
        style.FrameRounding = 4.0f;
        style.WindowBorderSize = 0.0f;
        style.Alpha = 0.95f;
        
        // Colors: Semi-transparent black background, white text
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.85f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.90f);
        colors[ImGuiCol_Border] = ImVec4(0.3f, 0.3f, 0.3f, 0.5f);
        colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
        colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
        
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 450");

        //// 3. SIMULATION
        int simWidth, simHeight;
        Utils::SimulationManager::getSimulationSize(Utils::SimulationResolution::HD_720, simWidth, simHeight);
        std::cout << "[Simulation] Fixed Size: " << simWidth << " x " << simHeight << std::endl;
        
        SimulationGPU simulation(1000000, simWidth, simHeight);
        simulation.initialize();

        //// 4. RENDER PIPELINE
        RenderPipeline renderPipeline(simWidth, simHeight);
        renderPipeline.initialize();
       
        //// 5. TIMESTEPPER
        Utils::TimestepManager timeStepper(1.0/60.0);
        double startTime = glfwGetTime();
        timeStepper.init(startTime);

        // UI State
        bool showMenu = false;
        
        // Simulation Parameters
        struct SimulationParams {
            // Physarum
            bool physarumEnabled = true;
            float physarumIntensity = 1.0f;
            float sensorDistance = 20.0f;
            float sensorAngle = 0.785f; // 45 deg
            float turnAngle = 0.785f;
            float speed = 100.0f;
            int color1Behavior = 0; // 0=Attract, 1=Repel
            int color2Behavior = 0;
            
            // Collisions
            bool collisionsEnabled = false;
            
            // Boids
            bool boidsEnabled = false;
            float alignmentWeight = 1.0f;
            float separationWeight = 1.5f;
            float cohesionWeight = 1.0f;
            
            // Mouse
            int mouseMode = 0; // 0=Attract, 1=Repel, 2=Ring, 3=Vortex
            
            // Particles
            int targetParticleCount = 1000000;
        } params;

        //// 6. MAIN LOOP
        while (!windowManager.shouldClose())
        {
            glfwPollEvents();

            // --- IMGUI FRAME ---
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // --- HAMBURGER MENU (Top-Right Corner) ---
            {
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | 
                                                 ImGuiWindowFlags_NoMove | 
                                                 ImGuiWindowFlags_NoResize | 
                                                 ImGuiWindowFlags_NoSavedSettings |
                                                 ImGuiWindowFlags_AlwaysAutoResize;
                
                const float DISTANCE = 10.0f;
                ImVec2 work_pos = ImGui::GetMainViewport()->WorkPos;
                ImVec2 work_size = ImGui::GetMainViewport()->WorkSize;
                ImVec2 window_pos = ImVec2(work_pos.x + work_size.x - DISTANCE, work_pos.y + DISTANCE);
                ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
                ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
                ImGui::SetNextWindowBgAlpha(0.85f);

                if (ImGui::Begin("##HamburgerButton", nullptr, window_flags))
                {
                    if (ImGui::Button(showMenu ? "X" : ":::"))
                    {
                        showMenu = !showMenu;
                    }
                }
                ImGui::End();
            }

            // --- MAIN SETTINGS PANEL ---
            if (showMenu)
            {
                ImGuiWindowFlags menu_flags = ImGuiWindowFlags_NoMove | 
                                              ImGuiWindowFlags_NoResize | 
                                              ImGuiWindowFlags_AlwaysAutoResize;
                
                const float DISTANCE = 10.0f;
                ImVec2 work_pos = ImGui::GetMainViewport()->WorkPos;
                ImVec2 work_size = ImGui::GetMainViewport()->WorkSize;
                ImVec2 menu_pos = ImVec2(work_pos.x + work_size.x - DISTANCE, work_pos.y + 50.0f);
                ImVec2 menu_pivot = ImVec2(1.0f, 0.0f);
                ImGui::SetNextWindowPos(menu_pos, ImGuiCond_Always, menu_pivot);
                ImGui::SetNextWindowSize(ImVec2(350, 0), ImGuiCond_Always);
                ImGui::SetNextWindowBgAlpha(0.85f);

                if (ImGui::Begin("Settings Menu", &showMenu, menu_flags))
                {
                    ImGui::Text("FPS: %.1f (%.2f ms)", io.Framerate, 1000.0f / io.Framerate);
                    ImGui::Separator();

                    // ========== SIMULATIONS ==========
                    if (ImGui::CollapsingHeader("Simulazioni", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        // --- Physarum ---
                        if (ImGui::TreeNode("Physarum"))
                        {
                            ImGui::Checkbox("Attiva sulle particelle##physarum", &params.physarumEnabled);
                            
                            if (params.physarumEnabled) {
                                ImGui::SliderFloat("Intensita Forza", &params.physarumIntensity, 0.0f, 5.0f);
                                ImGui::SliderFloat("Distanza Sensori", &params.sensorDistance, 5.0f, 50.0f);
                                ImGui::SliderFloat("Ampiezza Sensori (deg)", &params.sensorAngle, 0.1f, 1.57f);
                                ImGui::SliderFloat("Angolo Rotazione", &params.turnAngle, 0.1f, 1.57f);
                                ImGui::SliderFloat("Velocita", &params.speed, 10.0f, 300.0f);
                                
                                ImGui::Text("Colore 1:");
                                ImGui::RadioButton("Attrae##c1", &params.color1Behavior, 0); ImGui::SameLine();
                                ImGui::RadioButton("Respinge##c1", &params.color1Behavior, 1);
                                
                                ImGui::Text("Colore 2:");
                                ImGui::RadioButton("Attrae##c2", &params.color2Behavior, 0); ImGui::SameLine();
                                ImGui::RadioButton("Respinge##c2", &params.color2Behavior, 1);
                            }
                            ImGui::TreePop();
                        }

                        // --- Collisions ---
                        if (ImGui::TreeNode("Collisioni"))
                        {
                            ImGui::Checkbox("Attive##collisions", &params.collisionsEnabled);
                            if (params.collisionsEnabled) {
                                ImGui::Text("(Implementazione futura)");
                            }
                            ImGui::TreePop();
                        }

                        // --- Boids ---
                        if (ImGui::TreeNode("Boids"))
                        {
                            ImGui::Checkbox("Attiva sulle particelle##boids", &params.boidsEnabled);
                            if (params.boidsEnabled) {
                                ImGui::SliderFloat("Allineamento", &params.alignmentWeight, 0.0f, 5.0f);
                                ImGui::SliderFloat("Separazione", &params.separationWeight, 0.0f, 5.0f);
                                ImGui::SliderFloat("Coesione", &params.cohesionWeight, 0.0f, 5.0f);
                                ImGui::Text("(Implementazione futura)");
                            }
                            ImGui::TreePop();
                        }
                    }

                    // ========== TEXTURE ==========
                    if (ImGui::CollapsingHeader("Texture"))
                    {
                        ImGui::Text("Trasformazioni Colore");
                        ImGui::Text("(Palette mapping - futuro)");
                    }

                    // ========== AUTOMATON ==========
                    if (ImGui::CollapsingHeader("Automaton"))
                    {
                        ImGui::Text("Game of Life / Custom Rules");
                        ImGui::Text("(Implementazione futura)");
                    }

                    // ========== MOUSE EFFECTS ==========
                    if (ImGui::CollapsingHeader("Effetti Mouse", ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::RadioButton("Attrai", &params.mouseMode, 0);
                        ImGui::RadioButton("Respingi", &params.mouseMode, 1);
                        ImGui::RadioButton("Anello (Attrai+Respingi)", &params.mouseMode, 2);
                        ImGui::RadioButton("Vortice (Forza Perpendicolare)", &params.mouseMode, 3);
                    }

                    // ========== PARTICLE MANAGEMENT ==========
                    if (ImGui::CollapsingHeader("Gestione Particelle"))
                    {
                        ImGui::SliderInt("Numero Particelle Target", &params.targetParticleCount, 10000, 5000000);
                        ImGui::Text("(Cambio count: richiede reinit)");
                    }
                }
                ImGui::End();
            }

            // --- UPDATE SIMULATION PARAMETERS ---
            if (params.physarumEnabled) {
                simulation.setSensorDistance(params.sensorDistance);
                simulation.setSensorAngle(params.sensorAngle);
                simulation.setTurnAngle(params.turnAngle);
                simulation.setSpeed(params.speed);
            }

            // --- INPUT & SIMULATION ---
            inputHandler.update();

            auto [mx, my] = inputHandler.getMousePos();
            double winW = (double)windowManager.getWidth();
            double winH = (double)windowManager.getHeight();
            
            double nx = mx / winW;
            double ny = (winH - my) / winH; // Inverted Y

            float simMouseX = (float)(nx * simWidth);
            float simMouseY = (float)(ny * simHeight);
            
            bool isPressed = inputHandler.isLeftClicked() && !io.WantCaptureMouse;

            // Mouse mode: 0=Attract, 1=Repel, 2=Ring, 3=Vortex
            int shaderMouseMode = params.mouseMode;

            double currentTime = glfwGetTime();
            timeStepper.update(currentTime);

            while (timeStepper.hasSteps())
            {
                double dt = timeStepper.getStepDt();
                simulation.update(static_cast<float>(dt), simMouseX, simMouseY, isPressed, shaderMouseMode);
            }

            // --- RENDER ---
            renderPipeline.render(simulation, windowManager.getWidth(), windowManager.getHeight());
            
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            windowManager.swapBuffers();
        }

        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
        return -1;
    }
}
