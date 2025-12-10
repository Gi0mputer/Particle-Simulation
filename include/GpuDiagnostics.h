#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Utils
{
    struct GPUAdapterInfo
    {
        std::string name;
        uint64_t dedicatedMemory = 0;
        uint64_t sharedMemory = 0;
        unsigned int vendorId = 0;
        unsigned int deviceId = 0;
        unsigned int adapterIndex = 0;
        bool isHardware = false;
        bool isDiscrete = false;
    };

    // Rileva gli adattatori GPU disponibili (solo Windows per ora).
    std::vector<GPUAdapterInfo> enumerateGpuAdapters();

    // Sceglie la GPU "migliore" in base a memoria dedicata e discreta/integrata.
    std::optional<GPUAdapterInfo> pickBestGpu(const std::vector<GPUAdapterInfo>& adapters);

    // Controlla se la stringa renderer OpenGL sembra corrispondere alla GPU indicata.
    bool rendererMatchesAdapter(const GPUAdapterInfo& adapter, const std::string& renderer);

    // Formatta i byte in MB leggibili.
    std::string formatMemoryMB(uint64_t bytes);

    // Log esteso in console della rilevazione GPU e della GPU scelta.
    void logGpuAdapters(const std::vector<GPUAdapterInfo>& adapters, const std::optional<GPUAdapterInfo>& preferred);
}
