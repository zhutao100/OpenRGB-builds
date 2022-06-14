/*----------------------------------------------*\
|  NVIDIAIlluminationControllerDetect.cpp        |
|                                                |
|  Detect GPUS that are controlled by the direct |
|  NVIDIA Illumination controller.               |
|                                                |
|  Carter Miller (GingerRunner) 1/4/2022         |
\*----------------------------------------------*/

#include "Detector.h"

#include "LogManager.h"
#include "RGBController.h"
#include "RGBController_NVIDIAIllumination.h"

#include "i2c_smbus.h"
#include "pci_ids.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>

enum
{
    NVIDIA_ILLUMINATION_V1
};

#define GPU_NUM_DEVICES (sizeof(device_list) / sizeof(device_list[ 0 ]))

/*-----------------------------------------------------------------------------------------------------*\
| Certain devices seem to ignore the white value entirely, despite the zone being reported back by the  |
| API as RGBW, so this boolean is passed at detection time via constructor inform the controller logic. |
\*-----------------------------------------------------------------------------------------------------*/
#define TREATS_RGBW_AS_RGB  true
#define TREATS_RGBW_AS_RGBW false

/******************************************************************************************\
*                                                                                          *
*       DetectNVIDIAIlluminationGPUControllersRGBWforRGBw                                  *
*                                                                                          *
*       Detect NVIDIA ILLUMINATION GPU controllers that use direct NvAPI Illumination      *
*       calls                                                                              *
*                                                                                          *
*           bus - pointer to i2c_smbus_interface where NVIDIA ILLUMINATION GPU device      *
*                 is connected                                                             *
*                                                                                          *
*           dev - i2c addr                                                                 *
*                                                                                          *
*           name - GPU device name                                                         *
*                                                                                          *
\******************************************************************************************/

void DetectNVIDIAIlluminationGPUControllersRGBWforRGBW(i2c_smbus_interface* bus, uint8_t /*i2c_addr*/, const std::string& name)
{
    if(bus->port_id != 1)
    {
        return;
    }
    NVIDIAIlluminationV1Controller*     new_controller;
    RGBController_NVIDIAIlluminationV1* new_rgbcontroller;

    new_controller          = new NVIDIAIlluminationV1Controller(bus, TREATS_RGBW_AS_RGBW);
    new_rgbcontroller       = new RGBController_NVIDIAIlluminationV1(new_controller);
    new_rgbcontroller->name = name;
    ResourceManager::get()->RegisterRGBController(new_rgbcontroller);
}   /* DetectNVIDIAIlluminationGPUControllersRGBWforRGBW() */


/******************************************************************************************\
*                                                                                          *
*       DetectNVIDIAIlluminationGPUControllersRGBforRGBW                                   *
*                                                                                          *
*       Detect NVIDIA ILLUMINATION GPU controllers that use direct NvAPI Illumination      *
*       calls                                                                              *
*                                                                                          *
*           bus - pointer to i2c_smbus_interface where NVIDIA ILLUMINATION GPU device      *
*                 is connected                                                             *
*                                                                                          *
*           dev - i2c addr                                                                 *
*                                                                                          *
*           name - GPU device name                                                         *
*                                                                                          *
\******************************************************************************************/
void DetectNVIDIAIlluminationGPUControllersRGBforRGBW(i2c_smbus_interface* bus, uint8_t /*i2c_addr*/, const std::string& name)
{
    if(bus->port_id != 1)
    {
        return;
    }
    NVIDIAIlluminationV1Controller*     new_controller;
    RGBController_NVIDIAIlluminationV1* new_rgbcontroller;

    new_controller          = new NVIDIAIlluminationV1Controller(bus, TREATS_RGBW_AS_RGB);
    new_rgbcontroller       = new RGBController_NVIDIAIlluminationV1(new_controller);
    new_rgbcontroller->name = name;
    ResourceManager::get()->RegisterRGBController(new_rgbcontroller);

} /* DetectNVIDIAIlluminationGPUControllersRGBforRGBW() */

REGISTER_I2C_PCI_DETECTOR("NVIDIA 2070 SUPER FE",            DetectNVIDIAIlluminationGPUControllersRGBWforRGBW,   NVIDIA_VEN,   NVIDIA_RTX2070S_DEV,           NVIDIA_VEN,   NVIDIA_RTX2070_FE_SUPER_SUB_DEV,            0);
REGISTER_I2C_PCI_DETECTOR("PNY 3060TI XLR8 REVEL EPIC-X",    DetectNVIDIAIlluminationGPUControllersRGBforRGBW,    NVIDIA_VEN,   NVIDIA_RTX3060TI_LHR_DEV,      NVIDIA_VEN,   PNY_RTX_3060TI_XLR8_REVEL_EPIC_X_SUB_DEV,   0);
REGISTER_I2C_PCI_DETECTOR("NVIDIA 3060TI V1 LHR",            DetectNVIDIAIlluminationGPUControllersRGBforRGBW,    NVIDIA_VEN,   NVIDIA_RTX3060TI_V1_LHR_DEV,   NVIDIA_VEN,   NVIDIA_RTX3060TI_V1_LHR_DEV,                0);
REGISTER_I2C_PCI_DETECTOR("NVIDIA 3060TI LHR",               DetectNVIDIAIlluminationGPUControllersRGBWforRGBW,   NVIDIA_VEN,   NVIDIA_RTX3060TI_LHR_DEV,      NVIDIA_VEN,   NVIDIA_RTX3060TI_LHR_DEV,                   0);
REGISTER_I2C_PCI_DETECTOR("NVIDIA 3080 FE",                  DetectNVIDIAIlluminationGPUControllersRGBWforRGBW,   NVIDIA_VEN,   NVIDIA_RTX3080_DEV,            NVIDIA_VEN,   NVIDIA_RTX3080_FE_NON_LHR_SUB_DEV,          0);
