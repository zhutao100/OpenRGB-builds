#ifndef NVAPI_H
#define NVAPI_H

#include <stdint.h>

typedef int32_t NV_S32;
typedef uint32_t NV_U32;
typedef uint8_t NV_U8;
typedef uint16_t NV_U16;

typedef NV_S32* NV_HANDLE;
typedef NV_HANDLE NV_PHYSICAL_GPU_HANDLE;
typedef NV_HANDLE NV_VIRTUAL_GPU_HANDLE;
typedef NV_HANDLE NV_UNATTACHED_DISPLAY_HANDLE;
typedef NV_HANDLE NV_DISPLAY_HANDLE;

typedef char NV_SHORT_STRING[64];

typedef NV_S32 NV_STATUS;

#define NV_STRUCT_VERSION(STRUCT, VERSION) \
	(((VERSION) << 16) | sizeof(STRUCT))

enum class NV_CLOCK_SYSTEM : NV_S32 {
	GPU,
	MEMORY,
	SHADER
};

enum class NV_DYNAMIC_PSTATES_SYSTEM : NV_S32 {
	GPU,
	FB,
	VID,
	BUS
};

struct NV_DELTA_ENTRY {
	NV_DELTA_ENTRY();
	NV_S32 value;
	NV_S32 value_min;
	NV_S32 value_max;
};

struct NV_GPU_PSTATES20_V2 {
	NV_GPU_PSTATES20_V2();
	NV_U32 version;
	NV_U32 flags;
	NV_U32 state_count;
	NV_U32 clock_count;
	NV_U32 voltage_count;
	struct {
		NV_U32 state_num;
		NV_U32 flags;
		struct {
			NV_U32 domain;
			NV_U32 type;                     // NOTE(dweiler): 0 = single frequency, 1 = dynamic frequencu
			NV_U32 flags;                    // NOTE(dweiler): flags don't appear to be enforced by NVAPI
			NV_DELTA_ENTRY frequency_delta;  // NOTE(dweiler): only valid when type == 1
			NV_U32 min_or_single_frequency;  // NOTE(dweiler): only valid when type == 0
			NV_U32 max_frequency;            // NOTE(dweiler): only valid when type == 1
			NV_U32 voltage_domain;           // NOTE(dweiler): only valid when type == 1
			NV_U32 min_voltage;              // NOTE(dweiler): only valid when type == 1
			NV_U32 max_voltage;              // NOTE(dweiler): only valid when type == 1
		} clocks[8];
		struct {
			NV_U32 domain;
			NV_U32 flags;                    // NOTE(dweiler): base voltage can only be changed if bit 0 is set
			NV_U32 voltage;
			NV_DELTA_ENTRY voltage_delta;
		} base_voltages[4];                  // NOTE(dweiler): base voltage (resting voltage wheen given a pstate) for all available voltage domains
	} states[16];
	struct {
		NV_U32 voltage_count;
		struct {
			NV_U32 domain;
			NV_U32 flags;
			NV_U32 voltage;
			NV_DELTA_ENTRY voltage_delta;
		} voltages[4];
	} over_voltage;
};

enum class NV_CLOCK_FREQUENCY_TYPE : NV_S32 {
	CURRENT,
	BASE,
	BOOST,
	LAST
};

struct NV_CLOCK_FREQUENCIES_V2 {
	NV_CLOCK_FREQUENCIES_V2();
	NV_U32 version;
	NV_U32 clock_type;
	struct {
		NV_U32 present;
		NV_U32 frequency;
	} entries[32];
};

struct NV_GPU_PERFORMANCE_TABLE_V1 {
	NV_GPU_PERFORMANCE_TABLE_V1();
	NV_U32 version;
	NV_U32 plevel_count;
	NV_U32 : 32;                  // NOTE(dweiler): unknown value
	NV_U32 domain_entries;
	NV_U32 : 32;                  // NOTE(dweiler): unknown value
	NV_U32 pstate_level;
	NV_U32 : 32;                  // NOTE(dweiler): unknown value
	struct {
		struct {
			NV_U32 domain;
			NV_U32 : 32;          // NOTE(dweiler): unknown value
			NV_U32 clock;
			NV_U32 default_clock;
			NV_U32 min_clock;
			NV_U32 max_clock;
			NV_U32 : 32;          // NOTE(dweiler): unknown value
		} domains[32];
		NV_U32 : 32;              // NOTE(dweiler): unknown value
		NV_U32 setting_flags;
	} entries[10];
	NV_U32 unknown[450];          // NOTE(dweiler): the following block of memory is completely unknown
};

