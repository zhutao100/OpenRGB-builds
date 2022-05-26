#ifdef _WIN32
/*-----------------------------------------*\
|  NVIDIAIlluminationV1Controller.cpp           |
|                                           |
|  Driver for NVIDIA Founders GPUs          |
|                                           |
|  Carter Miller (GingerRunner) 1/5/2022    |
\*-----------------------------------------*/

#include "NVIDIAIlluminationV1Controller.h"

NVIDIAIlluminationV1Controller::NVIDIAIlluminationV1Controller(i2c_smbus_interface* bus_ptr)
{
    bus = bus_ptr;
}

NVIDIAIlluminationV1Controller::~NVIDIAIlluminationV1Controller()
{
}

void NVIDIAIlluminationV1Controller::getControl()
{
    std::memset(&zoneParams, 0, sizeof(zoneParams));
    // Hardcoded value found via sniffing
    zoneParams.version = 72012;
    zoneParams.bDefault = 0;
    // As far as I can tell, this pre-populates the zone type value, as well as the number of zones
    // able to be controlled, and their existing settings.
    bus->nvapi_xfer(NVAPI_ZONE_GET_CONTROL, &zoneParams);
}

void NVIDIAIlluminationV1Controller::setControl()
{
    bus->nvapi_xfer(NVAPI_ZONE_SET_CONTROL, &zoneParams);
}

bool NVIDIAIlluminationV1Controller::allZero(std::array<uint8_t, 4> colors)
{
    // This function exists to check if RGB colors are all set to zero, and if so, to take the brightness down 
    // to zero.  This was done to comply with functionality in OpenRGB such as "Lights Off" which sends RGB
    // values of all zeroes
    std::array<uint8_t, 4> allZeros = {0, 0, 0, 0};
    return colors == allZeros;
}

void NVIDIAIlluminationV1Controller::setZone(uint8_t zone, uint8_t mode, NVIDIAIllumination_Config zone_config)
{
    getControl();
    uint8_t red = RGBGetRValue(zone_config.colors[0]);
    uint8_t green = RGBGetGValue(zone_config.colors[0]);
    uint8_t blue = RGBGetBValue(zone_config.colors[0]);
    uint8_t white = 0;
    switch (mode)
    {
        case NVIDIA_ILLUMINATION_OFF:
            zoneParams.zones[zone].ctrlMode = NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGB;
            if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB)
            {
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorR = 0;
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorG = 0;
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorB = 0;
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.brightnessPct = 0;
            }
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGBW)
            {
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorR = 0;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorG = 0;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorB = 0;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorW = 0;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.brightnessPct = 0;
            }
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_SINGLE_COLOR)
            {
                zoneParams.zones[zone].data.singleColor.data.manualSingleColor.singleColorParams.brightnessPct = 0;
            }
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED)
            {
                zoneParams.zones[zone].data.colorFixed.data.manualColorFixed.colorFixedParams.brightnessPct = 0;
            }
            break;
        case NVIDIA_ILLUMINATION_DIRECT:
            zoneParams.zones[zone].ctrlMode = NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGB;
            if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB)
            {
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorR = red;
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorG = green;
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorB = blue;
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.brightnessPct = zone_config.brightness;
            }
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGBW)
            {
                uint8_t minRGBvalue = 0xFF; 
                uint8_t maxRGBvalue = 0;
                // Goal of this logic is to bring out the lowest RGB value
                minRGBvalue = ((red < 0xFF) ? red : minRGBvalue); 
                minRGBvalue = ((green < minRGBvalue) ? green : minRGBvalue); 
                minRGBvalue = ((blue < minRGBvalue) ? blue : minRGBvalue); 
                // Goal of this logic is to bring out the highest RGB value
                maxRGBvalue = ((red > 0) ? red : maxRGBvalue); 
                maxRGBvalue = ((green > maxRGBvalue) ? green : maxRGBvalue); 
                maxRGBvalue = ((blue > maxRGBvalue) ? blue : maxRGBvalue); 
                // If difference between the highest and lowest RGB values is 10 or lower, set the white value only, zero out the rest
                if (maxRGBvalue - minRGBvalue <= 10)
                {
                    red = 0;
                    green = 0;
                    blue = 0;
                    white = (maxRGBvalue + minRGBvalue)/2;
                }
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorR = red;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorG = green;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorB = blue;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorW = white;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.brightnessPct = zone_config.brightness;
            }
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_SINGLE_COLOR)
            {
                zoneParams.zones[zone].data.singleColor.data.manualSingleColor.singleColorParams.brightnessPct = allZero({red, green, blue, white}) ? 0 : zone_config.brightness;
            }
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED)
            {
                zoneParams.zones[zone].data.colorFixed.data.manualColorFixed.colorFixedParams.brightnessPct = allZero({red, green, blue, white}) ? 0 : zone_config.brightness;
            }
            break;
    }
    setControl();
}


void NVIDIAIlluminationV1Controller::getColor()
{
    getControl();
}


std::vector<NV_GPU_CLIENT_ILLUM_ZONE_TYPE> NVIDIAIlluminationV1Controller::getInfo()
{
    std::vector<NV_GPU_CLIENT_ILLUM_ZONE_TYPE> zoneTypes;
    getControl();
    for (unsigned int i = 0; i < zoneParams.numIllumZonesControl; i++)
    {
        zoneTypes.push_back(zoneParams.zones[i].type);
    }
    return zoneTypes;
}
#endif