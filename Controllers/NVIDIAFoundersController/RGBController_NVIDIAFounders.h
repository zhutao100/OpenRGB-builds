/*-----------------------------------------*\
|  RGBController_NVIDIAFounders.h           |
|                                           |
|  NVIDIA Founders Generic RGB Interface    |
|  for OpenRGB                              |
|                                           |
|  Carter Miller (GingerRunner) 1/04/2021   |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "NVIDIAFoundersV1Controller.h"

#define NVIDIA_FOUNDERS_V1_CONTROLLER_NAME   "NVIDIA_FOUNDERS_V1"

static const char* nvidia_illum_zone_names[] =
{
    "Backplate",
    "Logo"
};

class RGBController_NVIDIAFoundersV1 : public RGBController
{
public:
    RGBController_NVIDIAFoundersV1(NVIDIAFoundersV1Controller* nvidia_founders_ptr);
    ~RGBController_NVIDIAFoundersV1();

    void UpdateSingleLED(int led);

    void SetupZones();

    void DeviceUpdateLEDs();

    void UpdateZoneLEDs(int);

    void SetCustomMode();

    void DeviceUpdateMode();

    void ResizeZone(int, int);

private:
    NVIDIAFoundersV1Controller* nvidia_founders;
    std::vector<uint8_t> zoneIndexMap;
};
