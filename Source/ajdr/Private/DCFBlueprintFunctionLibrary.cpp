// Copyright© 2017 ihomefnt All Rights Reserved.

#include "DCFBlueprintFunctionLibrary.h"
#include <DXGI.h>
#include "Internationalization/Regex.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#include<tchar.h>
#include "Engine/GameEngine.h"

//#include "Windows/WindowsPlatformProcess.h"

using namespace std;
int32 UDCFBlueprintFunctionLibrary::m_nDisplayCardCount = 0;
DISPLAY_CARD_INFO* UDCFBlueprintFunctionLibrary::m_pDisplayCards = nullptr;
void* UDCFBlueprintFunctionLibrary::m_hNvApiDll = nullptr;
TArray<DXGI_ADAPTER_DESC> UDCFBlueprintFunctionLibrary::DisplayCardInfoArray;
FString UDCFBlueprintFunctionLibrary::DisplayCardType = "";
FString UDCFBlueprintFunctionLibrary::DisplayCardModel = "";
FString UDCFBlueprintFunctionLibrary::DisplayCardNumber = "";
SWindow* UDCFBlueprintFunctionLibrary::SWindowPtr = nullptr;
bool UDCFBlueprintFunctionLibrary::IsShow = false;
int32 UDCFBlueprintFunctionLibrary::SecondNum = 0;

bool  UDCFBlueprintFunctionLibrary::QueryAllDisplayCardInfo()
{
	//ASSERT(pDXGIFactory != 0);
	HINSTANCE hDXGI = LoadLibrary(L"dxgi.dll");

	typedef HRESULT(WINAPI* LPCREATEDXGIFACTORY)(REFIID, void**);

	LPCREATEDXGIFACTORY pCreateDXGIFactory = nullptr;
	IDXGIFactory* pDXGIFactory = nullptr;
	DisplayCardInfoArray.Empty();

	// We prefer the use of DXGI 1.1  
	pCreateDXGIFactory = (LPCREATEDXGIFACTORY)GetProcAddress(hDXGI, "CreateDXGIFactory1");

	if (!pCreateDXGIFactory)
	{
		pCreateDXGIFactory = (LPCREATEDXGIFACTORY)GetProcAddress(hDXGI, "CreateDXGIFactory");

		if (!pCreateDXGIFactory)
		{
			FreeLibrary(hDXGI);
			return false;
		}
	}

	HRESULT hr = pCreateDXGIFactory(__uuidof(IDXGIFactory), (LPVOID*)&pDXGIFactory);

	if (!pDXGIFactory)
		return false;

	for (UINT index = 0; ; ++index)
	{
		IDXGIAdapter* pAdapter = nullptr;
		HRESULT hr1 = pDXGIFactory->EnumAdapters(index, &pAdapter);
		if (FAILED(hr1)) // DXGIERR_NOT_FOUND is expected when the end of the list is hit  
			break;

		DXGI_ADAPTER_DESC desc;
		memset(&desc, 0, sizeof(DXGI_ADAPTER_DESC));
		if (SUCCEEDED(pAdapter->GetDesc(&desc)))
		{
			//显卡名称信息
			DisplayCardInfoArray.Push(desc); //desc.Description
		}

		pAdapter->Release();
	}

	return true;
}


