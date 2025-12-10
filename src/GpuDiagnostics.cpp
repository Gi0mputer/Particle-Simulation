#include "GpuDiagnostics.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#endif

namespace Utils
{
    namespace
    {
        std::string toLower(const std::string& input)
        {
            std::string out = input;
            std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
            return out;
        }

#ifdef _WIN32
        std::string narrow(const wchar_t* wide)
        {
            if (!wide) return {};
            int required = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
            if (required <= 0) return {};
            std::string out(static_cast<size_t>(required), '\0');
            int written = WideCharToMultiByte(CP_UTF8, 0, wide, -1, out.data(), required, nullptr, nullptr);
            if (written > 0 && !out.empty() && out.back() == '\0')
            {
                out.pop_back(); // rimuove terminatore
            }
            return out;
        }

        GPUAdapterInfo makeAdapterInfo(const DXGI_ADAPTER_DESC1& desc, unsigned int index)
        {
            GPUAdapterInfo info;
            info.name = narrow(desc.Description);
            info.dedicatedMemory = desc.DedicatedVideoMemory;
            info.sharedMemory = desc.SharedSystemMemory;
            info.vendorId = desc.VendorId;
            info.deviceId = desc.DeviceId;
            info.adapterIndex = index;
            info.isHardware = (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;
            info.isDiscrete = info.isHardware && desc.DedicatedVideoMemory > 0;
            return info;
        }
#endif
    }

    std::vector<GPUAdapterInfo> enumerateGpuAdapters()
    {
        std::vector<GPUAdapterInfo> adapters;

#ifdef _WIN32
        using Microsoft::WRL::ComPtr;

        ComPtr<IDXGIFactory6> factory6;
        HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory6));

        if (SUCCEEDED(hr) && factory6)
        {
            for (UINT idx = 0;; ++idx)
            {
                ComPtr<IDXGIAdapter1> adapter;
                hr = factory6->EnumAdapterByGpuPreference(idx, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
                if (hr == DXGI_ERROR_NOT_FOUND)
                    break;
                if (FAILED(hr))
                    break;

                DXGI_ADAPTER_DESC1 desc{};
                if (SUCCEEDED(adapter->GetDesc1(&desc)))
                {
                    adapters.push_back(makeAdapterInfo(desc, idx));
                }
            }
        }
        else
        {
            ComPtr<IDXGIFactory1> factory1;
            if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory1))))
            {
                for (UINT idx = 0;; ++idx)
                {
                    ComPtr<IDXGIAdapter1> adapter;
                    hr = factory1->EnumAdapters1(idx, &adapter);
                    if (hr == DXGI_ERROR_NOT_FOUND)
                        break;
                    if (FAILED(hr))
                        break;

                    DXGI_ADAPTER_DESC1 desc{};
                    if (SUCCEEDED(adapter->GetDesc1(&desc)))
                    {
                        adapters.push_back(makeAdapterInfo(desc, idx));
                    }
                }
            }
        }
#endif

        return adapters;
    }

    std::optional<GPUAdapterInfo> pickBestGpu(const std::vector<GPUAdapterInfo>& adapters)
    {
        if (adapters.empty())
            return std::nullopt;

        auto score = [](const GPUAdapterInfo& g) -> uint64_t {
            // Privilegia GPU discrete e piu' memoria dedicata.
            constexpr uint64_t discreteBias = 1ull << 60;
            uint64_t base = g.dedicatedMemory ? g.dedicatedMemory : (g.sharedMemory / 2);
            return (g.isDiscrete ? discreteBias : 0) + base;
        };

        return *std::max_element(adapters.begin(), adapters.end(),
                                 [&](const GPUAdapterInfo& a, const GPUAdapterInfo& b) {
                                     return score(a) < score(b);
                                 });
    }

    bool rendererMatchesAdapter(const GPUAdapterInfo& adapter, const std::string& renderer)
    {
        const std::string rendererLower = toLower(renderer);
        const std::string nameLower = toLower(adapter.name);

        if (!nameLower.empty() && rendererLower.find(nameLower) != std::string::npos)
            return true;

        if (adapter.vendorId == 0x10DE && rendererLower.find("nvidia") != std::string::npos)
            return true;
        if ((adapter.vendorId == 0x1002 || adapter.vendorId == 0x1022) && rendererLower.find("amd") != std::string::npos)
            return true;
        if (adapter.vendorId == 0x8086 && rendererLower.find("intel") != std::string::npos)
            return true;

        return false;
    }

    std::string formatMemoryMB(uint64_t bytes)
    {
        std::ostringstream oss;
        double mb = static_cast<double>(bytes) / (1024.0 * 1024.0);
        oss << std::fixed << std::setprecision(1) << mb << " MB";
        return oss.str();
    }

    void logGpuAdapters(const std::vector<GPUAdapterInfo>& adapters, const std::optional<GPUAdapterInfo>& preferred)
    {
        if (adapters.empty())
        {
            std::cout << "[GPU] Nessuna GPU rilevata via DXGI (o piattaforma non Windows)." << std::endl;
            return;
        }

        std::cout << "[GPU] GPU rilevate:" << std::endl;
        for (const auto& gpu : adapters)
        {
            std::cout << "  - " << gpu.name
                      << " | dedicated: " << formatMemoryMB(gpu.dedicatedMemory)
                      << " | shared: " << formatMemoryMB(gpu.sharedMemory)
                      << (gpu.isDiscrete ? " [discreta]" : " [integrata]") << std::endl;
        }

        if (preferred)
        {
            std::cout << "[GPU] GPU consigliata per la simulazione: " << preferred->name
                      << " (" << formatMemoryMB(preferred->dedicatedMemory) << ")" << std::endl;
        }
    }
}