struct NV_DYNAMIC_PSTATES_V1 {
	NV_DYNAMIC_PSTATES_V1();
	NV_U32 version;
	NV_U32 flags;
	struct {
		NV_U32 present;
		NV_U32 value;
	} pstates[8];
};

struct NV_GPU_POWER_POLICIES_INFO_V1 {
	NV_GPU_POWER_POLICIES_INFO_V1();
	NV_U32 version;
	NV_U32 flags;
	struct {
		NV_U32 pstate;
		NV_U32 : 32;              // NOTE(dweiler): unknown value
		NV_U32 : 32;              // NOTE(dweiler): unknown value
		NV_U32 min_power;
		NV_U32 : 32;              // NOTE(dweiler): unknown value
		NV_U32 : 32;              // NOTE(dweiler): unknown value
		NV_U32 default_power;
		NV_U32 : 32;              // NOTE(dweiler): unknown value
		NV_U32 : 32;              // NOTE(dweiler): unknown value
		NV_U32 max_power;
		NV_U32 : 32;              // NOTE(dweiler): unknown value
	} entries[4];
};

struct NV_GPU_POWER_POLICIES_STATUS_V1 {
	NV_GPU_POWER_POLICIES_STATUS_V1();
	NV_U32 version;
	NV_U32 count;
	struct {
		NV_U32 pstate;            // NOTE(dweiler): assumed?
		NV_U32 : 32;              // NOTE(dweiler): unknown value
		NV_U32 power; 
		NV_U32 : 32;              // NOTE(dweiler): unknown value
	} entries[4];
};

struct NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 {
	NV_GPU_VOLTAGE_DOMAINS_STATUS_V1();
	NV_U32 version;
	NV_U32 flags;
	NV_U32 count;
	struct {
		NV_U32 voltage_domain;
		NV_U32 current_voltage;
	} entries[16];
};

enum class NV_THERMAL_CONTROLLER : NV_S32 {
	NONE,
	GPU_INTERNAL,
	ADM103,
	MAX6649,
	MAX1617,
	LM99,
	LM89,
	LM64,
	ADT7473,
	SBMAX6649,
	VBIOSEVT,
	OS,
	UNKNOWN = -1
};

enum class NV_THERMAL_TARGET : NV_S32 {
	NONE         = 0,
	GPU          = 1,
	MEMORY       = 2,
	POWER_SUPPLY = 4,
	BOARD        = 8,
	ALL          = 15,
	UNKNOWN      = -1
};

enum class NV_I2C_SPEED : NV_U32 {
	NVAPI_I2C_SPEED_DEFAULT,
	NVAPI_I2C_SPEED_3KHZ,
	NVAPI_I2C_SPEED_10KHZ,
	NVAPI_I2C_SPEED_33KHZ,
	NVAPI_I2C_SPEED_100KHZ,
	NVAPI_I2C_SPEED_200KHZ,
	NVAPI_I2C_SPEED_400KHZ
};

struct NV_GPU_THERMAL_SETTINGS_V2 {
	NV_GPU_THERMAL_SETTINGS_V2();
	NV_U32 version;
	NV_U32 count;
	struct {
		NV_THERMAL_CONTROLLER controller;
		NV_S32 default_min;
		NV_S32 default_max;
		NV_S32 current_temperature;
		NV_THERMAL_TARGET target;
	} sensor[3];
};

