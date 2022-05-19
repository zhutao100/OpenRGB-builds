/*-----------------------------------------------*\
|  NVIDIAIlluminationV1Controller.cpp             |
|                                                 |
|  Driver for NVIDIA Illumination controlled GPUS |
|                                                 |
|  Carter Miller (GingerRunner) 1/5/2022          |
\*-----------------------------------------------*/
#ifdef _WIN32

#include "NVIDIAIlluminationV1Controller.h"

NVIDIAIlluminationV1Controller::NVIDIAIlluminationV1Controller(i2c_smbus_interface* bus_ptr, bool treats_rgbw_as_rgb)
{
    bus = bus_ptr;
    _treats_rgbw_as_rgb = treats_rgbw_as_rgb;
}

NVIDIAIlluminationV1Controller::~NVIDIAIlluminationV1Controller()
{
}

void NVIDIAIlluminationV1Controller::getControl()
{
    std::memset(&zoneParams, 0, sizeof(zoneParams));
    // Hardcoded value found via sniffing, this may be different for other cards, once that is 
    // found, may be best to simply if/else this based on the card detected or map it out in the detector and 
    // then pass via constructor to here?
    zoneParams.version = 72012;
    zoneParams.bDefault = 0;
    // As far as I can tell, this pre-populates the zone type value, as well as the number of zones
    // able to be controlled, and their existing settings, very useful for extending this controller.
    bus->nvapi_xfer(NVAPI_ZONE_GET_CONTROL, &zoneParams);
}

void NVIDIAIlluminationV1Controller::setControl()
{
    bus->nvapi_xfer(NVAPI_ZONE_SET_CONTROL, &zoneParams);
}

// This function exists to check if RGB colors are all set to zero, and if so, to take the brightness down 
// to zero.  This was done to comply with functionality in OpenRGB such as "Lights Off" which sends RGB
// values of all zeroes, but doesn't seem to send a brightness of zero.
bool NVIDIAIlluminationV1Controller::allZero(std::array<uint8_t, 4> colors)
{
    std::array<uint8_t, 4> allZeros = {0, 0, 0, 0};
    return colors == allZeros;
}

void NVIDIAIlluminationV1Controller::setZoneRGBW(uint8_t zone, uint8_t red, uint8_t green, uint8_t blue, uint8_t white, uint8_t brightness)
{
    zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorR = red;
    zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorG = green;
    zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorB = blue;
    zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.colorW = white;
    zoneParams.zones[zone].data.rgbw.data.manualRGBW.rgbwParams.brightnessPct = brightness;
}

void NVIDIAIlluminationV1Controller::setZoneRGB(uint8_t zone, uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness)
{
    zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorR = red;
    zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorG = green;
    zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorB = blue;
    zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.brightnessPct = brightness;
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
                setZoneRGB(zone, 0, 0, 0, 0);
            }
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGBW)
            {
                setZoneRGBW(zone, 0, 0, 0, 0, 0);
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
                setZoneRGB(zone, red, green, blue, zone_config.brightness);
            }
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGBW)
            {
                // Certain devices seem to ignore the white value entirely, despite the zone being reported back by the 
                // API as RGBW, as such, this if statement was added to conduct a different course of action based 
                // on definitions placed in the controller page
                if(!_treats_rgbw_as_rgb)
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
                    // If difference between the highest and lowest RGB values is 10 or lower, set the white value only,
                    // zero out the rest, this logic was found via tedious examination 
                    if (maxRGBvalue - minRGBvalue <= 10)
                    {
                        red = 0;
                        green = 0;
                        blue = 0;
                        white = (maxRGBvalue + minRGBvalue)/2;
                    }
                }
                setZoneRGBW(zone, red, green, blue, white, zone_config.brightness);
            }
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_SINGLE_COLOR)
            {
                zoneParams.zones[zone].data.singleColor.data.manualSingleColor.singleColorParams.brightnessPct = allZero({red, green, blue, white}) ? 0 : zone_config.brightness;
            }
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED)
            {
                zoneParams.zones[zone].data.colorFixed.data.manualColorFixed.colorFixedParams.brightnessPct = allZero({red, green, blue, white}) ? 0 : zone_config.brightness;
            }
            else if (zoneParams.zones[zone].type == NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED)
            {
                zoneParams.zones[zone].data.rgb.data.manualRGB.rgbParams.colorR = zone_config.brightness;
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