bool UDCFBlueprintFunctionLibrary::CheckDisplayCardInfo(FString& CardType, FString& CardModel, FString& CardNumber, int32& GPUMemory)
{
	if (!QueryAllDisplayCardInfo())
	{
		return false;
	}

	DisplayCardType = "";
	for (size_t i = 0; i < DisplayCardInfoArray.Num(); i++)
	{
		FString DisplayCardName = DisplayCardInfoArray[i].Description;
		FString LeftString;
		FString RightString;
		if (DisplayCardName.StartsWith("NVIDIA"))
		{
			CardType = "NVIDIA";
			DisplayCardName.Split(" ", &LeftString, &RightString, ESearchCase::IgnoreCase, ESearchDir::FromStart);  //NVIDIA
			FString LeftString1, LeftString2, LeftString3;
			FString RightString1, RightString2, RightString3;
			RightString.Split(" ", &LeftString1, &RightString1, ESearchCase::IgnoreCase, ESearchDir::FromStart); //GeForce
			RightString1.Split(" ", &LeftString2, &RightString2, ESearchCase::IgnoreCase, ESearchDir::FromStart); //GTX
			RightString2.Split(" ", &LeftString3, &RightString3, ESearchCase::IgnoreCase, ESearchDir::FromStart); //1070
			CardModel = LeftString2;  //RTX GTX,Titan GT
			CardNumber = LeftString3.IsEmpty() ? RightString2: LeftString3;   //数字
			GPUMemory = DisplayCardInfoArray[i].DedicatedVideoMemory / 1024 / 1024;   //MB
			return true;
		}
		else if(DisplayCardName.StartsWith("AMD"))
		{
			CardType = "AMD";
			CardModel = "RX";
			CardNumber = "RX";
			GPUMemory = DisplayCardInfoArray[i].DedicatedVideoMemory / 1024 / 1024;
			return true;
		}
	}
	return false;
}
bool UDCFBlueprintFunctionLibrary::IsNVIDIADisplay()
{
	if (!QueryAllDisplayCardInfo())
	{
		return false;
	}
	for (size_t i = 0; i < DisplayCardInfoArray.Num(); i++)
	{
		FString LeftString1;
		FString RightString1;
		FString LeftString2;
		FString RightString2;
		FString DisplayCardName = DisplayCardInfoArray[i].Description;
		DisplayCardName.Split(" ", &LeftString1, &RightString1, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		LeftString1.Split(" ", &LeftString2, &RightString2, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		if (RightString2.StartsWith("GT"))
		{
			DisplayCardModel = RightString2;
			DisplayCardNumber = RightString1;
			return true;
		}
	}

	DisplayCardModel = "";
	DisplayCardNumber = "";
	return false;
}

FString UDCFBlueprintFunctionLibrary::GetDisplayNameByCpp()
{
	if (!IsNVIDIADisplay())
		return "";

	FString LeftString;
	FString RightString;

	if (DisplayCardNumber.EndsWith("Ti"))
	{
		DisplayCardNumber.Split("Ti", &LeftString, &RightString);
		DisplayCardNumber = LeftString;
	}

	return DisplayCardNumber;
}

FString UDCFBlueprintFunctionLibrary::GetDisplayCardName()
{
	return FGenericPlatformMisc::GetPrimaryGPUBrand();
}

bool UDCFBlueprintFunctionLibrary::CheckStringIsValid(const FString str, const FString Reg)
{
	FRegexPattern Pattern(Reg);
	FRegexMatcher regMatcher(Pattern, str);
	regMatcher.SetLimits(0, str.Len());
	return regMatcher.FindNext();
}

bool UDCFBlueprintFunctionLibrary::IsNumber(const FString str)
{
	if (CheckStringIsValid(str, "[^0-9]"))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool UDCFBlueprintFunctionLibrary::IsPhoneNumber(const FString str)
{
	if (CheckStringIsValid(str, "^((13[0-9])|(14[5|7])|(15([0-3]|[5-9]))|(18[0,5-9]))\\d{8}$"))
	{
		return false;
	}
	else
	{
		return true;
	}
	return false;
}

bool UDCFBlueprintFunctionLibrary::GetWindowIsActived()
{
	if (!SWindowPtr)
	{
		UGameEngine* gameEngine = Cast<UGameEngine>(GEngine);
		if (gameEngine)
		{
			TSharedPtr<SWindow> windowPtr = gameEngine->GameViewportWindow.Pin();
			SWindowPtr = windowPtr.Get();
		}
	}
	return SWindowPtr->IsActive();
}



//FOnWindowActivatedEvent UDCFBlueprintFunctionLibrary::GetWindowActived()
//{
//	FOnWindowActivatedEvent Event;
//	if (!SWindowPtr)
//	{
//		UGameEngine* gameEngine = Cast<UGameEngine>(GEngine);
//		if (gameEngine)
//		{
//			TSharedPtr<SWindow> windowPtr = gameEngine->GameViewportWindow.Pin();
//			SWindowPtr = windowPtr.Get();
//			Event = SWindowPtr->GetOnWindowActivatedEvent();
//		}
//	}
//
//	return Event;
//}

//void UDCFBlueprintFunctionLibrary::SetWindowDeActived(FOnWindowDeactivated &DeActivatedDelegate)
//{
//	GEngine->GameViewport->GetWindow()->SetOnWindowDeactivated(DeActivatedDelegate);
//}

//FOnWindowDeactivatedEvent UDCFBlueprintFunctionLibrary::GetWindowDeActived()
//{
//	auto Event = GEngine->GameViewport->GetWindow()->GetOnWindowDeactivatedEvent();
//	return FOnWindowDeactivatedEvent();
//}

MemoryStates UDCFBlueprintFunctionLibrary::IsNeedAutoSave()
{
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(ms);
	GlobalMemoryStatusEx(&ms);
	float TotalPhys;//总物理内存
	float AvailPhys;//剩余可用物理内存
	DWORD dwMemoryLoad;//内存占用率
	DWORD MemoryLoad;
	int8 BeyondRAM=0;
	int8 BeyondMemory=0;
	TotalPhys = (float)ms.ullTotalPhys / 1024.f / 1024.f / 1024.f;
	AvailPhys = (float)ms.ullAvailPhys / 1024.f / 1024.f / 1024.f;
	dwMemoryLoad = ms.dwMemoryLoad;
	FDISPLAY_INFO  GPUINFO;

	if (m_hNvApiDll!=nullptr)
	{
		if (GetDisplayInfo(0, GPUINFO))
		{
			MemoryLoad = (GPUINFO.dwTotalMemory - GPUINFO.dwFreeMemory) * 100 / GPUINFO.dwTotalMemory;
			if (SecondNum%6==0&&IsShow)
				UE_LOG(LogTemp, Warning, TEXT("剩余显存%.2fG/最大显存%.2fG  显存占用率：%d"), GPUINFO.dwFreeMemory, GPUINFO.dwTotalMemory, MemoryLoad);
		}
	}
	if (SecondNum % 6 == 0&&IsShow)
		UE_LOG(LogTemp, Warning, TEXT("剩余内存%.2fG/总内存%.2fG  内存占用率：%d"), AvailPhys, TotalPhys, dwMemoryLoad);
	if (dwMemoryLoad >70)
	{
		BeyondRAM = 1;
	}
	if (MemoryLoad > 80)
	{
		BeyondMemory = 2;
	}
	++SecondNum;
	MemoryStates memorystate;
	memorystate = MemoryStates(BeyondRAM + BeyondMemory);
	return memorystate;
	
}



int32 UDCFBlueprintFunctionLibrary::EnumDisplayCards()
{
	NvStatus nvResult;
	NvDisplayHandle nvDisplayCardHandle;

	int nIndex = 0;

	m_nDisplayCardCount = 0;
	if (m_pfnNvAPI_EnumNvidiaDisplayHandle)
	{
		for (nIndex = 0; nIndex < MAX_DISPLAY_CARDS; ++nIndex)
		{
			nvResult = m_pfnNvAPI_EnumNvidiaDisplayHandle(nIndex, &nvDisplayCardHandle);
			if (enumNvStatus_OK == nvResult)
			{
				m_pDisplayCards[m_nDisplayCardCount].nvDisplayHandle = nvDisplayCardHandle;
				++m_nDisplayCardCount;
			}
		}
	}

	return m_nDisplayCardCount;
}


bool UDCFBlueprintFunctionLibrary::GetGpuHandles(const NvDisplayHandle nvDisplayHandle, DISPLAY_CARD_INFO *pCardInfo)
{
	bool bResult = false;

	NvStatus nvStatus;
	NvPhysicalGpuHandle *pnvHandles = NULL;

	int nIndex = 0;
	unsigned int uiGpuCount = 0;

	if (m_hNvApiDll&&m_pfnNvapi_QueryInterface&&m_pfnNvAPI_GetPhysicalGPUsFromDisplay)
	{
		pnvHandles = new NvPhysicalGpuHandle[MAX_PHYSICAL_GPUS];
		nvStatus = m_pfnNvAPI_GetPhysicalGPUsFromDisplay(nvDisplayHandle, pnvHandles, &uiGpuCount);
		if (enumNvStatus_OK == nvStatus)
		{
			if (uiGpuCount > MAX_GPU_NUM)
				pCardInfo->nGpuCount = MAX_GPU_NUM;
			else
				pCardInfo->nGpuCount = uiGpuCount;
			for (nIndex = 0; nIndex < pCardInfo->nGpuCount; ++nIndex)
			{
				pCardInfo->sGpuInfo[nIndex].nvGpuHandle = pnvHandles[nIndex];
			}

			bResult = true;
		}

		delete[]pnvHandles;
		pnvHandles = NULL;
	}

	return bResult;
}


bool UDCFBlueprintFunctionLibrary::GetDisplayCardGpuUsages(const NvDisplayHandle nvDisplayHandle, DISPLAY_CARD_INFO *pCardInfo)
{
	bool bResult = false;

	int nIndex = 0;

	NvStatus nvStatus = enumNvStatus_ERROR;
	NvUsages *pnvUsages = NULL;

	if (m_pfnNvAPI_GPU_GetUsages)
	{
		pnvUsages = new NvUsages;
		pnvUsages->Version = GPU_USAGES_VER;
		for (nIndex = 0; nIndex < pCardInfo->nGpuCount; ++nIndex)
		{
			nvStatus = m_pfnNvAPI_GPU_GetUsages(pCardInfo->sGpuInfo[nIndex].nvGpuHandle, pnvUsages);
			if (enumNvStatus_OK == nvStatus)
			{
				pCardInfo->sGpuInfo[nIndex].nUsage = pnvUsages->Usages[2];
			}
		}

		delete pnvUsages;
		pnvUsages = NULL;

		bResult = (enumNvStatus_OK == nvStatus) ? true : false;
	}

	return bResult;
}

bool UDCFBlueprintFunctionLibrary::GetDisplayCardMemoryInfo(const NvDisplayHandle nvDisplayHandle, DISPLAY_CARD_INFO *pCardInfo)
{
	bool bResult = false;

	int nIndex = 0;

	NvStatus nvStatus = enumNvStatus_ERROR;
	NvUsages *pnvUsages = NULL;
	NvMemoryInfo sMemoryInfo;

	if (m_hNvApiDll&&m_pfnNvapi_QueryInterface&&m_pfnNvAPI_GPU_GetMemoryInfo)
	{
		sMemoryInfo.Version = GPU_MEMORY_INFO_VER;
		nvStatus = m_pfnNvAPI_GPU_GetMemoryInfo(nvDisplayHandle, &sMemoryInfo);
		if (enumNvStatus_OK == nvStatus)
		{
			pCardInfo->dwTotalMemory = (DWORD)(sMemoryInfo.Values[0]);
			pCardInfo->dwFreeMemory = (DWORD)(sMemoryInfo.Values[4]);

			bResult = true;
		}
	}

	return bResult;
}


int32 UDCFBlueprintFunctionLibrary::GetDisplayCardCount()
{
	return m_nDisplayCardCount;
}

bool UDCFBlueprintFunctionLibrary::GetDisplayInfo(const int32 nCardIndex, FDISPLAY_INFO & pDisplayInfo)
{
	bool bResult = false;

	int nIndex = 0;

	if (m_hNvApiDll&&m_pfnNvapi_QueryInterface &&m_pfnNvAPI_Initialize)
	{
		/*bResult = GetDisplayCardGpuUsages(m_pDisplayCards[nCardIndex].nvDisplayHandle, &m_pDisplayCards[nCardIndex]);
		MY_PROCESS_ERROR(bResult);

		pDisplayInfo.nGpuCount = m_pDisplayCards[nCardIndex].nGpuCount;
		pDisplayInfo.nGpuUsages.Empty();
		for (nIndex = 0; nIndex < pDisplayInfo.nGpuCount; ++nIndex)
		{
			pDisplayInfo.nGpuUsages.Add(m_pDisplayCards[nCardIndex].sGpuInfo[nIndex].nUsage);
		}*/

		bResult = GetDisplayCardMemoryInfo(m_pDisplayCards[nCardIndex].nvDisplayHandle, &m_pDisplayCards[nCardIndex]);
		MY_PROCESS_ERROR(bResult);

		pDisplayInfo.dwTotalMemory = m_pDisplayCards[nCardIndex].dwTotalMemory/1024/1024;
		pDisplayInfo.dwFreeMemory = m_pDisplayCards[nCardIndex].dwFreeMemory/1024/1024;
	}
Exit0:
	return bResult;
}


bool UDCFBlueprintFunctionLibrary::GPUInit()
{
	bool bResult = false;
	bool bRetCode = false;

	int nIndex = 0;
	int nResult = 0;

     m_hNvApiDll = LoadLibrary(L"nvapi64.dll");
	 if (!m_hNvApiDll)
	 {
		 m_hNvApiDll = LoadLibrary(L"nvapi.dll");
	 }
	if (m_hNvApiDll)
	{
		m_pfnNvapi_QueryInterface = (nvapi_QueryInterfaceType)GetProcAddress((HMODULE)m_hNvApiDll, "nvapi_QueryInterface");
		if (m_pfnNvapi_QueryInterface)
		{
			m_pfnNvAPI_Initialize = (NvAPI_InitializeType)m_pfnNvapi_QueryInterface(ID_NvAPI_Initialize);
			m_pfnNvAPI_GPU_GetFullName = (NvAPI_GPU_GetFullNameType)m_pfnNvapi_QueryInterface(ID_NvAPI_GPU_GetFullName);
			m_pfnNvAPI_GPU_GetThermalSettings = (NvAPI_GPU_GetThermalSettingsType)m_pfnNvapi_QueryInterface(ID_NvAPI_GPU_GetThermalSettings);
			m_pfnNvAPI_EnumNvidiaDisplayHandle = (NvAPI_EnumNvidiaDisplayHandleType)m_pfnNvapi_QueryInterface(ID_NvAPI_EnumNvidiaDisplayHandle);
			m_pfnNvAPI_GetPhysicalGPUsFromDisplay = (NvAPI_GetPhysicalGPUsFromDisplayType)m_pfnNvapi_QueryInterface(ID_NvAPI_GetPhysicalGPUsFromDisplay);
			m_pfnNvAPI_EnumPhysicalGPUs = (NvAPI_EnumPhysicalGPUsType)m_pfnNvapi_QueryInterface(ID_NvAPI_EnumPhysicalGPUs);
			m_pfnNvAPI_GPU_GetTachReading = (NvAPI_GPU_GetTachReadingType)m_pfnNvapi_QueryInterface(ID_NvAPI_GPU_GetTachReading);
			m_pfnNvAPI_GPU_GetAllClocks = (NvAPI_GPU_GetAllClocksType)m_pfnNvapi_QueryInterface(ID_NvAPI_GPU_GetAllClocks);
			m_pfnNvAPI_GPU_GetPStates = (NvAPI_GPU_GetPStatesType)m_pfnNvapi_QueryInterface(ID_NvAPI_GPU_GetPStates);
			m_pfnNvAPI_GPU_GetUsages = (NvAPI_GPU_GetUsagesType)m_pfnNvapi_QueryInterface(ID_NvAPI_GPU_GetUsages);
			m_pfnNvAPI_GPU_GetCoolerSettings = (NvAPI_GPU_GetCoolerSettingsType)m_pfnNvapi_QueryInterface(ID_NvAPI_GPU_GetCoolerSettings);
			m_pfnNvAPI_GPU_SetCoolerLevels = (NvAPI_GPU_SetCoolerLevelsType)m_pfnNvapi_QueryInterface(ID_NvAPI_GPU_SetCoolerLevels);
			m_pfnNvAPI_GPU_GetMemoryInfo = (NvAPI_GPU_GetMemoryInfoType)m_pfnNvapi_QueryInterface(ID_NvAPI_GPU_GetMemoryInfo);
			m_pfnNvAPI_GetDisplayDriverVersion = (NvAPI_GetDisplayDriverVersionType)m_pfnNvapi_QueryInterface(ID_NvAPI_GetDisplayDriverVersion);
			m_pfnNvAPI_GetInterfaceVersionString = (NvAPI_GetInterfaceVersionStringType)m_pfnNvapi_QueryInterface(ID_NvAPI_GetInterfaceVersionString);
			m_pfnNvAPI_GPU_GetPCIIdentifiers = (NvAPI_GPU_GetPCIIdentifiersType)m_pfnNvapi_QueryInterface(ID_NvAPI_GPU_GetPCIIdentifiers);

			if (m_pfnNvAPI_Initialize)
			{
				nResult = m_pfnNvAPI_Initialize();
				if (enumNvStatus_OK == nResult)
				{
					m_pDisplayCards = new DISPLAY_CARD_INFO[MAX_DISPLAY_CARDS];
					ZeroMemory(m_pDisplayCards, MAX_DISPLAY_CARDS * sizeof(DISPLAY_CARD_INFO));

					//// 获取显卡个数
					//nResult = EnumDisplayCards();
					//MY_PROCESS_ERROR(nResult > 0);

					//// 获取每块显卡的GPU个数
					//for (nIndex = 0; nIndex < m_nDisplayCardCount; ++nIndex)
					//{
						bRetCode = GetGpuHandles(m_pDisplayCards[nIndex].nvDisplayHandle, &m_pDisplayCards[nIndex]);
						MY_PROCESS_ERROR(bRetCode);
					//}

					bResult = true;
				}
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("只支持查看N卡显存"));
Exit0:
	return bResult;
}


bool UDCFBlueprintFunctionLibrary::GPUUnit()
{
	m_nDisplayCardCount = 0;

	if (m_pDisplayCards)
	{
		delete[]m_pDisplayCards;
		m_pDisplayCards = NULL;
	}

	if (m_hNvApiDll)
	{
		FreeLibrary((HMODULE)m_hNvApiDll);
		m_hNvApiDll = NULL;
	}

	return true;
}

void UDCFBlueprintFunctionLibrary::SetIsShowLog()
{
	IsShow = !IsShow;
}
