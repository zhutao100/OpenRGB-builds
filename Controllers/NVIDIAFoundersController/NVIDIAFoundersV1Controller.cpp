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
    // This value probably needs retrieved with another NvAPI call, but hard-coding it does work
    //zoneParams.version = (NV_U32)72012;
    // This value can't be hard coded, it seems to come back incremented by the actual number of zones for each NvAPI call involving it
    zoneParams.numIllumZonesControl = 0;
}

NVIDIAFoundersV1Controller::~NVIDIAFoundersV1Controller()
{
}


void NVIDIAFoundersV1Controller::setColor(NV_U8 red, NV_U8 green, NV_U8 blue)
{
    std::memset(&zoneParams, 0, sizeof(zoneParams));
    zoneParams.version = 72012;
    zoneParams.bDefault = 0;
    red = (NV_U8)218;
    green = (NV_U8)86;
    blue = (NV_U8)150;
    bus->nvapi_xfer(NVAPI_ZONE_GET_CONTROL, &zoneParams);
    // This zone is the V-shaped space on the 3080FE
    zoneParams.zones[0].ctrlMode = NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGB;
    // Forcing to this for now to avoid having to use the undefined type 3
    zoneParams.zones[0].type = NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB;
    // zoneParams.zones[0].type = static_cast<NV_GPU_CLIENT_ILLUM_ZONE_TYPE>(3);
    zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorR = red;
    zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorG = green;
    zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorB = blue;
    zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.brightnessPct = (NV_U8)0x64;

    bus->nvapi_xfer(NVAPI_ZONE_SET_CONTROL, &zoneParams);
}

std::array<unsigned char, 3> NVIDIAFoundersV1Controller::getColor()
{
    bus->nvapi_xfer(NVAPI_ZONE_GET_CONTROL, &zoneParams);

    unsigned char red = (unsigned char)zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorR;
    unsigned char green = (unsigned char)zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorG;
    unsigned char blue = (unsigned char)zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorB;

    return {red, green, blue};
}
