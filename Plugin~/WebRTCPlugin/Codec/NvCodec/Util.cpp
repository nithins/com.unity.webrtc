#include "pch.h"
#include "nvEncodeAPI.h"
#include "NvEncoder.h"

namespace unity
{
namespace webrtc
{
    void* s_hModule = nullptr;

    bool LoadModule()
    {
        if (s_hModule != nullptr)
            return true;

    #if defined(_WIN32)
    #if defined(_WIN64)
        const HMODULE module = LoadLibrary(TEXT("nvEncodeAPI64.dll"));
    #else
        HMODULE module = LoadLibrary(TEXT("nvEncodeAPI.dll"));
    #endif
    #else
        void* module = dlopen("libnvidia-encode.so.1", RTLD_LAZY);
    #endif

        if (module == nullptr)
        {
            LogPrint("NVCodec library file is not found. Please ensure NV driver is installed");
            return false;
        }
        s_hModule = module;
        return true;
    }

    void UnloadModule()
    {
        if (s_hModule)
        {
#if _WIN32
            FreeLibrary(static_cast<HMODULE>(s_hModule));
#else
            dlclose(s_hModule);
#endif
            s_hModule = nullptr;
        }
    }

    bool CheckDriverVersion()
    {
        uint32_t version = 0;
        uint32_t currentVersion = (NVENCAPI_MAJOR_VERSION << 4) | NVENCAPI_MINOR_VERSION;
        NVENC_API_CALL(NvEncodeAPIGetMaxSupportedVersion(&version));
        if (currentVersion > version)
        {
            return false;
        }
        return true;
    }
} // end namespace webrtc
} // end namespace unity
