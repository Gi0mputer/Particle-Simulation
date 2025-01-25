#pragma once

enum class SimulationResolution {
    HD_720,
    FHD_1080,
    UHD_4K,
    NATIVE
};
class SimulationManager {
public:
    static void getSimulationSize(SimulationResolution resolution, int& simWidth, int& simHeight);
};