struct NV_GPU_THERMAL_POLICIES_INFO_V2 {
	NV_GPU_THERMAL_POLICIES_INFO_V2();
	NV_U32 version;
	NV_U32 flags;
	struct {
		NV_U32 controller;    // NOTE(dweiler): can't be NV_THERMAL_CONTROLLER since this needs to be unsigned
		NV_U32 : 32;          // NOTE(dweiler): unknown value
		NV_S32 min;           // NOTE(dweiler): stored as multiples of 256
		NV_S32 default_;      // NOTE(dweiler): stored as multiples of 256
		NV_S32 max;           // NOTE(dweiler): stored as multiples of 256
		NV_U32 default_flags; // NOTE(dweiler): bit zero of the flags indicates the thermal priority
	} entries[4];
};

struct NV_GPU_THERMAL_POLICIES_STATUS_V2 {
	NV_GPU_THERMAL_POLICIES_STATUS_V2();
	NV_U32 version;
	NV_U32 count;
	struct {
		NV_U32 controller;
		NV_U32 value;         // NOTE(dweiler): stored as multiples of 256
		NV_U32 flags;         // NOTE(dweiler): bit zero of the flags indicates the thermal priority
	} entries[4];
};

struct NV_GPU_COOLER_SETTINGS_V2 {
	NV_GPU_COOLER_SETTINGS_V2();
	NV_U32 version;
	NV_U32 count;
	struct {
		NV_S32 type;
		NV_S32 controller;
		NV_S32 default_min;
		NV_S32 default_max;
		NV_S32 current_min;
		NV_S32 current_max;
		NV_S32 current_level;
		NV_S32 default_policy;
		NV_S32 current_policy;
		NV_S32 target;
		NV_S32 control_type;
		NV_S32 active;
	} coolers[20];
};

struct NV_GPU_COOLER_LEVELS_V1 {
	NV_GPU_COOLER_LEVELS_V1();
	NV_U32 version;
	struct {
		NV_S32 level;
		NV_S32 policy; // NOTE(dweiler): 0x20 is default policy, 0x01 is user supplied policy
		               // TODO(dweiler): figure out what other policy values are valid
	} levels[20];
};

struct NV_MEMORY_INFO_V2 {
	NV_MEMORY_INFO_V2();
	NV_U32 version;
	NV_U32 values[5];
};

struct NV_DISPLAY_DRIVER_VERSION_V1 {
	NV_DISPLAY_DRIVER_VERSION_V1();
	NV_U32 version;
	NV_U32 driver_version;        // NOTE(dweiler): major = (driver_version / 100), minor = (driver_version % 100)
	NV_U32 : 32;                  // NOTE(dweiler): unknown vaue
	NV_SHORT_STRING build_branch;
	NV_SHORT_STRING adapter;
};

struct NV_I2C_INFO_V3 {
	NV_I2C_INFO_V3();
	NV_U32 version;
	NV_U32 display_mask;
	NV_U8 is_ddc_port;
	NV_U8 i2c_dev_address;
	NV_U8* i2c_reg_address;
	NV_U32 reg_addr_size;
	NV_U8* data;
	NV_U32 size;
	NV_U32 i2c_speed;
	NV_I2C_SPEED i2c_speed_khz;
	NV_U8 port_id;
	NV_U32 is_port_id_set;
};

// NvAPI RGB related stuff, if you start at NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS 
// and read upwards, it will make more sense (CMiller)
struct NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED
{
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED_PARAMS colorFixedParams;
};

struct NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_COLOR_FIXED
{
    union
    {
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED           manualColorFixed;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_COLOR_FIXED piecewiseLinearColorFixed;
        NV_U8                                         rsvd[64];
    } data;
    NV_U8    rsvd[64];
};

struct NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED_PARAMS
{
    NV_U8 brightnessPct;
};

#define NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_COLOR_ENDPOINTS           2
struct NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_COLOR_FIXED
{
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_COLOR_FIXED_PARAMS colorFixedParams[NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_COLOR_ENDPOINTS];
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR          piecewiseLinearData;
};

struct NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_RGB
{
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB_PARAMS rgbParams[NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_COLOR_ENDPOINTS];
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR  piecewiseLinearData;
};

enum NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_TYPE
{
    NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_HALF_HALT = 0,
    NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_FULL_HALT,
    NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_FULL_REPEAT,
    NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_INVALID = 0xFF,
};

