/*-----------------------------------------*\
|  NVIDIAFoundersV1Controller.h                    |
|                                           |
|  Definitions and types for EVGA           |
|  GP102-based Nvidia GPUs' RGB controller  |
|  GeForce GTX 1080 Ti K|NGP|N and FTW3.    |
|                                           |
|  Fabricio Murta (avengerx) 1/31/2021      |
\*-----------------------------------------*/

#include <string>
#include "i2c_smbus.h"
#include "i2c_smbus_nvapi.h"
#include "RGBController.h"
#include "nvapi.h"
#include <vector>

#include <cstring>

#pragma once

#define NVIDIA_FOUNDERS_V1_CONTROLLER_NAME   "NVIDIA_FOUNDERS_V1"

struct NVIDIAFounders_Config
{
    uint8_t brightness;
    RGBColor colors[7];
};

enum
{
    NVIDIA_FOUNDERS_OFF = 0,
    NVIDIA_FOUNDERS_DIRECT = 1
};

class NVIDIAFoundersV1Controller
{
public:
    NVIDIAFoundersV1Controller(i2c_smbus_interface* bus);
    ~NVIDIAFoundersV1Controller();

    void setZone(uint8_t zone, uint8_t mode, NVIDIAFounders_Config zone_config);
    std::array<unsigned char, 3> getColor();
    uint8_t normalizeToPercentage(uint8_t value);
    void getControl();
    void setControl();
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS zoneParams;

private:
    i2c_smbus_interface* bus;
};
