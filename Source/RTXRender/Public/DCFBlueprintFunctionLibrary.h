// Copyright© 2017 ihomefnt All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Core/Public/GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/SlateCore/Public/Widgets/SWindow.h"
#include "DCFBlueprintFunctionLibrary.generated.h"
 



#define MAX_GPU_NUM     4L      // 监控的GPU个数（NVIDIA定义的最多GPU个数是64，这里最多只监控4个）
#define MY_PROCESS_ERROR(Condition) do { if (!(Condition)) goto Exit0; } while (false)

#define MAX_DISPLAY_CARDS               4      
// 常量定义
#define MAX_PHYSICAL_GPUS               64
#define SHORT_STRING_MAX                64
#define MAX_THERMAL_SENSORS_PER_GPU     3
#define MAX_CLOCKS_PER_GPU              0x120
#define MAX_PSTATES_PER_GPU             8
#define MAX_USAGES_PER_GPU              33
#define MAX_COOLER_PER_GPU              20
#define MAX_MEMORY_VALUES_PER_GPU       5

// 接口ID值
#define ID_NvAPI_Initialize                     0x0150E828
#define ID_NvAPI_GPU_GetFullName                0xCEEE8E9F
#define ID_NvAPI_GPU_GetThermalSettings         0xE3640A56
#define ID_NvAPI_EnumNvidiaDisplayHandle        0x9ABDD40D
#define ID_NvAPI_GetPhysicalGPUsFromDisplay     0x34EF9506
#define ID_NvAPI_EnumPhysicalGPUs               0xE5AC921F
#define ID_NvAPI_GPU_GetTachReading             0x5F608315
#define ID_NvAPI_GPU_GetAllClocks               0x1BD69F49
#define ID_NvAPI_GPU_GetPStates                 0x60DED2ED
#define ID_NvAPI_GPU_GetUsages                  0x189A1FDF
#define ID_NvAPI_GPU_GetCoolerSettings          0xDA141340
#define ID_NvAPI_GPU_SetCoolerLevels            0x891FA0AE
#define ID_NvAPI_GPU_GetMemoryInfo              0x774AA982
#define ID_NvAPI_GetDisplayDriverVersion        0xF951A4D1
#define ID_NvAPI_GetInterfaceVersionString      0x01053FA5
#define ID_NvAPI_GPU_GetPCIIdentifiers          0x2DDFB66E

// 版本号参数定义
#define GPU_THERMAL_SETTINGS_VER                (sizeof(NvGPUThermalSettings) | 0x10000)
#define GPU_CLOCKS_VER                          (sizeof(NvClocks) | 0x20000)
#define GPU_PSTATES_VER                         (sizeof(NvPStates) | 0x10000)
#define GPU_USAGES_VER                          (sizeof(NvUsages) | 0x10000)
#define GPU_COOLER_SETTINGS_VER                 (sizeof(NvGPUCoolerSettings) | 0x20000)
#define GPU_MEMORY_INFO_VER                     (sizeof(NvMemoryInfo) | 0x20000)
#define DISPLAY_DRIVER_VERSION_VER              (sizeof(NvDisplayDriverVersion) | 0x10000)
#define GPU_COOLER_LEVELS_VER                   (sizeof(NvGPUCoolerLevels) | 0x10000)

