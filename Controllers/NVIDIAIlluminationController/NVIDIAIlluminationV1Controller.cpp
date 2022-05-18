/*-----------------------------------------*\
|  NVIDIAIlluminationV1Controller.cpp           |
|                                           |
|  Driver for NVIDIA Founders GPUs          |
|                                           |
|  Carter Miller (GingerRunner) 1/5/2022    |
\*-----------------------------------------*/

#include "NVIDIAIlluminationV1Controller.h"
#include "LogManager.h"

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
    // As far as I can tell, this pre-populates the zone type value!
    bus->nvapi_xfer(NVAPI_ZONE_GET_CONTROL, &zoneParams);
}

void NVIDIAIlluminationV1Controller::setControl()
{
    bus->nvapi_xfer(NVAPI_ZONE_SET_CONTROL, &zoneParams);
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
            // This controller type requries the brightnessPct gets set to the red LED zone.  Not sure why.
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_SINGLE_COLOR)
            {
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorR = 0;
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
                // If difference between the highest and lowest RGB values is 10 or lower, set the white value only
                if (maxRGBvalue - minRGBvalue <= 10)
                {
                    red, green, blue = 0;
                    white = (maxRGBvalue + minRGBvalue)/2;
                }
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorR = red;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorG = green;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorB = blue;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorW = white;
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.brightnessPct = zone_config.brightness;
            }
            // This controller type requries the brightnessPct gets set to the red LED zone.  Not sure why.
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_SINGLE_COLOR)
            {
                zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorR = zone_config.brightness;
            }
            break;
    }
    setControl();
}


std::array<unsigned char, 3> NVIDIAIlluminationV1Controller::getColor()
{
    bus->nvapi_xfer(NVAPI_ZONE_GET_CONTROL, &zoneParams);

    unsigned char red = (unsigned char)zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorR;
    unsigned char green = (unsigned char)zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorG;
    unsigned char blue = (unsigned char)zoneParams.zones[0].data.rgb.data.manualRGB.rgbParams.colorB;

    return {red, green, blue};
}
