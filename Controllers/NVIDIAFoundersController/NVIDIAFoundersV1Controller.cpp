/*-----------------------------------------*\
|  NVIDIAFoundersV1Controller.cpp           |
|                                           |
|  Driver for NVIDIA Founders GPUs          |
|                                           |
|  Carter Miller (GingerRunner) 1/5/2022    |
\*-----------------------------------------*/

#include "NVIDIAFoundersV1Controller.h"
#include "LogManager.h"

NVIDIAFoundersV1Controller::NVIDIAFoundersV1Controller(i2c_smbus_interface* bus_ptr)
{
    bus = bus_ptr;
}

NVIDIAFoundersV1Controller::~NVIDIAFoundersV1Controller()
{
}

void NVIDIAFoundersV1Controller::getControl()
{
    std::memset(&zoneParams, 0, sizeof(zoneParams));
    // Hardcoded value found via sniffing
    zoneParams.version = 72012;
    zoneParams.bDefault = 0;
    // As far as I can tell, this pre-populates the zone type value!
    bus->nvapi_xfer(NVAPI_ZONE_GET_CONTROL, &zoneParams);
}

void NVIDIAFoundersV1Controller::setControl()
{
    bus->nvapi_xfer(NVAPI_ZONE_SET_CONTROL, &zoneParams);
}

void NVIDIAFoundersV1Controller::setZone(uint8_t zone, uint8_t mode, NVIDIAFounders_Config zone_config)
{
    getControl();
    switch (mode)
    {
        case NVIDIA_FOUNDERS_DIRECT:
            zoneParams.zones[zone].ctrlMode = NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGB;
            // Forcing to this for now to avoid having to use the undefined type 3 found during sniffing, which seems to warrant
            // slightly different logic (RGBW) that the nvapi.h in this project doesn't seem to accomodate yet.
            // If the zone type is something else, this means it is a static zone that takes no colors
            if (zoneParams.zones[zone].type == 3 || zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB)
            {
                zoneParams.zones[zone].type = NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB;
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorR = RGBGetRValue(zone_config.colors[0]);
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorG = RGBGetGValue(zone_config.colors[0]);
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorB = RGBGetBValue(zone_config.colors[0]);
            }
            zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.brightnessPct = zone_config.brightness;
            break;
    }
    setControl();
}


std::array<unsigned char, 3> NVIDIAFoundersV1Controller::getColor()
{
    bus->nvapi_xfer(NVAPI_ZONE_GET_CONTROL, &zoneParams);

    unsigned char red = (unsigned char)zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorR;
    unsigned char green = (unsigned char)zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorG;
    unsigned char blue = (unsigned char)zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorB;

    return {red, green, blue};
}
