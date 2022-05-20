/*-----------------------------------------*\
|  i2c_smbus_nvapi.h                        |
|                                           |
|  Definitions and types for NVidia NvAPI   |
|  I2C driver                               |
|                                           |
|  Adam Honse (CalcProgrammer1) 2/21/2020   |
\*-----------------------------------------*/

#include "i2c_smbus.h"
#ifdef _WIN32
#include "nvapi.h"
#endif

#pragma once

// NVAPI Direct Calls
#define NVAPI_ZONE_GET_CONTROL 0
#define NVAPI_ZONE_SET_CONTROL 1

class i2c_smbus_nvapi : public i2c_smbus_interface
{
public:
    i2c_smbus_nvapi(NV_PHYSICAL_GPU_HANDLE handle);
    s32 i2c_read_block_data(u8 addr, u8 length, u8 *values);
    s32 i2c_write_block_data(u8 addr, u8 length, u8 *values);

private:
    s32 i2c_smbus_xfer(u8 addr, char read_write, u8 command, int mode, i2c_smbus_data* data);
    s32 i2c_xfer(u8 addr, char read_write, int* size, u8* data);
    #ifdef _WIN32
    s32 nvapi_xfer(char nvapi_call, NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS * zone_control_struct);
    #endif
    NV_PHYSICAL_GPU_HANDLE handle;
};
