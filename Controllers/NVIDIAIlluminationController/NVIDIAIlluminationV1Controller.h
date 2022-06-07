/*-----------------------------------------*\
|  NVIDIAIlluminationV1Controller.h         |
|                                           |
|  Definitions and types for direct NVIDIA  |
|  Illumination-based NVIDIA GPUs' RGB      |
|  controller                               |
|                                           |
|  Carter Miller (GingerRunner) 1/4/2022    |
\*-----------------------------------------*/

#ifdef _WIN32

#include <string>
#include "i2c_smbus.h"
#include "i2c_smbus_nvapi.h"
#include "RGBController.h"
#include "LogManager.h"
#include <vector>

#include <cstring>

#pragma once

#define NVIDIA_ILLUMINATION_V1_CONTROLLER_NAME   "NVIDIA_ILLUMINATION_V1"

struct NVIDIAIllumination_Config
{
    uint8_t brightness;
    RGBColor colors[7];
};

enum
{
    NVIDIA_ILLUMINATION_OFF = 0,
    NVIDIA_ILLUMINATION_DIRECT = 1
};

class NVIDIAIlluminationV1Controller
{
    public:
        NVIDIAIlluminationV1Controller(i2c_smbus_interface* bus, bool treats_rgbw_as_rgb);
        ~NVIDIAIlluminationV1Controller();

        void setZone(uint8_t zone, uint8_t mode, NVIDIAIllumination_Config zone_config);
        bool allZero(std::array<uint8_t, 4> colors);
        void setZoneRGBW(uint8_t zone, uint8_t red, uint8_t green, uint8_t blue, uint8_t white, uint8_t brightness);
        void setZoneRGB(uint8_t zone, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness);
        void getColor();
        std::vector<NV_GPU_CLIENT_ILLUM_ZONE_TYPE> getInfo();
        void getControl();
        void setControl();
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS zoneParams;

    private:
        i2c_smbus_interface* bus;
        bool _treats_rgbw_as_rgb;
};
#endif