struct NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR
{
    NV_GPU_CLIENT_ILLUM_PIECEWISE_LINEAR_CYCLE_TYPE    cycleType;
    NV_U8    grpCount;
    NV_U16   riseTimems;
    NV_U16   fallTimems;
    NV_U16   ATimems;
    NV_U16   BTimems;
    NV_U16   grpIdleTimems;
    NV_U16   phaseOffsetms;
};

struct NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB_PARAMS
{
    NV_U8 colorR;
    NV_U8 colorG;
    NV_U8 colorB;
    NV_U8 brightnessPct;
};

struct NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB
{
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB_PARAMS rgbParams;
};

struct NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGB
{
    union
    {
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_MANUAL_RGB            manualRGB;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_PIECEWISE_LINEAR_RGB  piecewiseLinearRGB;
        NV_U8                                         rsvd[64];
    } data;
    NV_U8    rsvd[64];
};

enum NV_GPU_CLIENT_ILLUM_CTRL_MODE
{
    NV_GPU_CLIENT_ILLUM_CTRL_MODE_MANUAL_RGB = 0,
    NV_GPU_CLIENT_ILLUM_CTRL_MODE_PIECEWISE_LINEAR_RGB,

    // Only add new control modes above this.
    NV_GPU_CLIENT_ILLUM_CTRL_MODE_INVALID = 0xFF,
};

enum NV_GPU_CLIENT_ILLUM_ZONE_TYPE
{
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE_INVALID = 0,
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE_RGB,
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE_COLOR_FIXED,
};

struct NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_V1
{
    NV_GPU_CLIENT_ILLUM_ZONE_TYPE  type;
    NV_GPU_CLIENT_ILLUM_CTRL_MODE  ctrlMode;
    union
    {
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_RGB           rgb;
        NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_DATA_COLOR_FIXED   colorFixed;
        NV_U8                                                rsvd[64];
    } data;
    NV_U8    rsvd[64];
};

#define NV_GPU_CLIENT_ILLUM_ZONE_NUM_ZONES_MAX 32
struct NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS {

    NV_U32 version;
 
    NV_U32 bDefault : 1;
    NV_U32 rsvdField : 31;

    NV_U32 numIllumZonesControl;

    NV_U8 rsvd[64];
  
    NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_V1 zones[NV_GPU_CLIENT_ILLUM_ZONE_NUM_ZONES_MAX];

};


// Interface: 0150E828
NV_STATUS NvAPI_Initialize();

// Interface: D22BDD7E
NV_STATUS NvAPI_Unload();

// Interface: 9ABDD40D
NV_STATUS NvAPI_EnumDisplayHandle(
	NV_S32 this_enum,
	NV_DISPLAY_HANDLE *display_handle);

// Interface: E5AC921F
NV_STATUS NvAPI_EnumPhysicalGPUs(
	NV_PHYSICAL_GPU_HANDLE *physical_gpu_handles,
	NV_S32 *gpu_count);

// Interface: F951A4D1
NV_STATUS NvAPI_GetDisplayDriverVersion(
	NV_DISPLAY_HANDLE display_handle,
	NV_DISPLAY_DRIVER_VERSION_V1 *display_driver_version);

// Interface: 01053FA5
NV_STATUS NvAPI_GetInterfaceVersionString(
	NV_SHORT_STRING version);

// Interface: 34EF9506
NV_STATUS NvAPI_GetPhysicalGPUsFromDisplay(
	NV_DISPLAY_HANDLE display_handle,
	NV_PHYSICAL_GPU_HANDLE *gpu_handles,
	NV_U32 *gpu_count);

// Interface: 774AA982
NV_STATUS NvAPI_GetMemoryInfo(
	NV_DISPLAY_HANDLE display_handle,
	NV_MEMORY_INFO_V2 *memory_info);

// Interface: 0CEEE8E9F
NV_STATUS NvAPI_GPU_GetFullName(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_SHORT_STRING name);

// Interface: 6FF81213
NV_STATUS NvAPI_GPU_GetPStates20(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_GPU_PSTATES20_V2 *pstates);

// Interface: 0F4DAE6B
NV_STATUS NvAPI_GPU_SetPStates20(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_GPU_PSTATES20_V2 *pstates);