enum  NvStatus
{
	enumNvStatus_OK = 0,
	enumNvStatus_ERROR = -1,
	enumNvStatus_LIBRARY_NOT_FOUND = -2,
	enumNvStatus_NO_IMPLEMENTATION = -3,
	enumNvStatus_API_NOT_INTIALIZED = -4,
	enumNvStatus_INVALID_ARGUMENT = -5,
	enumNvStatus_NVIDIA_DEVICE_NOT_FOUND = -6,
	enumNvStatus_END_ENUMERATION = -7,
	enumNvStatus_INVALID_HANDLE = -8,
	enumNvStatus_INCOMPATIBLE_STRUCT_VERSION = -9,
	enumNvStatus_HANDLE_INVALIDATED = -10,
	enumNvStatus_OPENGL_CONTEXT_NOT_CURRENT = -11,
	enumNvStatus_NO_GL_EXPERT = -12,
	enumNvStatus_INSTRUMENTATION_DISABLED = -13,
	enumNvStatus_EXPECTED_LOGICAL_GPU_HANDLE = -100,
	enumNvStatus_EXPECTED_PHYSICAL_GPU_HANDLE = -101,
	enumNvStatus_EXPECTED_DISPLAY_HANDLE = -102,
	enumNvStatus_INVALID_COMBINATION = -103,
	enumNvStatus_NOT_SUPPORTED = -104,
	enumNvStatus_PORTID_NOT_FOUND = -105,
	enumNvStatus_EXPECTED_UNATTACHED_DISPLAY_HANDLE = -106,
	enumNvStatus_INVALID_PERF_LEVEL = -107,
	enumNvStatus_DEVICE_BUSY = -108,
	enumNvStatus_NV_PERSIST_FILE_NOT_FOUND = -109,
	enumNvStatus_PERSIST_DATA_NOT_FOUND = -110,
	enumNvStatus_EXPECTED_TV_DISPLAY = -111,
	enumNvStatus_EXPECTED_TV_DISPLAY_ON_DCONNECTOR = -112,
	enumNvStatus_NO_ACTIVE_SLI_TOPOLOGY = -113,
	enumNvStatus_SLI_RENDERING_MODE_NOTALLOWED = -114,
	enumNvStatus_EXPECTED_DIGITAL_FLAT_PANEL = -115,
	enumNvStatus_ARGUMENT_EXCEED_MAX_SIZE = -116,
	enumNvStatus_DEVICE_SWITCHING_NOT_ALLOWED = -117,
	enumNvStatus_TESTING_CLOCKS_NOT_SUPPORTED = -118,
	enumNvStatus_UNKNOWN_UNDERSCAN_CONFIG = -119,
	enumNvStatus_TIMEOUT_RECONFIGURING_GPU_TOPO = -120,
	enumNvStatus_DATA_NOT_FOUND = -121,
	enumNvStatus_EXPECTED_ANALOG_DISPLAY = -122,
	enumNvStatus_NO_VIDLINK = -123,
	enumNvStatus_REQUIRES_REBOOT = -124,
	enumNvStatus_INVALID_HYBRID_MODE = -125,
	enumNvStatus_MIXED_TARGET_TYPES = -126,
	enumNvStatus_SYSWOW64_NOT_SUPPORTED = -127,
	enumNvStatus_IMPLICIT_SET_GPU_TOPOLOGY_CHANGE_NOT_ALLOWED = -128,
	enumNvStatus_REQUEST_USER_TO_CLOSE_NON_MIGRATABLE_APPS = -129,
	enumNvStatus_OUT_OF_MEMORY = -130,
	enumNvStatus_WAS_STILL_DRAWING = -131,
	enumNvStatus_FILE_NOT_FOUND = -132,
	enumNvStatus_TOO_MANY_UNIQUE_STATE_OBJECTS = -133,
	enumNvStatus_INVALID_CALL = -134,
	enumNvStatus_D3D10_1_LIBRARY_NOT_FOUND = -135,
	enumNvStatus_FUNCTION_NOT_FOUND = -136
};

enum  NvThermalController
{
	enumNvThermalController_NONE = 0,
	enumNvThermalController_GPU_INTERNAL,
	enumNvThermalController_ADM1032,
	enumNvThermalController_MAX6649,
	enumNvThermalController_MAX1617,
	enumNvThermalController_LM99,
	enumNvThermalController_LM89,
	enumNvThermalController_LM64,
	enumNvThermalController_ADT7473,
	enumNvThermalController_SBMAX6649,
	enumNvThermalController_VBIOSEVT,
	enumNvThermalController_OS,
	enumNvThermalController_UNKNOWN = -1,
};

enum  NvThermalTarget
{
	enumNvThermalTarget_NONE = 0,
	enumNvThermalTarget_GPU = 1,
	enumNvThermalTarget_MEMORY = 2,
	enumNvThermalTarget_POWER_SUPPLY = 4,
	enumNvThermalTarget_BOARD = 8,
	enumNvThermalTarget_ALL = 15,
	enumNvThermalTarget_UNKNOWN = -1
};

typedef struct __declspec(dllexport) _NvSensor
{
	NvThermalController Controller;
	unsigned int DefaultMinTemp;
	unsigned int DefaultMaxTemp;
	unsigned int CurrentTemp;
	NvThermalTarget Target;
}NvSensor;

