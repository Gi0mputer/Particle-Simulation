#ifdef _WIN32
// Hint to use discrete GPU on dual-GPU systems (NVIDIA Optimus / AMD Switchable)
extern "C" {
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif
