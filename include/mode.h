#ifndef MODE_H
#define MODE_H

enum class SimulationMode {
    SIMPLE,
    SHADER,
    SHADER_PINGPONG
};

enum class SimulationResolution {
    NATIVE,  // 1:1 rispetto alla risoluzione del monitor
    HD,      // 1920x1080
    SD       // 1280x720
};

#endif // MODE_H
