/*--------------------------------------------*\
|  RGBController_NVIDIAIllumination.cpp        |
|                                              |
|  Generic RGB Interface for OpenRGB direct    |
|  NVIDIA Illumination controller NVIDIA GPUs. |
|                                              |
|  Carter Miller (GingerRunner) 1/4/2022       |
\*--------------------------------------------*/
#ifdef _WIN32

#include "RGBController_NVIDIAIllumination.h"
#include "NVIDIAIlluminationV1Controller.h"
#include <array>

RGBController_NVIDIAIlluminationV1::RGBController_NVIDIAIlluminationV1(NVIDIAIlluminationV1Controller* nvidia_illumination_ptr)
{
    nvidia_illumination = nvidia_illumination_ptr;

    name        = "NVIDIA Illumination GPU";
    vendor      = "NVIDIA";
    description = "NVIDIA Illumination RGB GPU Device";

    type = DEVICE_TYPE_GPU;

    mode Off;
    Off.name       = "Off";
    Off.value      = NVIDIA_ILLUMINATION_OFF;
    Off.color_mode = MODE_COLORS_NONE;
    modes.push_back(Off);

    mode Static;
    Static.name       = "Direct";
    Static.value      = NVIDIA_ILLUMINATION_DIRECT;
    Static.flags      = MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_PER_LED_COLOR;
    Static.color_mode = MODE_COLORS_PER_LED;
    Static.colors_min = 1;
    Static.colors_max = 1;
    Static.brightness_min = 0;
    Static.brightness = 100;
    Static.brightness_max = 100;
    modes.push_back(Static);

    LOG_DEBUG("Setting up Zones...");
    SetupZones();

    LOG_DEBUG("Retrieving stored color...");
    nvidia_illumination->getColor();
    for(unsigned int i = 0; i < nvidia_illumination->zone_params.numIllumZonesControl; i++)
    {
        zones[i].colors[0] = ToRGBColor(nvidia_illumination->zone_params.zones[i].data.rgb.data.manualRGB.rgbParams.colorR,
        nvidia_illumination->zone_params.zones[i].data.rgb.data.manualRGB.rgbParams.colorG,
        nvidia_illumination->zone_params.zones[i].data.rgb.data.manualRGB.rgbParams.colorB);
    }
}

RGBController_NVIDIAIlluminationV1::~RGBController_NVIDIAIlluminationV1()
{
    delete nvidia_illumination;
}

void RGBController_NVIDIAIlluminationV1::UpdateSingleLED(int)
{
    DeviceUpdateLEDs();
}

void RGBController_NVIDIAIlluminationV1::SetupZones()
{
    /*--------------------------------------------------------------------------*\
    | Use the NvAPI to gather existing zones on the card and their capabilities, |
    | populate available zones accordingly.                                      |
    \*--------------------------------------------------------------------------*/
    LOG_DEBUG("Calling get info...");
    zoneTypes = nvidia_illumination->getInfo();
    LOG_DEBUG("Info retrieved!");
    nvidia_illum_zone_names[NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB] = "RGB";
    nvidia_illum_zone_names[NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGBW] = "RGBW";
    nvidia_illum_zone_names[NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED] = "FIXED COLOR";
    nvidia_illum_zone_names[NV_GPU_CLIENT_ILLUM_ZONE_TYPE_SINGLE_COLOR] = "SINGLE COLOR";
    LOG_DEBUG("Starting zone for loop...");
    for(uint8_t zone_idx = 0; zone_idx < zoneTypes.size(); zone_idx++)
    {
        zone* new_zone = new zone();
        led*  new_led  = new led();

        new_zone->name          = std::to_string(zone_idx) + " - " + (std::string)nvidia_illum_zone_names[zoneTypes[zone_idx]];
        new_zone->type          = ZONE_TYPE_SINGLE;
        new_zone->leds_min      = 1;
        new_zone->leds_max      = 1;
        new_zone->leds_count    = 1;
        new_zone->matrix_map    = NULL;
        new_led->name           = "Entire Zone";
        leds.push_back(*new_led);
        zones.push_back(*new_zone);
        zoneIndexMap.push_back(zone_idx);
    }
    LOG_DEBUG("Setting up colors...");
    SetupColors();

}

void RGBController_NVIDIAIlluminationV1::DeviceUpdateLEDs()
{
    NVIDIAIllumination_Config nv_zone_config;
    for(uint8_t zone_idx = 0; zone_idx < zoneIndexMap.size(); zone_idx++)
    {
        nv_zone_config.colors[0] = colors[zone_idx];
        nv_zone_config.brightness = modes[active_mode].brightness;
        nvidia_illumination->setZone(zone_idx, modes[active_mode].value, nv_zone_config);
    }
}

void RGBController_NVIDIAIlluminationV1::UpdateZoneLEDs(int zone)
{
    NVIDIAIllumination_Config nv_zone_config;
    nv_zone_config.colors[0] = colors[zone];
    nv_zone_config.brightness = modes[active_mode].brightness;
    nvidia_illumination->setZone(zone, modes[active_mode].value, nv_zone_config);
}

uint8_t RGBController_NVIDIAIlluminationV1::getModeIndex(uint8_t mode_value)
{
    for(uint8_t mode_index = 0; mode_index < modes.size(); mode_index++)
    {
        if(modes[mode_index].value == mode_value)
        {
            return mode_index;
        }
    }
    return 0;
}

void RGBController_NVIDIAIlluminationV1::SetCustomMode()
{
    active_mode = getModeIndex(NVIDIA_ILLUMINATION_DIRECT);
}

void RGBController_NVIDIAIlluminationV1::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}

void RGBController_NVIDIAIlluminationV1::ResizeZone(int zone, int new_size)
{

}
#endif