typedef struct __declspec(dllexport) _NvGPUThermalSettings
{
	unsigned int Version;
	unsigned int Count;
	NvSensor Sensor[MAX_THERMAL_SENSORS_PER_GPU];
}NvGPUThermalSettings;

typedef struct __declspec(dllexport) _NvClocks
{
	unsigned int Version;
	unsigned int Clock[MAX_CLOCKS_PER_GPU];
}NvClocks;

typedef struct __declspec(dllexport) _NvPState
{
	bool Present;
	int Percentage;
}NvPState;

typedef struct __declspec(dllexport) _NvPStates
{
	unsigned int Version;
	unsigned int Flags;
	NvPState PStates[MAX_PSTATES_PER_GPU];
}NvPStates;

typedef struct __declspec(dllexport) _NvUsages
{
	unsigned int Version;
	unsigned int Usages[MAX_USAGES_PER_GPU];
}NvUsages;

typedef struct __declspec(dllexport) _NvCooler
{
	int Type;
	int Controller;
	int DefaultMin;
	int DefaultMax;
	int CurrentMin;
	int CurrentMax;
	int CurrentLevel;
	int DefaultPolicy;
	int CurrentPolicy;
	int Target;
	int ControlType;
	int Active;
}NvCooler;

typedef struct __declspec(dllexport) _NvGPUCoolerSettings
{
	unsigned int Version;
	unsigned int Count;
	NvCooler Coolers[MAX_COOLER_PER_GPU];
}NvGPUCoolerSettings;

typedef struct __declspec(dllexport) _NvLevel
{
	int Level;
	int Policy;
}NvLevel;

typedef struct __declspec(dllexport) _NvGPUCoolerLevels
{
	unsigned int Version;
	NvLevel Levels[MAX_COOLER_PER_GPU];
}NvGPUCoolerLevels;

typedef struct __declspec(dllexport) _NvMemoryInfo
{
	unsigned int Version;
	unsigned int Values[MAX_MEMORY_VALUES_PER_GPU];
}NvMemoryInfo;

typedef struct __declspec(dllexport) _NvDisplayDriverVersion
{
	unsigned int Version;
	unsigned int DriverVersion;
	unsigned int BldChangeListNum;
	char szBuildBranch[SHORT_STRING_MAX];
	char szAdapter[SHORT_STRING_MAX];
}NvDisplayDriverVersion;

typedef int NvPhysicalGpuHandle;
typedef int NvDisplayHandle;

// 函数定义


extern "C" typedef   void* (*nvapi_QueryInterfaceType)(unsigned int uiInterfaceID);
extern "C" typedef  NvStatus(*NvAPI_InitializeType)();
extern "C" typedef  NvStatus(*NvAPI_GPU_GetFullNameType)(const NvPhysicalGpuHandle gpuHandle, char *pszName);
extern "C" typedef  NvStatus(*NvAPI_GPU_GetThermalSettingsType)(const NvPhysicalGpuHandle gpuHandle, int sensorIndex, NvGPUThermalSettings *pnvGPUThermalSettings);
extern "C" typedef  NvStatus(*NvAPI_EnumNvidiaDisplayHandleType)(const int thisEnum, NvDisplayHandle *pDisplayHandle);
extern "C" typedef  NvStatus(*NvAPI_GetPhysicalGPUsFromDisplayType)(const NvDisplayHandle displayHandle, NvPhysicalGpuHandle *pGpuHandles, unsigned int *pGpuCount);
extern "C" typedef  NvStatus(*NvAPI_EnumPhysicalGPUsType)(NvPhysicalGpuHandle *pGpuHandles, int *pGpuCount);
extern "C" typedef  NvStatus(*NvAPI_GPU_GetTachReadingType)(const NvPhysicalGpuHandle gpuHandle, int *pnValue);
extern "C" typedef  NvStatus(*NvAPI_GPU_GetAllClocksType)(const NvPhysicalGpuHandle gpuHandle, NvClocks *pnvClocks);
extern "C" typedef  NvStatus(*NvAPI_GPU_GetPStatesType)(const NvPhysicalGpuHandle gpuHandle, NvPStates *pnvPStates);
extern "C" typedef  NvStatus(*NvAPI_GPU_GetUsagesType)(const NvPhysicalGpuHandle gpuHandle, NvUsages *pnvUsages);
extern "C" typedef  NvStatus(*NvAPI_GPU_GetCoolerSettingsType)(const NvPhysicalGpuHandle gpuHandle, int coolerIndex, NvGPUCoolerSettings *pnvGPUCoolerSettings);
extern "C" typedef  NvStatus(*NvAPI_GPU_SetCoolerLevelsType)(const NvPhysicalGpuHandle gpuHandle, int coolerIndex, NvGPUCoolerLevels *pnvGPUCoolerLevels);
extern "C" typedef  NvStatus(*NvAPI_GPU_GetMemoryInfoType)(const NvDisplayHandle displayHandle, NvMemoryInfo *pnvMemoryInfo);
extern "C" typedef  NvStatus(*NvAPI_GetDisplayDriverVersionType)(const NvDisplayHandle displayHandle, NvDisplayDriverVersion *pnvDisplayDriverVersion);
extern "C" typedef  NvStatus(*NvAPI_GetInterfaceVersionStringType)(char *pszVersion);
extern "C" typedef  NvStatus(*NvAPI_GPU_GetPCIIdentifiersType)(
		const NvPhysicalGpuHandle gpuHandle,
		unsigned int *puiDeviceId,
		unsigned int *puiSubSystemId,
		unsigned int *puiRevisionId,
		unsigned int *puiExtDeviceId
		);

