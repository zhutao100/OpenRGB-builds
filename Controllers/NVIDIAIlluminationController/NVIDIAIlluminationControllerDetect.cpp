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
    const char *    name;
} gpu_pci_device;

#define GPU_NUM_DEVICES (sizeof(device_list) / sizeof(device_list[ 0 ]))

static const gpu_pci_device device_list[] =
{
    {NVIDIA_VEN,   NVIDIA_RTX3080_DEV,         NVIDIA_VEN,   NVIDIA_RTX3080_FE_NON_LHR_SUB_DEV,       NVIDIA_ILLUMINATION_V1,    "3080FE Non-LHR"},
    {NVIDIA_VEN,   NVIDIA_RTX2070S_DEV,        NVIDIA_VEN,   NVIDIA_RTX2070_FE_SUPER_SUB_DEV,         NVIDIA_ILLUMINATION_V1,    "2070FE SUPER"},
    {NVIDIA_VEN,   NVIDIA_GTX1070_DEV,         ZOTAC_SUB_VEN,   NVIDIA_GTX1070_AMP_EXTREME_SUB_DEV,   NVIDIA_ILLUMINATION_V1,    "1070 AMP EXTREME"},
};
/******************************************************************************************\
*                                                                                          *
*   DetectNVIDIAIlluminationGPUControllers                                                  *
*                                                                                          *
*       Detect NVIDIA ILLUMINATION GPU controllers for you use NvAPI                       *
*                                                                                          *
*           bus - pointer to i2c_smbus_interface where NVIDIA ILLUMINATION GPU device      *
*                 is connected                                                             *
*           dev - I2C address of NVIDIA ILLUMINATION GPU device                            *
*                                                                                          *
\******************************************************************************************/

void DetectNVIDIAIlluminationGPUControllers(std::vector<i2c_smbus_interface*>& busses)
{
    for (unsigned int bus = 0; bus < busses.size(); bus++)
    {
        for(unsigned int dev_idx = 0; dev_idx < GPU_NUM_DEVICES; dev_idx++)
        {
            if (busses[bus]->port_id != 1)
            {
                break;
            }

            if(busses[bus]->pci_vendor           == device_list[dev_idx].pci_vendor           &&
               busses[bus]->pci_device           == device_list[dev_idx].pci_device           &&
               busses[bus]->pci_subsystem_vendor == device_list[dev_idx].pci_subsystem_vendor &&
               busses[bus]->pci_subsystem_device == device_list[dev_idx].pci_subsystem_device)
            {
                switch(device_list[dev_idx].gpu_rgb_version)
                {
                    case NVIDIA_ILLUMINATION_V1:
                        {
                            LOG_DEBUG(GPU_DETECT_MESSAGE, NVIDIA_ILLUMINATION_V1_CONTROLLER_NAME, bus, device_list[dev_idx].pci_device, device_list[dev_idx].pci_subsystem_device, device_list[dev_idx].name );
                            NVIDIAIlluminationV1Controller*     new_controller;
                            RGBController_NVIDIAIlluminationV1* new_rgbcontroller;

                            // TODO: Slap a QueryIllumSupport if statement around this
                            new_controller          = new NVIDIAIlluminationV1Controller(busses[bus]);
                            new_rgbcontroller       = new RGBController_NVIDIAIlluminationV1(new_controller);
                            new_rgbcontroller->name = device_list[dev_idx].name;
                            ResourceManager::get()->RegisterRGBController(new_rgbcontroller);
                        }
                        break;
                }
            }
        }
    }
}   /* DetectNVIDIAGPUControllers() */

REGISTER_I2C_DETECTOR("NVIDIA FOUNDERS GPU", DetectNVIDIAIlluminationGPUControllers);