// Get frequencies of all clocks of the GPU
//
// The actual frequencies (current, base, boost) returned is based on the value
// set in frequencies->clock_type before calling this function. The value values
// are part of the NV_CLOCK_FREQUENCY_TYPE enumeration.
//
// Interface: DCB616C3
NV_STATUS NvAPI_GPU_GetAllClockFrequencies(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_CLOCK_FREQUENCIES_V2 *frequencies);

// Interface: 60DED2ED
NV_STATUS NvAPI_GPU_GetDynamicPStates(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_DYNAMIC_PSTATES_V1 *dynamic_pstates);

// Interface: 34206D86
NV_STATUS NvAPI_GPU_GetPowerPoliciesInfo(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_GPU_POWER_POLICIES_INFO_V1 *policies_info);

// Interface: 70916171
NV_STATUS NvAPI_GPU_GetPowerPoliciesStatus(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_GPU_POWER_POLICIES_STATUS_V1 *policies_status);

// Interface: 0C16C7E2C
NV_STATUS NvAPI_GPU_GetVoltageDomainStatus(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_GPU_VOLTAGE_DOMAINS_STATUS_V1 *voltage_domains_status);

// Get the thermal settings of the GPU
//
// The value of [sensor_index] must be one of the values of NV_THERMAL_TARGET,
// either a single sensor, or the special value NV_THERMAL_TARGET::ALL for all
// sensors present on the GPU
//
// Interface: 0E3640A56
NV_STATUS NvAPI_GPU_GetThermalSettings(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_THERMAL_TARGET sensor_index,
	NV_GPU_THERMAL_SETTINGS_V2 *thermal_settings);

// Get the serial number of the GPU
//
// The NV_SHORT_STRING will be filled out accordingly
//
// Interface: 014B83A5F
NV_STATUS NvAPI_GPU_GetSerialNumber(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_SHORT_STRING serial_number);

// Interface: 0AD95F5ED
NV_STATUS NvAPI_GPU_SetPowerPoliciesStatus(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_GPU_POWER_POLICIES_STATUS_V1* policies_status);

// Interface: 00D258BB5
NV_STATUS NvAPI_GPU_GetThermalPoliciesInfo(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_GPU_THERMAL_POLICIES_INFO_V2* thermal_info);

// Interface: 0E9C425A1
NV_STATUS NvAPI_GPU_GetThermalPoliciesStatus(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_GPU_THERMAL_POLICIES_STATUS_V2* thermal_status);

// Interface: 034C0B13D
NV_STATUS NvAPI_GPU_SetThermalPoliciesStatus(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_GPU_THERMAL_POLICIES_STATUS_V2* thermal_status);

// Interface: DA141340
NV_STATUS NvAPI_GPU_GetCoolerSettings(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_S32 cooler_index,
	NV_GPU_COOLER_SETTINGS_V2 *cooler_settings);

// Interface: 891FA0AE
NV_STATUS NvAPI_GPU_SetCoolerLevels(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_S32 cooler_index,
	NV_GPU_COOLER_LEVELS_V1 *cooler_levels);

// Interface: 2DDFB66E
NV_STATUS NvAPI_GPU_GetPCIIdentifiers(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_U32 *device_id,
	NV_U32 *sub_system_id,
	NV_U32 *revision_id,
	NV_U32 *ext_device_id);

// Interface: 283AC65A
NV_STATUS NvAPI_I2CWriteEx(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_I2C_INFO_V3 *i2c_info,
	NV_U32 *unknown);

// Interface: 4D7B0709
NV_STATUS NvAPI_I2CReadEx(
	NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
	NV_I2C_INFO_V3* i2c_info,
	NV_U32 *unknown);

// Interface: 73C01D58
NV_STATUS NvAPI_GPU_ClientIllumZonesGetControl(
    __in NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
    __inout NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS* pIllumZonesControl);

// Interface: 57024C62
NV_STATUS NvAPI_GPU_ClientIllumZonesSetControl(
    __in NV_PHYSICAL_GPU_HANDLE physical_gpu_handle,
    __inout NV_GPU_CLIENT_ILLUM_ZONE_CONTROL_PARAMS* pIllumZonesControl);

#endif
