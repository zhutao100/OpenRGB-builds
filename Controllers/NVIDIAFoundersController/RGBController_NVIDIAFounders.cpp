/*-----------------------------------------*\
|  RGBController_EVGAGPUv1.cpp              |
|                                           |
|  Generic RGB Interface for OpenRGB EVGA   |
|  GP102-based Nvidia GPUs.                 |
|                                           |
|  Fabricio Murta (avengerx) 1/31/2021      |
\*-----------------------------------------*/

#include "RGBController_NVIDIAFounders.h"
#include "NVIDIAFoundersV1Controller.h"
#include <array>

RGBController_NVIDIAFoundersV1::RGBController_NVIDIAFoundersV1(std::vector<NVIDIAFoundersV1Controller*> nvidia_founders_ptr)
{
    nvidia_founders = nvidia_founders_ptr;

    name        = "NVIDIA FOUNDERS GPU";
    vendor      = "NVIDIA";
    description = "NVIDIA Founders Edition RGB GPU Device";

    type = DEVICE_TYPE_GPU;

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = EVGA_GP102_MODE_CUSTOM;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    SetupZones();

    // Initialize active mode and stored color

    unsigned char raw_active_mode = nvidia_founders[0]->GetMode();

    active_mode = 0;
    for(unsigned int i = 0; i < modes.size(); i++)
    {
        if (modes[i].value == raw_active_mode)
        {
            active_mode = i;
            break;
        }
    }
    for(unsigned int i = 0; i < zones.size(); i++)
    {
        std::array<unsigned char, 3> rgb = nvidia_founders[i]->GetColor();

        colors[i] = ToRGBColor(rgb[0], rgb[1], rgb[2]);
    }
}

RGBController_NVIDIAFoundersV1::~RGBController_NVIDIAFoundersV1()
{
    for(unsigned int i = 0; i < nvidia_founders.size(); i++)
    {
        delete nvidia_founders[i];
    }
}

void RGBController_NVIDIAFoundersV1::SetupZones()
{
    /*---------------------------------------------------------*\
    | This device basically has two controllable zones, one at  |
    | the top of the board with GeForce 1080 Ti and another for |
    | the backplate logo (K|NGP|N logo, or EVGA GeForce 1080 Ti |
    | for the FTW3).
    \*---------------------------------------------------------*/
    for(unsigned int i = 0; i < nvidia_founders.size(); i++)
    {
        zone new_zone;
        led new_led;

        new_zone.name          = nvidia_founders[i]->GetName();
        new_zone.type          = ZONE_TYPE_SINGLE;
        new_zone.leds_min      = 1;
        new_zone.leds_max      = 1;
        new_zone.leds_count    = 1;
        new_zone.matrix_map    = NULL;

        new_led.name           = nvidia_founders[i]->GetName();

        leds.push_back(new_led);
        zones.push_back(new_zone);
    }

    SetupColors();
}

void RGBController_NVIDIAFoundersV1::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_NVIDIAFoundersV1::DeviceUpdateLEDs()
{
    for(unsigned int zone_idx = 0; zone_idx < zones.size(); zone_idx++)
    {
        UpdateZoneLEDs(zone_idx);
    }
}

void RGBController_NVIDIAFoundersV1::UpdateZoneLEDs(int zone)
{
    RGBColor color    = colors[zone];
    unsigned char red = RGBGetRValue(color);
    unsigned char grn = RGBGetGValue(color);
    unsigned char blu = RGBGetBValue(color);
    nvidia_founders[zone]->SetColor(red, grn, blu);
}

void RGBController_NVIDIAFoundersV1::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_NVIDIAFoundersV1::SetCustomMode()
{
    active_mode = 1;
}

void RGBController_NVIDIAFoundersV1::DeviceUpdateMode()
{
    for (int i = 0; i < nvidia_founders.size(); i++)
    {
        nvidia_founders[i]->SetMode((unsigned char)modes[(unsigned int)active_mode].value);
    }
}

void RGBController_NVIDIAFoundersV1::DeviceSaveMode()
{
}
