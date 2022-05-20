/*-----------------------------------------*\
|  RGBController_NVIDIAIllumination.h       |
|                                           |
|  NVIDIA Illumination Generic RGB Interface|
|  for OpenRGB                              |
|                                           |
|  Carter Miller (GingerRunner) 1/04/2021   |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "NVIDIAIlluminationV1Controller.h"

#define NVIDIA_FOUNDERS_V1_CONTROLLER_NAME   "NVIDIA_FOUNDERS_V1"

static const char* nvidia_illum_zone_names[] =
{
    "Backplate",
    "Logo"
};

class RGBController_NVIDIAIlluminationV1 : public RGBController
{
public:
    RGBController_NVIDIAIlluminationV1(NVIDIAIlluminationV1Controller* nvidia_founders_ptr);
    ~RGBController_NVIDIAIlluminationV1();

    void UpdateSingleLED(int led);

    void SetupZones();

    void DeviceUpdateLEDs();

    void UpdateZoneLEDs(int);

    void SetCustomMode();

    void DeviceUpdateMode();

    void ResizeZone(int, int);

private:
    uint8_t getModeIndex(uint8_t mode_value);
    NVIDIAIlluminationV1Controller* nvidia_illumination;
    std::vector<uint8_t> zoneIndexMap;
};