extern "C" static nvapi_QueryInterfaceType m_pfnNvapi_QueryInterface;
extern "C" static NvAPI_InitializeType m_pfnNvAPI_Initialize;
extern "C" static NvAPI_GPU_GetFullNameType m_pfnNvAPI_GPU_GetFullName;
extern "C" static NvAPI_GPU_GetThermalSettingsType m_pfnNvAPI_GPU_GetThermalSettings;
extern "C" static NvAPI_EnumNvidiaDisplayHandleType m_pfnNvAPI_EnumNvidiaDisplayHandle;
extern "C" static NvAPI_GetPhysicalGPUsFromDisplayType m_pfnNvAPI_GetPhysicalGPUsFromDisplay;
extern "C" static NvAPI_EnumPhysicalGPUsType m_pfnNvAPI_EnumPhysicalGPUs;
extern "C" static NvAPI_GPU_GetTachReadingType m_pfnNvAPI_GPU_GetTachReading;
extern "C" static NvAPI_GPU_GetAllClocksType m_pfnNvAPI_GPU_GetAllClocks;
extern "C" static NvAPI_GPU_GetPStatesType m_pfnNvAPI_GPU_GetPStates;
extern "C" static NvAPI_GPU_GetUsagesType m_pfnNvAPI_GPU_GetUsages;
extern "C" static NvAPI_GPU_GetCoolerSettingsType m_pfnNvAPI_GPU_GetCoolerSettings;
extern "C" static NvAPI_GPU_SetCoolerLevelsType m_pfnNvAPI_GPU_SetCoolerLevels;
extern "C" static NvAPI_GPU_GetMemoryInfoType m_pfnNvAPI_GPU_GetMemoryInfo;
extern "C" static NvAPI_GetDisplayDriverVersionType m_pfnNvAPI_GetDisplayDriverVersion;
extern "C" static NvAPI_GetInterfaceVersionStringType m_pfnNvAPI_GetInterfaceVersionString;
extern "C" static NvAPI_GPU_GetPCIIdentifiersType m_pfnNvAPI_GPU_GetPCIIdentifiers;
//*****************************以下是类内部使用的结构体************************************//
/*
* @brief GPU信息
*/
struct RTXRENDER_API _GPU_INFO
{
	NvPhysicalGpuHandle     nvGpuHandle;        // GPU句柄
	int                     nUsage;             // GPU占用率
};
typedef _GPU_INFO GPU_INFO;
/*
* @brief 显卡信息
*/
struct RTXRENDER_API DISPLAY_CARD_INFO
{
	NvDisplayHandle nvDisplayHandle;            // 显卡句柄
	int             nGpuCount;                  // Gpu个数
	DWORD           dwTotalMemory;              // 总显存大小（KB）
	DWORD           dwFreeMemory;               // 空闲显存大小（KB）

	GPU_INFO        sGpuInfo[MAX_GPU_NUM];      // GPU信息
 };
