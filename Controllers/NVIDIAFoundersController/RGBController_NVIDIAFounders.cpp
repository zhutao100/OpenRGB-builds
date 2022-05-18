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

RGBController_NVIDIAFoundersV1::RGBController_NVIDIAFoundersV1(NVIDIAFoundersV1Controller* nvidia_founders_ptr)
{
    nvidia_founders = nvidia_founders_ptr;

    name        = "NVIDIA FOUNDERS GPU";
    vendor      = "NVIDIA";
    description = "NVIDIA Founders Edition RGB GPU Device";

    type = DEVICE_TYPE_GPU;

    mode Off;
    Off.name       = "Off";
    Off.value      = 0;
    Off.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Off.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Off);

    mode Static;
    Static.name       = "Static";
    Static.value      = 1;
    Static.flags      = MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_PER_LED_COLOR;
    Static.color_mode = MODE_COLORS_PER_LED;
    Static.colors_min = 1;
    Static.colors_max = 1;
    Static.brightness_min = 0;
    Static.brightness = 100;
    Static.brightness_max = 100;
    modes.push_back(Static);

    SetupZones();

    // Initialize active mode and stored color
    // unsigned char raw_active_mode = nvidia_founders[0]->GetMode();

    for(uint8_t zone_idx = 0; zone_idx < 2; zone_idx++)
    {
        std::array<unsigned char, 3> rgb = nvidia_founders->getColor();
        zones[zone_idx].colors[0] = ToRGBColor(rgb[0], rgb[1], rgb[2]);
    }
}

RGBController_NVIDIAFoundersV1::~RGBController_NVIDIAFoundersV1()
{
    delete nvidia_founders;
}

void RGBController_NVIDIAFoundersV1::UpdateSingleLED(int)
{
    DeviceUpdateLEDs();
}

void RGBController_NVIDIAFoundersV1::SetupZones()
{
    /*---------------------------------------------------------*\
    | This device only allows setting the entire zone for all   |
    | LED's in the zone and does not allow per LED control.     |
    \*---------------------------------------------------------*/

    // The founders' cards have two zones, the V-shaped area and the logo.
    // The logo doesn't have color control, only brightness, it has not been
    // attempted to control per-LED on this card
    for(uint8_t zone_idx = 0; zone_idx < 2; zone_idx++)
    {
        zone* new_zone = new zone();
        led*  new_led  = new led();

        new_zone->name          = nvidia_illum_zone_names[zone_idx];
        new_zone->type          = ZONE_TYPE_SINGLE;
        new_zone->leds_min      = 1;
        new_zone->leds_max      = 1;
        new_zone->leds_count    = 1;
        new_zone->matrix_map    = NULL;

        new_led->name           = nvidia_illum_zone_names[zone_idx];
        /*---------------------------------------------------------*\
        | Push the zone and LED on to device vectors                |
        \*---------------------------------------------------------*/
        leds.push_back(*new_led);
        zones.push_back(*new_zone);
        zoneIndexMap.push_back(zone_idx);
    }
    SetupColors();

}

// Gets called from apply all to selection
void RGBController_NVIDIAFoundersV1::DeviceUpdateLEDs()
{
    NVIDIAFounders_Config nv_zone_config;
    for(uint8_t zone_idx = 0; zone_idx < zoneIndexMap.size(); zone_idx++)
    {
        nv_zone_config.colors[0] = colors[zone_idx];
        nv_zone_config.brightness = modes[active_mode].brightness;
        nvidia_founders->setZone(zone_idx, modes[active_mode].value, nv_zone_config);
    }
}

void RGBController_NVIDIAFoundersV1::UpdateZoneLEDs(int zone)
{
    NVIDIAFounders_Config nv_zone_config;
    nv_zone_config.colors[0] = colors[zone];
    nv_zone_config.brightness = modes[active_mode].brightness;
    nvidia_founders->setZone(zone, modes[active_mode].value, nv_zone_config);
}

void RGBController_NVIDIAFoundersV1::SetCustomMode()
{

}

void RGBController_NVIDIAFoundersV1::DeviceUpdateMode()
{

}

void RGBController_NVIDIAFoundersV1::ResizeZone(int zone, int new_size)
{

}
