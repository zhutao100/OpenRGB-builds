/*-----------------------------------------*\
|  NVIDIAIlluminationV1Controller.h         |
|                                           |
|  Definitions and types for direct NVIDIA  |
|  Illumination-based Nvidia GPUs' RGB      |
|  controller                               |
|                                           |
|  Carter Miller (GingerRunner) 1/04/2022   |
\*-----------------------------------------*/

#include <string>
#include "i2c_smbus.h"
#include "i2c_smbus_nvapi.h"
#include "RGBController.h"
#include "nvapi.h"
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
    NVIDIAIlluminationV1Controller(i2c_smbus_interface* bus);
    ~NVIDIAIlluminationV1Controller();

    void setZone(uint8_t zone, uint8_t mode, NVIDIAIllumination_Config zone_config);
    std::array<unsigned char, 3> getColor();
    uint8_t normalizeToPercentage(uint8_t value);
    void getControl();
    void setControl();
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS zoneParams;

private:
    i2c_smbus_interface* bus;
};