//*****************************************************************************************//

/*
* @brief 显卡相关信息（可用于导出的结构体）
*/
struct RTXRENDER_API _DISPLAY_INFO
{
	int         nGpuCount;                  // Gpu个数
	int         nGpuUsages[MAX_GPU_NUM];    // Gpu占用率
	DWORD       dwTotalMemory;              // 总显存大小（KB）
	DWORD       dwFreeMemory;               // 空闲显存大小（KB）
};
typedef _DISPLAY_INFO DISPLAY_INFO;

USTRUCT(Blueprintable)
struct RTXRENDER_API FDISPLAY_INFO
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	int        nGpuCount;                  // Gpu个数
	UPROPERTY(BlueprintReadWrite)
	TArray<int32>		nGpuUsages;   // Gpu占用率
	UPROPERTY(BlueprintReadWrite)
	float       dwTotalMemory;              // 总显存大小（KB）
	UPROPERTY(BlueprintReadWrite)
	float       dwFreeMemory;               // 空闲显存大小（KB）
};

UENUM(BlueprintType)
enum class MemoryStates:uint8
{
	NormalState,
	BeyondRAM,
	BeyondMemory,
	AllBeyond
};
struct DXGI_ADAPTER_DESC;

UCLASS()
class RTXRENDER_API UDCFBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	/****获取显卡信息*****/
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static bool QueryAllDisplayCardInfo();

	/****判断是不是NVIDIA显卡*****/
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static bool IsNVIDIADisplay();

	/****判断显卡类型：NVIDIA ，AMD，Other*****/
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static bool CheckDisplayCardInfo(FString& CardType, FString& CardModel, FString& CardNumber, int32& GPUMemory);

	/****NVIDIA显卡的名字*****/
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static FString GetDisplayNameByCpp();

	/****NVIDIA显卡的名字*****/
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static FString GetDisplayCardName();

	/****正则检测*****/
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static bool CheckStringIsValid(const FString str, const FString Reg);

	/****正则检测是不是数字*****/
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static bool IsNumber(const FString str);

	/****正则检测是不是数字*****/
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static bool IsPhoneNumber(const FString str);

	//--------------------------
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		bool GetWindowIsActived();

	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static MemoryStates IsNeedAutoSave();
	//UFUNCTION(BlueprintCallable, Category = "DCFUtil")
	//	uint32 GetPID();

	//UFUNCTION(BlueprintCallable, Category = "DCFUtil")
	//	static FOnWindowActivatedEvent GetWindowActived();

	//UFUNCTION(BlueprintCallable, Category = "DCFUtil")
	//	static void SetWindowDeActived(FOnWindowDeactivated &DeActivatedDelegate);

	//UFUNCTION(BlueprintCallable, Category = "DCFUtil")
	//	static FOnWindowDeactivatedEvent GetWindowDeActived();

	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static void SetIsShowLog();


	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static bool GPUInit();
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
	    static bool GPUUnit();
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static int32 GetDisplayCardCount();
	UFUNCTION(BlueprintCallable, Category = "DCFUtil")
		static bool GetDisplayInfo(const int32 nCardIndex, FDISPLAY_INFO & pDisplayInfo);
private:

	static int32     EnumDisplayCards();
	static bool    GetGpuHandles(const NvDisplayHandle nvDisplayHandle, DISPLAY_CARD_INFO *pCardInfo);

	static bool    GetDisplayCardGpuUsages(const NvDisplayHandle nvDisplayHandle, DISPLAY_CARD_INFO *pCardInfo);
	static bool    GetDisplayCardMemoryInfo(const NvDisplayHandle nvDisplayHandle, DISPLAY_CARD_INFO *pCardInfo);

public:
	static int32                 m_nDisplayCardCount;
	static DISPLAY_CARD_INFO*  m_pDisplayCards;
	static void *             m_hNvApiDll;
	static bool   IsShow;
	static int32              SecondNum;
private:
	static TArray<DXGI_ADAPTER_DESC> DisplayCardInfoArray;
	static FString DisplayCardType;			//显卡类型：Nvidia, AMD ,Other
	static FString DisplayCardModel;		//显卡型号：GTX, GT
	static FString DisplayCardNumber;		//显示编号 1080 960
	
	static SWindow* SWindowPtr;
	
};
