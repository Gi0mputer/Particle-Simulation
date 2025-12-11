#include <iostream>
#include <stdexcept>
#include <chrono>
#include <string>
#include <algorithm>
#include <limits>
#include <cfloat>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <cstdio>

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
#include "GpuDiagnostics.h"
#include "Utils.h"
#include "InputHandler.h"

int main(int argc, char **argv)
{
    try
    {
        // 0. GPU DIAGNOSTICS (prima di creare la finestra)
        const auto gpuAdapters = Utils::enumerateGpuAdapters();
        const auto preferredGpu = Utils::pickBestGpu(gpuAdapters);
        Utils::logGpuAdapters(gpuAdapters, preferredGpu);

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
        io.IniFilename = nullptr; // Disable imgui.ini
        
        // FONT SIZE - Slightly smaller to avoid clipping in narrow panels
        io.FontGlobalScale = 1.45f;
        
        // Premium Dark Style
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        
        // Window styling
        style.WindowRounding = 0.0f;
        style.WindowBorderSize = 0.0f;
        style.WindowPadding = ImVec2(20, 20);
        style.WindowMinSize = ImVec2(500, 600);
        
        // Frame styling
        style.FrameRounding = 6.0f;
        style.FramePadding = ImVec2(12, 8);
        style.FrameBorderSize = 0.0f;
        
        // Item styling
        style.ItemSpacing = ImVec2(12, 10);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.IndentSpacing = 25.0f;
        style.ScrollbarSize = 12.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabMinSize = 12.0f;
        style.GrabRounding = 6.0f;
        
        // Tab styling
        style.TabRounding = 6.0f;
        
        // Colors: Premium dark theme
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.12f, 0.95f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.12f, 0.95f);
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.28f, 0.50f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        
        // Text
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.97f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.52f, 1.00f);
        
        // Headers (TreeNode, CollapsingHeader)
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.28f, 0.35f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.32f, 0.40f, 1.00f);
        
        // Buttons
        colors[ImGuiCol_Button] = ImVec4(0.22f, 0.24f, 0.28f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.32f, 0.38f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.37f, 0.45f, 1.00f);
        
        // Frame backgrounds
        colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.26f, 0.32f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.30f, 0.36f, 1.00f);
        
        // Tabs
        colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.20f, 0.24f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.32f, 0.38f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.24f, 0.26f, 0.32f, 1.00f);
        
        // Title
        colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.16f, 0.22f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.14f, 0.18f, 0.75f);
        
        // Scrollbar
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.12f, 0.14f, 0.18f, 0.50f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.30f, 0.36f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.37f, 0.45f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.42f, 0.52f, 1.00f);
        
        // Slider/Check/Radio
        colors[ImGuiCol_CheckMark] = ImVec4(0.70f, 0.72f, 0.80f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.60f, 0.62f, 0.70f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.75f, 0.77f, 0.85f, 1.00f);
        
        // Separator
        colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.28f, 0.50f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.35f, 0.35f, 0.40f, 0.75f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.45f, 0.45f, 0.52f, 1.00f);
        
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 450");

        //// 3. SIMULATION
        int simWidth, simHeight;
        Utils::SimulationManager::getSimulationSize(Utils::SimulationResolution::FHD_1080, simWidth, simHeight);
        std::cout << "[Simulation] 1080p Resolution: " << simWidth << " x " << simHeight << std::endl;

        // Simulation Parameters (UI state)
        struct SimulationParams {
            // Physarum
            bool physarumEnabled = false;
            float physarumIntensity = 1.0f;
            float sensorDistance = 20.0f;
            float sensorAngle = 0.785f;
            float turnAngle = 0.785f;
            float speedMin = 10.0f;
            float speedMax = 300.0f;
            float speed = 100.0f;
            float trailFade = 0.99f;
            float toneExposure = 3.0f;
            float autoDimThreshold = 0.25f;
            float autoDimStrength = 0.5f;
            float autoDimGlobal = 4.0f;

            // Colors

            float color1[3] = {0.0f, 1.0f, 1.0f};
            float color2[3] = {1.0f, 0.0f, 1.0f};
            float backgroundColor[3] = {0.0f, 0.0f, 0.0f};
            float colorOffset = 0.0f;
            int   colorSource = 0;       // 0=angle, 1=speed manual, 2=speed auto
            float colorSpeedMin = 0.0f;
            float colorSpeedMax = 300.0f;
            int   colorMode = 0;
            float neonSpeed = 1.0f;
            float neonRange = 1.0f;
            
            // Boids
            bool boidsEnabled = false;
            float alignment = 1.0f;
            float separation = 1.2f;
            float cohesion = 1.0f;
            float radius = 50.0f;
            float inertia = 0.85f;
            float restitution = 1.0f;
            
            // Collisions
            bool collisionsEnabled = false;
            float collisionRadius = 40.0f;
            
            // Boundaries
            int boundaryMode = 0;

            // Mouse
            int mouseMode = 0;
            int mouseFalloff = 1; // 0=1/r,1=1/r^2,2=1/r^3,3=gauss,4=oscillatory
            float mouseStrength = 1.0f;
            float mouseGaussianSigma = 250.0f; // pixel radius for gaussian
            float mouseOscFreq = 0.5f; // cycles per 1000px approx (scaled)
            bool mouseRingOverlay = false;
            float mouseRingRadius = 400.0f;
            
            // Particles
            int targetParticleCount = 1000000;
            
            // Texture / Resolution
            int resolutionPreset = 1; // 0=720, 1=1080, 2=1440, 3=4K
            int textureFormat = 2;    // 0=R8, 1=RG8, 2=RGBA8
        } params;
        
        constexpr int maxParticles = 5000000;
        const int defaultParticles = 1000000;
        params.targetParticleCount = defaultParticles;

        const std::string configDir = "configs";
        const std::string defaultConfigPath = configDir + "/default.cfg";
        const Utils::HardwareInfo hardwareInfo = Utils::getHardwareInfo();
        if (!hardwareInfo.renderer.empty())
        {
            std::cout << "[GPU] Renderer OpenGL attivo: " << hardwareInfo.renderer << std::endl;
        }
        const bool usingPreferredGpu = preferredGpu && Utils::rendererMatchesAdapter(*preferredGpu, hardwareInfo.renderer);
        const std::string preferredGpuLabel = preferredGpu
            ? preferredGpu->name + " (" + Utils::formatMemoryMB(preferredGpu->dedicatedMemory) + ")"
            : "non rilevata";
        if (preferredGpu && !usingPreferredGpu)
        {
            std::cout << "[GPU] Attenzione: stai renderizzando con \"" << hardwareInfo.renderer
                      << "\" ma la GPU consigliata e' \"" << preferredGpu->name << "\"." << std::endl;
        }
        const std::string hardwarePresetName = Utils::makeHardwarePresetName(hardwareInfo);
        const std::string hardwarePresetPath = configDir + "/" + hardwarePresetName + ".cfg";
        std::string currentPresetLabel = "default (non salvato)";
        std::string presetStatus = "Nessun preset caricato";
        static char presetNameBuf[64] = "my_preset";

        auto clampParams = [&](SimulationParams& p) {
            constexpr float kSpeedMaxCap = 1e9f; // effectively "no cap" for UI max
            p.sensorDistance = std::clamp(p.sensorDistance, 1.0f, 500.0f);
            p.sensorAngle    = std::clamp(p.sensorAngle, 0.05f, 1.57f);
            p.turnAngle      = std::clamp(p.turnAngle, 0.05f, 1.57f);
            p.speedMin       = std::clamp(p.speedMin, 0.0f, kSpeedMaxCap);
            p.speedMax       = std::clamp(p.speedMax, p.speedMin + 1.0f, kSpeedMaxCap);
            p.speed          = std::clamp(p.speed, p.speedMin, p.speedMax);
            p.physarumIntensity = std::clamp(p.physarumIntensity, 0.0f, 5.0f);
            p.trailFade      = std::clamp(p.trailFade, 0.5f, 0.9999f);
            p.toneExposure   = std::clamp(p.toneExposure, 0.01f, 20.0f);
            p.autoDimThreshold = std::clamp(p.autoDimThreshold, 0.0f, 1.0f);
            p.autoDimStrength  = std::clamp(p.autoDimStrength, 0.0f, 1.0f);
            p.autoDimGlobal    = std::clamp(p.autoDimGlobal, 0.0f, 20.0f);

            p.alignment  = std::clamp(p.alignment, 0.0f, 2.0f);
            p.separation = std::clamp(p.separation, 0.0f, 2.0f);
            p.cohesion   = std::clamp(p.cohesion, 0.0f, 2.0f);
            p.radius     = std::clamp(p.radius, 5.0f, 300.0f);
            p.inertia    = std::clamp(p.inertia, 0.0f, 0.99f);
            p.restitution= std::clamp(p.restitution, 0.0f, 1.5f);

            p.collisionRadius = std::clamp(p.collisionRadius, 5.0f, 400.0f);
            p.boundaryMode = std::clamp(p.boundaryMode, 0, 2);
            p.mouseMode = std::clamp(p.mouseMode, 0, 3);
            p.targetParticleCount = std::clamp(p.targetParticleCount, 10000, maxParticles);
            p.colorOffset = std::clamp(p.colorOffset, 0.0f, 1.0f);
            p.colorMode   = std::clamp(p.colorMode, 0, 4);
            p.neonSpeed   = std::clamp(p.neonSpeed, 0.0f, 10.0f);
            p.neonRange   = std::clamp(p.neonRange, 0.01f, 10.0f);
            for (int i = 0; i < 3; ++i) {
                p.backgroundColor[i] = std::clamp(p.backgroundColor[i], 0.0f, 1.0f);
            }
            p.colorSource = std::clamp(p.colorSource, 0, 2);
            p.colorSpeedMin = std::clamp(p.colorSpeedMin, 0.0f, 2000.0f);
            p.colorSpeedMax = std::clamp(p.colorSpeedMax, p.colorSpeedMin + 1.0f, 5000.0f);
            p.mouseMode = std::clamp(p.mouseMode, 0, 3);
            p.mouseFalloff = std::clamp(p.mouseFalloff, 0, 4);
            p.mouseStrength = std::clamp(p.mouseStrength, 0.0f, 10.0f);
            p.mouseGaussianSigma = std::clamp(p.mouseGaussianSigma, 10.0f, 5000.0f);
            p.mouseOscFreq = std::clamp(p.mouseOscFreq, 0.01f, 20.0f);
            p.mouseOscFreq = std::clamp(p.mouseOscFreq, 0.01f, 20.0f);
            p.mouseRingRadius = std::clamp(p.mouseRingRadius, 10.0f, 5000.0f);
            p.resolutionPreset = std::clamp(p.resolutionPreset, 0, 3);
            p.textureFormat = std::clamp(p.textureFormat, 0, 2);
        };

        auto saveParamsToFile = [&](const SimulationParams& p, const std::string& path) -> bool {
            SimulationParams data = p;
            clampParams(data);
            try {
                std::filesystem::path fsPath(path);
                if (fsPath.has_parent_path()) {
                    std::filesystem::create_directories(fsPath.parent_path());
                }
                std::ofstream out(path);
                if (!out.is_open()) return false;
                out << "physarumEnabled " << (data.physarumEnabled ? 1 : 0) << "\n";
                out << "physarumIntensity " << data.physarumIntensity << "\n";
                out << "sensorDistance " << data.sensorDistance << "\n";
                out << "sensorAngle " << data.sensorAngle << "\n";
                out << "turnAngle " << data.turnAngle << "\n";
                out << "speedMin " << data.speedMin << "\n";
                out << "speedMax " << data.speedMax << "\n";
                out << "speed " << data.speed << "\n";
                out << "trailFade " << data.trailFade << "\n";
                out << "toneExposure " << data.toneExposure << "\n";
                out << "autoDimThreshold " << data.autoDimThreshold << "\n";
                out << "autoDimStrength " << data.autoDimStrength << "\n";
                out << "autoDimGlobal " << data.autoDimGlobal << "\n";
                out << "color1 " << data.color1[0] << " " << data.color1[1] << " " << data.color1[2] << "\n";
                out << "color2 " << data.color2[0] << " " << data.color2[1] << " " << data.color2[2] << "\n";
                out << "backgroundColor " << data.backgroundColor[0] << " " << data.backgroundColor[1] << " " << data.backgroundColor[2] << "\n";
                out << "colorOffset " << data.colorOffset << "\n";
                out << "colorSource " << data.colorSource << "\n";
                out << "colorSpeedMin " << data.colorSpeedMin << "\n";
                out << "colorSpeedMax " << data.colorSpeedMax << "\n";
                out << "colorMode " << data.colorMode << "\n";
                out << "neonSpeed " << data.neonSpeed << "\n";
                out << "neonRange " << data.neonRange << "\n";
                out << "boidsEnabled " << (data.boidsEnabled ? 1 : 0) << "\n";
                out << "alignment " << data.alignment << "\n";
                out << "separation " << data.separation << "\n";
                out << "cohesion " << data.cohesion << "\n";
                out << "radius " << data.radius << "\n";
                out << "inertia " << data.inertia << "\n";
                out << "restitution " << data.restitution << "\n";
                out << "collisionsEnabled " << (data.collisionsEnabled ? 1 : 0) << "\n";
                out << "collisionRadius " << data.collisionRadius << "\n";
                out << "boundaryMode " << data.boundaryMode << "\n";
                out << "mouseMode " << data.mouseMode << "\n";
                out << "mouseFalloff " << data.mouseFalloff << "\n";
                out << "mouseStrength " << data.mouseStrength << "\n";
                out << "mouseGaussianSigma " << data.mouseGaussianSigma << "\n";
                out << "mouseOscFreq " << data.mouseOscFreq << "\n";
                out << "mouseRingOverlay " << (data.mouseRingOverlay ? 1 : 0) << "\n";
                out << "mouseRingRadius " << data.mouseRingRadius << "\n";
                out << "targetParticleCount " << data.targetParticleCount << "\n";
                out << "resolutionPreset " << data.resolutionPreset << "\n";
                out << "textureFormat " << data.textureFormat << "\n";
                return true;
            } catch (...) {
                return false;
            }
        };

        auto loadParamsFromFile = [&](SimulationParams& p, const std::string& path) -> bool {
            std::ifstream in(path);
            if (!in.is_open()) return false;
            std::string line, key;
            while (std::getline(in, line)) {
                std::istringstream iss(line);
                if (!(iss >> key)) continue;
                if (key == "physarumEnabled") { int v; if (iss >> v) p.physarumEnabled = (v != 0); }
                else if (key == "physarumIntensity") iss >> p.physarumIntensity;
                else if (key == "sensorDistance") iss >> p.sensorDistance;
                else if (key == "sensorAngle") iss >> p.sensorAngle;
                else if (key == "turnAngle") iss >> p.turnAngle;
                else if (key == "speedMin") iss >> p.speedMin;
                else if (key == "speedMax") iss >> p.speedMax;
                else if (key == "speed") iss >> p.speed;
                else if (key == "trailFade") iss >> p.trailFade;
                else if (key == "toneExposure") iss >> p.toneExposure;
                else if (key == "autoDimThreshold") iss >> p.autoDimThreshold;
                else if (key == "autoDimStrength") iss >> p.autoDimStrength;
                else if (key == "autoDimGlobal") iss >> p.autoDimGlobal;
                else if (key == "color1") iss >> p.color1[0] >> p.color1[1] >> p.color1[2];
                else if (key == "color2") iss >> p.color2[0] >> p.color2[1] >> p.color2[2];
                else if (key == "backgroundColor") iss >> p.backgroundColor[0] >> p.backgroundColor[1] >> p.backgroundColor[2];
                else if (key == "colorOffset") iss >> p.colorOffset;
                else if (key == "colorSource") iss >> p.colorSource;
                else if (key == "colorSpeedMin") iss >> p.colorSpeedMin;
                else if (key == "colorSpeedMax") iss >> p.colorSpeedMax;
                else if (key == "colorMode") iss >> p.colorMode;
                else if (key == "neonSpeed") iss >> p.neonSpeed;
                else if (key == "neonRange") iss >> p.neonRange;
                else if (key == "boidsEnabled") { int v; if (iss >> v) p.boidsEnabled = (v != 0); }
                else if (key == "alignment") iss >> p.alignment;
                else if (key == "separation") iss >> p.separation;
                else if (key == "cohesion") iss >> p.cohesion;
                else if (key == "radius") iss >> p.radius;
                else if (key == "inertia") iss >> p.inertia;
                else if (key == "restitution") iss >> p.restitution;
                else if (key == "collisionsEnabled") { int v; if (iss >> v) p.collisionsEnabled = (v != 0); }
                else if (key == "collisionRadius") iss >> p.collisionRadius;
                else if (key == "boundaryMode") iss >> p.boundaryMode;
                else if (key == "mouseMode") iss >> p.mouseMode;
                else if (key == "mouseFalloff") iss >> p.mouseFalloff;
                else if (key == "mouseStrength") iss >> p.mouseStrength;
                else if (key == "mouseGaussianSigma") iss >> p.mouseGaussianSigma;
                else if (key == "mouseOscFreq") iss >> p.mouseOscFreq;
                else if (key == "mouseRingOverlay") { int v; if (iss >> v) p.mouseRingOverlay = (v != 0); }
                else if (key == "mouseRingRadius") iss >> p.mouseRingRadius;
                else if (key == "targetParticleCount") iss >> p.targetParticleCount;
                else if (key == "resolutionPreset") iss >> p.resolutionPreset;
                else if (key == "textureFormat") iss >> p.textureFormat;
            }
            clampParams(p);
            return true;
        };

        std::filesystem::create_directories(configDir);
        bool presetLoaded = false;
        if (std::filesystem::exists(hardwarePresetPath) && loadParamsFromFile(params, hardwarePresetPath)) {
            currentPresetLabel = hardwarePresetName + ".cfg";
            presetStatus = "Preset hardware caricato";
            presetLoaded = true;
        }
        if (!presetLoaded && std::filesystem::exists(defaultConfigPath) && loadParamsFromFile(params, defaultConfigPath)) {
            currentPresetLabel = "default.cfg";
            presetStatus = "Caricato default.cfg (fallback)";
            presetLoaded = true;
        }
        if (!presetLoaded) {
            clampParams(params);
            presetStatus = "Usando impostazioni di default hardcoded";
        }

        SimulationGPU simulation(maxParticles, simWidth, simHeight);
        simulation.initialize();
        simulation.setActiveParticleCount(params.targetParticleCount);

        //// 4. RENDER PIPELINE
        RenderPipeline renderPipeline(simWidth, simHeight);
        renderPipeline.initialize();
       
        //// 5. TIMESTEPPER
        Utils::TimestepManager timeStepper(1.0/60.0);
        double startTime = glfwGetTime();
        timeStepper.init(startTime);

        // UI State
        bool showMenu = false;

        //// 6. MAIN LOOP
        while (!windowManager.shouldClose())
        {
            glfwPollEvents();

            // --- IMGUI FRAME ---
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // --- HAMBURGER MENU BUTTON (Only visible when menu is CLOSED) ---
            if (!showMenu)
            {
                ImGuiWindowFlags btn_flags = ImGuiWindowFlags_NoDecoration | 
                                             ImGuiWindowFlags_NoMove | 
                                             ImGuiWindowFlags_NoResize | 
                                             ImGuiWindowFlags_NoSavedSettings |
                                             ImGuiWindowFlags_AlwaysAutoResize |
                                             ImGuiWindowFlags_NoBackground;
                
                ImVec2 work_pos = ImGui::GetMainViewport()->WorkPos;
                ImVec2 work_size = ImGui::GetMainViewport()->WorkSize;
                ImVec2 btn_pos = ImVec2(work_pos.x + work_size.x - 100, work_pos.y + 15);
                ImGui::SetNextWindowPos(btn_pos, ImGuiCond_Always);

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
                if (ImGui::Begin("##MenuButton", nullptr, btn_flags))
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.18f, 0.9f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.30f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.30f, 0.30f, 0.38f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.95f, 0.95f, 0.97f, 1.0f));
                    
                    if (ImGui::Button(" MENU ", ImVec2(80, 50)))
                    {
                        showMenu = true;
                    }
                    
                    ImGui::PopStyleColor(4);
                }
                ImGui::End();
                ImGui::PopStyleVar();
            }

            // --- MAIN SETTINGS PANEL (Full Height, Right-Aligned) ---
            if (showMenu)
            {
                ImGuiWindowFlags menu_flags = ImGuiWindowFlags_NoMove | 
                                              ImGuiWindowFlags_NoResize | 
                                              ImGuiWindowFlags_NoCollapse |
                                              ImGuiWindowFlags_NoTitleBar |
                                              ImGuiWindowFlags_NoScrollbar;
                
                ImVec2 work_pos = ImGui::GetMainViewport()->WorkPos;
                ImVec2 work_size = ImGui::GetMainViewport()->WorkSize;
                
                // Adaptive width: at least 720px, up to 90% of viewport, scaled to 42% by default
                float menuWidth = std::min(std::max(720.0f, work_size.x * 0.42f), work_size.x * 0.90f);
                float menuHeight = work_size.y;
                
                ImVec2 menu_pos = ImVec2(work_pos.x + work_size.x - menuWidth, work_pos.y);
                ImGui::SetNextWindowPos(menu_pos, ImGuiCond_Always);
                ImGui::SetNextWindowSize(ImVec2(menuWidth, menuHeight), ImGuiCond_Always);
                ImGui::SetNextWindowSizeConstraints(ImVec2(720.0f, 400.0f), ImVec2(FLT_MAX, FLT_MAX));
                ImGui::SetNextWindowBgAlpha(0.96f);

                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(25, 25));
                if (ImGui::Begin("Settings", nullptr, menu_flags))
                {
                    // Let widgets use the full available width
                    ImGui::PushItemWidth(-FLT_MIN);
                    // Close button (top right inside menu)
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.15f, 0.15f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.2f, 0.2f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.25f, 0.25f, 1.0f));
                    
                    float hideButtonWidth = 90.0f;
                    float fullscreenButtonWidth = 140.0f;
                    float padding = 10.0f; // Padding between buttons
                    ImGui::SetCursorPosX(menuWidth - hideButtonWidth - fullscreenButtonWidth - padding - 25);
                    if (ImGui::Button(" HIDE ", ImVec2(hideButtonWidth, 40)))
                    {
                        showMenu = false;
                    }
                    ImGui::SameLine();
                    
                    bool isFs = windowManager.getIsFullscreen();
                    if (ImGui::Button(isFs ? " WINDOW " : " FULLSCREEN ", ImVec2(fullscreenButtonWidth, 40)))
                    {
                        windowManager.toggleFullscreen();
                    }
                    ImGui::PopStyleColor(3);
                    
                    ImGui::Spacing();
                    
                    // Title
                    ImGui::PushFont(nullptr);
                    ImGui::TextColored(ImVec4(0.85f, 0.87f, 0.92f, 1.0f), "PARTICLE SIMULATION");
                    ImGui::PopFont();
                    
                    ImGui::Spacing();
                    ImGui::TextColored(ImVec4(0.60f, 0.65f, 0.72f, 1.0f), "FPS: %.1f  |  %.2f ms", io.Framerate, 1000.0f / io.Framerate);
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();
                    
                    // Begin scrollable child region for content
                    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_None);

                    // ========== CONFIGURATIONS ==========
                    if (ImGui::CollapsingHeader("[ CONFIGURATIONS ]"))
                    {
                        ImGui::Indent(10);
                        // Info box
                        if (ImGui::TreeNodeEx("Info"))
                        {
                            ImGui::PushTextWrapPos(0.0f);
                            ImGui::TextColored(ImVec4(0.7f,0.8f,0.9f,1.0f), "Preset corrente:");
                            ImGui::TextColored(ImVec4(0.8f,0.85f,0.9f,1.0f), "%s", currentPresetLabel.c_str());
                            ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1.0f), "%s", presetStatus.c_str());

                            ImGui::Spacing();
                            ImGui::TextColored(ImVec4(0.6f,0.75f,0.9f,1.0f), "Profilo hardware:");
                            ImGui::TextColored(ImVec4(0.7f,0.8f,0.9f,1.0f), "%s.cfg", hardwarePresetName.c_str());

                            ImGui::Spacing();
                            ImGui::TextColored(ImVec4(0.6f,0.7f,0.8f,1.0f), "GPU in uso (OpenGL):");
                            ImGui::TextWrapped("%s", hardwareInfo.renderer.c_str());
                            if (preferredGpu)
                            {
                                ImGui::TextColored(ImVec4(0.55f,0.65f,0.75f,1.0f), "GPU preferita:");
                                ImGui::TextWrapped("%s", preferredGpuLabel.c_str());
                                if (!usingPreferredGpu)
                                {
                                    ImGui::TextColored(ImVec4(0.95f,0.65f,0.25f,1.0f), "ATTENZIONE: forza GPU dedicata nelle impostazioni Windows.");
                                }
                            }
                            else
                            {
                                ImGui::TextColored(ImVec4(0.8f,0.5f,0.5f,1.0f), "GPU preferita non rilevata");
                            }
                            ImGui::PopTextWrapPos();
                            ImGui::Spacing();
                            ImGui::TreePop();
                        }

                        // Preset management
                        if (ImGui::TreeNodeEx("Presets"))
                        {
                            ImGui::Spacing();
                            ImGui::InputText("Nome preset", presetNameBuf, IM_ARRAYSIZE(presetNameBuf));
                            ImGui::Spacing();
                            if (ImGui::Button("Carica preset", ImVec2(170, 40))) {
                                std::string path = std::string(configDir) + "/" + std::string(presetNameBuf) + ".cfg";
                                if (loadParamsFromFile(params, path)) {
                                    currentPresetLabel = path;
                                    presetStatus = "Caricato " + path;
                                } else {
                                    presetStatus = "Impossibile caricare " + path;
                                }
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Salva preset", ImVec2(180, 40))) {
                                std::string path = std::string(configDir) + "/" + std::string(presetNameBuf) + ".cfg";
                                if (saveParamsToFile(params, path)) {
                                    currentPresetLabel = path;
                                    presetStatus = "Salvato " + path;
                                } else {
                                    presetStatus = "Errore salvataggio " + path;
                                }
                            }
                            ImGui::Spacing();
                            if (ImGui::Button("Salva come default", ImVec2(200, 40))) {
                                if (saveParamsToFile(params, defaultConfigPath)) {
                                    currentPresetLabel = "default.cfg";
                                    presetStatus = "Default aggiornato";
                                } else {
                                    presetStatus = "Errore nel salvataggio default.cfg";
                                }
                            }
                            ImGui::SameLine();
                            if (ImGui::Button("Carica default", ImVec2(200, 40))) {
                                if (loadParamsFromFile(params, defaultConfigPath)) {
                                    currentPresetLabel = "default.cfg";
                                    presetStatus = "Caricato default.cfg";
                                } else {
                                    presetStatus = "default.cfg non trovato";
                                }
                            }

                            ImGui::Spacing();
                            if (ImGui::TreeNode("Preset salvati"))
                            {
                                std::vector<std::string> presetList;
                                try {
                                    for (const auto& entry : std::filesystem::directory_iterator(configDir)) {
                                        if (entry.is_regular_file() && entry.path().extension() == ".cfg") {
                                            presetList.push_back(entry.path().filename().string());
                                        }
                                    }
                                    std::sort(presetList.begin(), presetList.end());
                                } catch (...) {
                                    // ignore errors
                                }
                                ImGui::BeginChild("PresetList", ImVec2(0, 150), true);
                                for (const auto& name : presetList) {
                                    bool isCurrent = (currentPresetLabel == name || currentPresetLabel == (configDir + "/" + name));
                                    if (ImGui::Selectable(name.c_str(), isCurrent, ImGuiSelectableFlags_None, ImVec2(menuWidth - 100, 0))) { // Leave space for delete button
                                        std::string stem = std::filesystem::path(name).stem().string();
                                        std::snprintf(presetNameBuf, IM_ARRAYSIZE(presetNameBuf), "%s", stem.c_str());
                                    }
                                    
                                    // Handle Double Click to Load
                                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                                        std::string path = std::string(configDir) + "/" + name;
                                        if (loadParamsFromFile(params, path)) {
                                            currentPresetLabel = path;
                                            presetStatus = "Caricato " + path;
                                        } else {
                                            presetStatus = "Impossibile caricare " + path;
                                        }
                                    }

                                    // Delete Button
                                    ImGui::SameLine();
                                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.1f, 0.1f, 1.0f));
                                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.2f, 0.2f, 1.0f));
                                    if (ImGui::Button((" X ##" + name).c_str())) {
                                        std::string path = std::string(configDir) + "/" + name;
                                        try {
                                            if (std::filesystem::remove(path)) {
                                                presetStatus = "Eliminato " + name;
                                                // If we deleted the current preset, clear label
                                                if (currentPresetLabel == path || currentPresetLabel == name) {
                                                    currentPresetLabel = "Nessun preset";
                                                }
                                            } else {
                                                presetStatus = "Errore eliminazione " + name;
                                            }
                                        } catch (...) {
                                            presetStatus = "Eccezione eliminazione " + name;
                                        }
                                    }
                                    ImGui::PopStyleColor(2);
                                }
                                ImGui::EndChild();
                                ImGui::TreePop();
                            }
                            ImGui::TreePop();
                        }

                        ImGui::Spacing();
                        ImGui::Unindent(10);
                    }

                    // ========== SIMULATIONS ==========
                    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.22f, 0.24f, 0.30f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.28f, 0.30f, 0.38f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.32f, 0.34f, 0.42f, 1.0f));
                    
                    if (ImGui::CollapsingHeader("[ SIMULATIONS ]"))
                    {
                        ImGui::Indent(10);

                        // Particles count (direct)
                        int maxCount = simulation.getMaxParticleCount();
                        params.targetParticleCount = std::clamp(params.targetParticleCount, 10000, maxCount);
                        ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.7f, 1.0f), "Particles");
                        ImGui::SliderInt("##ParticleCount", &params.targetParticleCount, 10000, maxCount, "%d", ImGuiSliderFlags_AlwaysClamp);
                        
                        ImGui::Spacing();
                        // --- Boundary ---
                        static const char* boundaryOptions[] = {
                            "Toroidal wrap",
                            "Rigid bounce (reflect)",
                            "Klein bottle (full twist)"
                        };
                        ImGui::TextColored(ImVec4(0.6f,0.7f,0.8f,1.0f), "Boundary");
                        ImGui::Combo("##BoundaryMode", &params.boundaryMode, boundaryOptions, IM_ARRAYSIZE(boundaryOptions));

                        ImGui::Spacing();
                        ImGui::Separator();
                        
                        // --- Physarum ---
                        ImGui::Checkbox("Physarum", &params.physarumEnabled);
                        ImGui::SameLine();
                        if (ImGui::TreeNode("Settings##Physarum"))
                    {
                        ImGui::Spacing();
                            if (params.physarumEnabled) {
                                ImGui::Spacing();
                                ImGui::SliderFloat("Force Intensity", &params.physarumIntensity, 0.0f, 5.0f, "%.2f");
                                ImGui::SliderFloat("Sensor Distance", &params.sensorDistance, 5.0f, 50.0f, "%.1f px");
                                
                                float sensorDeg = params.sensorAngle * 57.2958f;
                                if (ImGui::SliderFloat("Sensor Angle", &sensorDeg, 5.0f, 90.0f, "%.1f deg")) {
                                    params.sensorAngle = sensorDeg / 57.2958f;
                                }
                                
                                float turnDeg = params.turnAngle * 57.2958f;
                                if (ImGui::SliderFloat("Turn Angle", &turnDeg, 5.0f, 90.0f, "%.1f deg")) {
                                    params.turnAngle = turnDeg / 57.2958f;
                                }
                                
                                ImGui::SliderFloat("Speed", &params.speed, 10.0f, 300.0f, "%.0f");
                                
                                ImGui::Spacing();
                                ImGui::Spacing();
                            }
                            ImGui::Spacing();
                            ImGui::TreePop();
                        }

                        ImGui::Spacing();
                        
                        // --- Boids ---
                        ImGui::Checkbox("Boids", &params.boidsEnabled);
                        ImGui::SameLine();
                        if (ImGui::TreeNode("Settings##Boids"))
                        {
                            ImGui::Spacing();
                            if (params.boidsEnabled) {
                                ImGui::SliderFloat("Alignment", &params.alignment, 0.0f, 2.0f);
                                ImGui::SliderFloat("Separation", &params.separation, 0.0f, 2.0f);
                                ImGui::SliderFloat("Cohesion", &params.cohesion, 0.0f, 2.0f);
                                ImGui::SliderFloat("Radius", &params.radius, 10.0f, 100.0f);
                            } else {
                                ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1.0f), "Abilita Boids per modificare i parametri");
                            }
                            ImGui::Spacing();
                            ImGui::TreePop();
                        }
                        
                        ImGui::Spacing();
                        
                        // --- Physics ---
                        if (ImGui::TreeNodeEx("Physics"))
                        {
                            ImGui::Spacing();
                            ImGui::SliderFloat("Inertia", &params.inertia, 0.0f, 0.99f, "%.2f");
                            ImGui::DragFloat("Speed min", &params.speedMin, 1.0f, 0.0f, params.speedMax - 1.0f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
                            ImGui::DragFloat("Speed max", &params.speedMax, 5.0f, params.speedMin + 1.0f, 1e9f, "%.0f", ImGuiSliderFlags_AlwaysClamp);
                            ImGui::DragFloat("Base speed", &params.speed, 5.0f, params.speedMin, params.speedMax, "%.0f", ImGuiSliderFlags_AlwaysClamp);
                            ImGui::Spacing();
                            ImGui::TreePop();
                        }
                        
                        ImGui::Spacing();
                        
                        // --- Collisions ---
                        if (ImGui::TreeNode("Collisions"))
                        {
                            ImGui::Spacing();
                            ImGui::Checkbox("Active", &params.collisionsEnabled);
                            ImGui::SliderFloat("Radius", &params.collisionRadius, 10.0f, 200.0f, "%.0f px");
                            ImGui::SliderFloat("Restitution", &params.restitution, 0.0f, 1.5f, "%.2f");
                            ImGui::Spacing();
                            ImGui::TreePop();
                        }

                        ImGui::Spacing();

                        // (Boundary moved up)
                        ImGui::Spacing();
                        
                        
                        ImGui::Unindent(10);
                    }

                    ImGui::Spacing();

                    // ========== TEXTURE & COLORS ==========
                    if (ImGui::CollapsingHeader("[ COLORS ]"))
                    {
                        ImGui::Indent(10);
                        ImGui::Spacing();

                        // 1. Texture Settings
                        if (ImGui::TreeNodeEx("Texture Settings")) {
                             const char* fmtItems[] = { "R8 (1 Ch: Low Bandwidth)", "RG8 (2 Ch: Medium)", "RGBA8 (4 Ch: Standard)" };
                             ImGui::Combo("Format", &params.textureFormat, fmtItems, IM_ARRAYSIZE(fmtItems));
                             if (ImGui::IsItemHovered()) ImGui::SetTooltip("R8=Solo Rosso (veloce), RG8=Rosso+Verde, RGBA8=Full Pixel Color");
                             
                             const char* resItems[] = { "720p (HD)", "1080p (FHD)", "1440p (QHD)", "2160p (4K)" };
                             ImGui::Combo("Resolution", &params.resolutionPreset, resItems, IM_ARRAYSIZE(resItems));
                             
                             ImGui::TreePop();
                             ImGui::Separator();
                             ImGui::Spacing();
                        }

                        // 2. Trail Emission
                        if (ImGui::TreeNode("Trail Emission")) {
                            ImGui::Spacing();
                            ImGui::TextColored(ImVec4(0.7f,0.8f,0.7f,1.0f), "[ Data Generation ]");
                            
                            // Decay
                            ImGui::SliderFloat("Trail Decay", &params.trailFade, 0.90f, 0.9999f, "%.4f");
                            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Determina quanto velocemente la scia scompare (0.99 = lunga, 0.90 = breve)");

                            ImGui::Spacing();

                            // Colors
                            ImGui::ColorEdit3("Color A", params.color1);
                            ImGui::ColorEdit3("Color B", params.color2);
                            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Colori base usati per l'interpolazione (A -> B)");

                            ImGui::Spacing();
                            
                            // Source Logic
                             static const char* colorSources[] = { "Particle Angle", "Speed (Manual Range)", "Speed (Auto Range)" };
                             ImGui::Combo("Color Source", &params.colorSource, colorSources, IM_ARRAYSIZE(colorSources));
                             
                             if (params.colorSource == 1) { // Manual Speed
                                ImGui::Indent();
                                ImGui::SliderFloat("Speed Min", &params.colorSpeedMin, 0.0f, 1000.0f, "%.0f");
                                ImGui::SliderFloat("Speed Max", &params.colorSpeedMax, params.colorSpeedMin + 1.0f, 2000.0f, "%.0f");
                                ImGui::Unindent();
                             } else if (params.colorSource == 2) { // Auto Speed
                                ImGui::Indent();
                                float autoMin = simulation.getAutoColorSpeedMin();
                                float autoMax = simulation.getAutoColorSpeedMax();
                                ImGui::TextColored(ImVec4(0.6f,0.8f,1.0f,1.0f), "Auto Range: %.0f - %.0f", autoMin, autoMax);
                                ImGui::Unindent();
                             }
                             
                             ImGui::Spacing();
                             // Mutation
                             ImGui::SliderFloat("Mutation Offset", &params.colorOffset, 0.0f, 1.0f, "%.2f");
                             if (ImGui::IsItemHovered()) ImGui::SetTooltip("Ruota la palette colori (offset globale)\nGenera variazioni nel tempo.");

                             ImGui::Spacing();
                             ImGui::TreePop();
                        }
                        
                        ImGui::Spacing();

                        // 3. Post-Processing
                        if (ImGui::TreeNode("Post-Processing")) {
                             ImGui::Spacing();
                             ImGui::TextColored(ImVec4(0.7f,0.8f,0.7f,1.0f), "[ Visual Style ]");
                             
                             ImGui::ColorEdit3("Background", params.backgroundColor);
                             if (ImGui::IsItemHovered()) ImGui::SetTooltip("Colore di pulizia dello schermo (sfondo)");
                             
                             static const char* colorModes[] = { "Standard", "Legacy Supernova", "Neon Thermal", "Bismuth Iridescent", "Psycho Overflow" };
                             ImGui::Combo("Aesthetics Mode", &params.colorMode, colorModes, IM_ARRAYSIZE(colorModes));
                             
                             if (params.colorMode >= 2) { // Neon effects
                                ImGui::Indent();
                                ImGui::SliderFloat("Cycle Speed", &params.neonSpeed, 0.0f, 5.0f);
                                ImGui::SliderFloat("Band Frequency", &params.neonRange, 0.1f, 5.0f);
                                ImGui::Unindent();
                             }
                             
                             ImGui::Separator();
                             ImGui::Text("Camera / Exposure");
                             ImGui::SliderFloat("Exposure", &params.toneExposure, 0.1f, 10.0f, "%.2f");
                             if (ImGui::IsItemHovered()) ImGui::SetTooltip("Moltiplicatore di luminosita' globale");

                             ImGui::SliderFloat("Auto-Dim Threshold", &params.autoDimThreshold, 0.0f, 1.0f, "%.2f");
                             if (ImGui::IsItemHovered()) ImGui::SetTooltip("Soglia di luminosita' media oltre cui scurire l'immagine");

                             ImGui::SliderFloat("Auto-Dim Strength", &params.autoDimStrength, 0.0f, 1.0f, "%.2f");
                             ImGui::SliderFloat("Global Dim", &params.autoDimGlobal, 0.0f, 20.0f, "%.1f");
                             
                             ImGui::TreePop();
                        }

                        ImGui::Spacing();
                        ImGui::Unindent(10);
                    }

                    ImGui::Spacing();

                    // ========== MOUSE EFFECTS ==========
                    if (ImGui::CollapsingHeader("[ MOUSE EFFECTS ]"))
                    {
                        ImGui::Indent(10);
                        ImGui::Spacing();
                        ImGui::TextColored(ImVec4(0.7f,0.8f,0.9f,1.0f), "Comportamento");
                        ImGui::RadioButton("Attract", &params.mouseMode, 0); ImGui::SameLine();
                        ImGui::RadioButton("Repel", &params.mouseMode, 1); ImGui::SameLine();
                        ImGui::RadioButton("Ring", &params.mouseMode, 2); ImGui::SameLine();
                        ImGui::RadioButton("Vortex", &params.mouseMode, 3);
                        ImGui::Spacing();

                        static const char* falloffNames[] = { "1/r", "1/r^2", "1/r^3", "Gaussian", "Oscillatory" };
                        ImGui::TextColored(ImVec4(0.7f,0.8f,0.9f,1.0f), "Legge di forza");
                        ImGui::Combo("##MouseFalloff", &params.mouseFalloff, falloffNames, IM_ARRAYSIZE(falloffNames));
                        ImGui::SliderFloat("Strength", &params.mouseStrength, 0.0f, 10.0f, "%.2f");
                        if (params.mouseFalloff == 3) {
                            ImGui::SliderFloat("Gaussian sigma (px)", &params.mouseGaussianSigma, 10.0f, 2000.0f, "%.0f");
                        }
                        if (params.mouseFalloff == 4) {
                            ImGui::SliderFloat("Osc freq (cycles/px)", &params.mouseOscFreq, 0.01f, 5.0f, "%.3f");
                        }

                        ImGui::Separator();
                        ImGui::Checkbox("Ring overlay (attr+rep)", &params.mouseRingOverlay);
                        if (params.mouseRingOverlay || params.mouseMode == 2) {
                            ImGui::SliderFloat("Ring radius", &params.mouseRingRadius, 50.0f, 2000.0f, "%.0f px");
                        }
                        ImGui::TextColored(ImVec4(0.6f,0.7f,0.8f,1.0f), "L'effetto ora agisce su tutta l'area, con decadimento scelto.");
                        ImGui::Spacing();
                        ImGui::Unindent(10);
                    }

                    ImGui::Spacing();

                    ImGui::PopStyleColor(3);
                    
                    ImGui::EndChild(); // End scrollable region
                    ImGui::PopItemWidth();
                }
                ImGui::End();
                ImGui::PopStyleVar();
            }

            // --- UPDATE RESOLUTION / FORMAT ---
            Utils::SimulationManager::getSimulationSize((Utils::SimulationResolution)params.resolutionPreset, simWidth, simHeight);
            simulation.resize(simWidth, simHeight, (SimulationGPU::TextureFormat)params.textureFormat);

            // --- UPDATE RENDER PIPELIE ---
            renderPipeline.setColorMode(params.colorMode);
            renderPipeline.setTime((float)glfwGetTime());
            renderPipeline.setColors(params.color1, params.color2);
            renderPipeline.setNeonParams(params.neonSpeed, params.neonRange);
            renderPipeline.setBackgroundColor(params.backgroundColor);
            simulation.setColorSource(params.colorSource);
            simulation.setColorSpeedRange(params.colorSpeedMin, params.colorSpeedMax);

            // --- UPDATE SIMULATION PARAMETERS FROM UI ---
            simulation.setColorOffset(params.colorOffset);
            simulation.setPhysarumEnabled(params.physarumEnabled);
            simulation.setPhysarumIntensity(params.physarumIntensity);
            simulation.setSensorDistance(params.sensorDistance);
            simulation.setSensorAngle(params.sensorAngle);
            simulation.setTurnAngle(params.turnAngle);
            simulation.setSpeedRange(params.speedMin, params.speedMax);
            simulation.setSpeed(params.speed);
            simulation.setTrailFade(params.trailFade);
            simulation.setToneExposure(params.toneExposure);
            simulation.setAutoDimThreshold(params.autoDimThreshold);
            simulation.setAutoDimStrength(params.autoDimStrength);
            simulation.setAutoDimGlobal(params.autoDimGlobal);
            simulation.setInertia(params.inertia);
            simulation.setRestitution(params.restitution);
            simulation.setRandomWeight(0.05f); // Fixed for now

            // Collisions
            simulation.setCollisionsEnabled(params.collisionsEnabled);
            simulation.setCollisionRadius(params.collisionRadius);
            simulation.setBoundaryMode(params.boundaryMode);
            simulation.setMouseFalloff(params.mouseFalloff);
            simulation.setMouseStrength(params.mouseStrength);
            simulation.setMouseGaussianSigma(params.mouseGaussianSigma);
            simulation.setMouseOscFreq(params.mouseOscFreq);
            simulation.setMouseRingOverlay(params.mouseRingOverlay);
            simulation.setMouseRingRadius(params.mouseRingRadius);
            
            // Colors
            simulation.setColor1(params.color1[0], params.color1[1], params.color1[2]);
            simulation.setColor2(params.color2[0], params.color2[1], params.color2[2]);
            
            // Boids
            simulation.setBoidsEnabled(params.boidsEnabled);
            simulation.setAlignmentWeight(params.alignment);
            simulation.setSeparationWeight(params.separation);
            simulation.setCohesionWeight(params.cohesion);
            simulation.setBoidsRadius(params.radius);
            
            // Particle pool
            simulation.setActiveParticleCount(params.targetParticleCount);

            // --- INPUT & SIMULATION ---
            inputHandler.update();

            auto [mx, my] = inputHandler.getMousePos();
            double winW = (double)windowManager.getWidth();
            double winH = (double)windowManager.getHeight();
            
            double nx = mx / winW;
            double ny = (winH - my) / winH;

            float simMouseX = (float)(nx * simWidth);
            float simMouseY = (float)(ny * simHeight);
            
            bool isPressed = inputHandler.isLeftClicked() && !io.WantCaptureMouse;

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
