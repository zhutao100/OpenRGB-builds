/*----------------------------------------------*\
|  NVIDIAIlluminationControllerDetect.cpp        |
|                                                |
|  Detect GPUS that are controlled by the direct |
|  NVIDIA Illumination controller.               |
|                                                |
|  Carter Miller (GingerRunner) 1/4/2022         |
\*----------------------------------------------*/

#ifdef _WIN32
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

typedef struct
{
    int             pci_vendor;
    int             pci_device;
    int             pci_subsystem_vendor;
    int             pci_subsystem_device;
    int             gpu_rgb_version;
    bool            treats_rgbw_as_rgb;
    const char *    name;
} gpu_pci_device;

#define GPU_NUM_DEVICES (sizeof(device_list) / sizeof(device_list[ 0 ]))

// These defines and the additional boolen added to the structure help with controller logic for a weird edge case involving a combination of zone type + specific device being controlled
#define TREATS_RGBW_AS_RGB  true
#define TREATS_RGBW_AS_RGBW false

static const gpu_pci_device device_list[] =
{
    {NVIDIA_VEN,   NVIDIA_RTX2070S_DEV,          NVIDIA_VEN,        NVIDIA_RTX2070_FE_SUPER_SUB_DEV,             NVIDIA_ILLUMINATION_V1,    TREATS_RGBW_AS_RGBW,   "NVIDIA 2070 SUPER FE"        },
    {NVIDIA_VEN,   NVIDIA_RTX3060TI_LHR_DEV,     PNY_SUB_VEN,       PNY_RTX_3060TI_XLR8_REVEL_EPIC_X_SUB_DEV,    NVIDIA_ILLUMINATION_V1,    TREATS_RGBW_AS_RGB,    "PNY 3060TI XLR8 REVEL EPIC-X"},
    {NVIDIA_VEN,   NVIDIA_RTX3060TI_V1_LHR_DEV,  NVIDIA_VEN,        NVIDIA_RTX3060TI_V1_LHR_DEV,                 NVIDIA_ILLUMINATION_V1,    TREATS_RGBW_AS_RGB,    "NVIDIA 3060TI V1 LHR"        },
    {NVIDIA_VEN,   NVIDIA_RTX3060TI_LHR_DEV,     NVIDIA_VEN,        NVIDIA_RTX3060TI_LHR_DEV,                    NVIDIA_ILLUMINATION_V1,    TREATS_RGBW_AS_RGBW,   "NVIDIA 3060TI LHR"           },
    {NVIDIA_VEN,   NVIDIA_RTX3080_DEV,           NVIDIA_VEN,        NVIDIA_RTX3080_FE_NON_LHR_SUB_DEV,           NVIDIA_ILLUMINATION_V1,    TREATS_RGBW_AS_RGBW,   "NVIDIA 3080 FE"              },
};
/******************************************************************************************\
*                                                                                          *
*       DetectNVIDIAIlluminationGPUControllers                                             *
*                                                                                          *
*       Detect NVIDIA ILLUMINATION GPU controllers that use direct NvAPI Illumination      *
*       calls                                                                              *
*                                                                                          *
*           bus - pointer to i2c_smbus_interface where NVIDIA ILLUMINATION GPU device      *
*                 is connected                                                             *
*           dev - I2C address of NVIDIA ILLUMINATION GPU device                            *
*                                                                                          *
\******************************************************************************************/

void DetectNVIDIAIlluminationGPUControllers(std::vector<i2c_smbus_interface*>& busses)
{
    //LOG_DEBUG("Entered detection function.");
    for (unsigned int bus = 0; bus < busses.size(); bus++)
    {
        LOG_DEBUG("Inside outer for loop.");
        for(unsigned int dev_idx = 0; dev_idx < GPU_NUM_DEVICES; dev_idx++)
        {
            LOG_DEBUG("Inside inner for loop.");
            if (busses[bus]->port_id != 1)
            {
                break;
            }

            if(busses[bus]->pci_vendor           == device_list[dev_idx].pci_vendor           &&
               busses[bus]->pci_device           == device_list[dev_idx].pci_device           &&
               busses[bus]->pci_subsystem_vendor == device_list[dev_idx].pci_subsystem_vendor &&
               busses[bus]->pci_subsystem_device == device_list[dev_idx].pci_subsystem_device)
            {
                LOG_DEBUG("Outside switch statement.");
                switch(device_list[dev_idx].gpu_rgb_version)
                {
                    case NVIDIA_ILLUMINATION_V1:
                        {
                            //LOG_DEBUG(GPU_DETECT_MESSAGE, NVIDIA_ILLUMINATION_V1_CONTROLLER_NAME, bus, device_list[dev_idx].pci_device, device_list[dev_idx].pci_subsystem_device, device_list[dev_idx].name);
                            NVIDIAIlluminationV1Controller*     new_controller;
                            RGBController_NVIDIAIlluminationV1* new_rgbcontroller;

                            // TODO: Slap a QueryIllumSupport if statement around this, maybe?
                            //LOG_DEBUG("Creating Illumination controller...");
                            new_controller          = new NVIDIAIlluminationV1Controller(busses[bus], device_list[dev_idx].treats_rgbw_as_rgb);
                            //LOG_DEBUG("Creating RGB controller...");
                            new_rgbcontroller       = new RGBController_NVIDIAIlluminationV1(new_controller);
                            //LOG_DEBUG("Setting name to device list by index...");
                            new_rgbcontroller->name = device_list[dev_idx].name;
                            //LOG_DEBUG("Regsitering controller via resource manager...");
                            ResourceManager::get()->RegisterRGBController(new_rgbcontroller);
                            //LOG_DEBUG("Past Resource Manager get call...");
                        }
                        break;
                }
            }
        }
    }
}   /* DetectNVIDIAGPUControllers() */

REGISTER_I2C_DETECTOR("NVIDIA ILLUMINATION GPU", DetectNVIDIAIlluminationGPUControllers);
#endif
