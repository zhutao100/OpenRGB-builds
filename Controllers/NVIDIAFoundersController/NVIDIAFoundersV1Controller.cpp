/*-----------------------------------------*\
|  NVIDIAFoundersV1Controller.cpp           |
|                                           |
|  Driver for NVIDIA Founders GPUs          |
|                                           |
|  Carter Miller (GingerRunner) 1/5/2022    |
\*-----------------------------------------*/

#include "NVIDIAFoundersV1Controller.h"
#include "LogManager.h"

NVIDIAFoundersV1Controller::NVIDIAFoundersV1Controller(i2c_smbus_interface* bus_ptr, zoneinfo info)
{
    bus = bus_ptr;
    zi = info;
}

NVIDIAFoundersV1Controller::~NVIDIAFoundersV1Controller()
{
}

std::string NVIDIAFoundersV1Controller::GetDeviceLocation()
{
    std::string return_string(bus->device_name);
    char addr[5];
    snprintf(addr, 5, "0x%02X", zi.dev_addr);
    return_string.append(", address ");
    return_string.append(addr);
    return("I2C: " + return_string);
}

void NVIDIAFoundersV1Controller::SetColor(unsigned char red, unsigned char green, unsigned char blue)
{
    SendCommand(EVGA_GP102_CMD_BEGIN);
    SendCommand(EVGA_GP102_CMD_COLOR);

    if (CommandAcknowledged())
    {
        unsigned char rgb[] = { red, green, blue };

        for (int i = 0; i < 3; i++)
        {
            bus->i2c_smbus_write_byte_data(zi.dev_addr, zi.color_addrs[i], rgb[i]);
        }

        SendCommand(EVGA_GP102_CMD_END);

        if (!CommandCompleted())
        {
            LOG_WARNING("[%s] Non-clear status report from hardware.", NVIDIA_FOUNDERS_V1_CONTROLLER_NAME);
        }
    }
}
std::array<unsigned char, 3> NVIDIAFoundersV1Controller::GetColor()
{
    return { GetRed(), GetGreen(), GetBlue() };
}

bool NVIDIAFoundersV1Controller::IsValid()
{
    for (int i = 0; i < 3; i++)
    {
        char res = bus->i2c_smbus_read_byte_data(zi.dev_addr, EVGA_GP102_REG_VALID);
        if (res == 0x1F)
        {
            LOG_TRACE("[%s] Zone discovery successful on address: 0x%02X.", NVIDIA_FOUNDERS_V1_CONTROLLER_NAME, zi.dev_addr);
            return true;
        }
        LOG_DEBUG("[%s] Zone discovery failed on address: 0x%02X expected: 0x1F received: 0x%02X.", NVIDIA_FOUNDERS_V1_CONTROLLER_NAME, zi.dev_addr, res);
    }
    return false;
}

void NVIDIAFoundersV1Controller::SetMode(unsigned char mode)
{
    bus->i2c_smbus_write_byte_data(zi.dev_addr, EVGA_GP102_REG_MODE, mode);
}

unsigned char NVIDIAFoundersV1Controller::GetMode()
{
    return(bus->i2c_smbus_read_byte_data(zi.dev_addr, EVGA_GP102_REG_MODE));
}

void NVIDIAFoundersV1Controller::SendCommand(s32 command)
{
    bus->i2c_smbus_write_byte_data(zi.dev_addr, EVGA_GP102_REG_CMD, command);
}

s32 NVIDIAFoundersV1Controller::QueryCommand(s32 command)
{
    return bus->i2c_smbus_read_byte_data(zi.dev_addr, command);
}

bool NVIDIAFoundersV1Controller::CommandAcknowledged()
{
    return QueryCommand(EVGA_GP102_REG_CMD) == zi.resp_ready;
}

bool NVIDIAFoundersV1Controller::CommandCompleted()
{
    return QueryCommand(EVGA_GP102_REG_CMD) == zi.resp_clear;
}

unsigned char NVIDIAFoundersV1Controller::GetRed()
{
    return(bus->i2c_smbus_read_byte_data(zi.dev_addr, zi.color_addrs[EVGA_GP102_CIDX_RED]));
}

unsigned char NVIDIAFoundersV1Controller::GetGreen()
{
    return(bus->i2c_smbus_read_byte_data(zi.dev_addr, zi.color_addrs[EVGA_GP102_CIDX_GREEN]));
}

unsigned char NVIDIAFoundersV1Controller::GetBlue()
{
    return(bus->i2c_smbus_read_byte_data(zi.dev_addr, zi.color_addrs[EVGA_GP102_CIDX_BLUE]));
}

std::string NVIDIAFoundersV1Controller::GetName()
{
    return zi.zone_name;
}
