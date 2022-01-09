
/*-----------------------------------------*\
|  RGBController_EVGAGP102.h                |
|                                           |
|  EVGA Generic RGB Interface for OpenRGB   |
|  EVGA GP102-based Nvidia GPUs.            |
|                                           |
|  Fabricio Murta (avengerx) 1/31/2021      |
\*-----------------------------------------*/

#pragma once

#include "RGBController.h"
#include "NVIDIAFoundersV1Controller.h"

#define NVIDIA_FOUNDERS_V1_CONTROLLER_NAME   "NVIDIA_FOUNDERS_V1"

class RGBController_NVIDIAFounders_V1 : public RGBController
{
public:
    RGBController_NVIDIAFoundersV1(std::vector<NVIDIAFoundersV1Controller*> nvidia_founders_ptr);
    ~RGBController_NVIDIAFoundersV1();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();
    void        DeviceSaveMode();

private:
    std::vector<NVIDIAFoundersV1Controller*> nvidia_founders;
};