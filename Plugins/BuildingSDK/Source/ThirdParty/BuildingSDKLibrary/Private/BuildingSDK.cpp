
#include "BuildingSDK.h"
#include "ISuite.h"
#include "MXFile/MXFile.h"
#include "Stream/FileStream.h"

IValueFactory *GValueFactory = NULL;

BuildingSDK::BuildingSDK()
	:_bInitialized(false)
	,_Transaction(nullptr)
{
}

bool BuildingSDK::Initialize()
{
	_bInitialized = true;
	_Transaction = new FTransaction();
	GValueFactory = &_ValueFactoy;
	return true;
}

void BuildingSDK::UnInitialize()
{
	for (size_t i = 0; i < _Suites.size(); ++i)
	{
		SuiteImpl *Suite = _Suites[i];
		delete Suite;
	}
	
	_bInitialized = false; 
	GValueFactory = nullptr;

	if (_Transaction)
	{
		delete _Transaction;
		_Transaction = nullptr;
	}
}

ITransact *BuildingSDK::GetTransaction()
{
	return _Transaction;
}

void BuildingSDK::SetTransaction(ITransact *InTransaction)
{
	_Transaction = InTransaction;
}

ISuite *BuildingSDK::CreateSuite()
{
#if	!USE_MX_ONLY
	if (_bInitialized)
	{
		SuiteImpl *Suite = new SuiteImpl();
		_Suites.push_back(Suite);
		return Suite;
	}
#endif
	return nullptr;
}

void BuildingSDK::DestroySuite(ISuite *Suite)
{
#if !USE_MX_ONLY
	SuiteImpl *SuiteImp = (SuiteImpl*)Suite;
	if (SuiteImp)
	{
		for (size_t i = 0; i < _Suites.size(); ++i)
		{
			if (_Suites[i] == SuiteImp)
			{
				_Suites.erase(_Suites.begin() + i);
				break;
			}
		}
		delete SuiteImp;
	}
#endif
}

ISuite *BuildingSDK::LoadSuite(const char *Filename)
{
#if !USE_MX_ONLY
	if (_bInitialized)
	{
		SuiteImpl *Suite = (SuiteImpl *)CreateSuite();
		Suite->Load(Filename);
		return Suite;
	}
#endif
	return nullptr;
}

ISerialize *BuildingSDK::CreateFileWriter(const char *Filename)
{
	return new FileWriter(Filename);
}

ISerialize *BuildingSDK::CreateFileReader(const char *Filename)
{
	return new FileReader(Filename);
}

IObject *BuildingSDK::LoadFile(const char *Filename)
{
	return MXFile::LoadFromFile(Filename);
}

IValueFactory *BuildingSDK::GetValueFactory()
{
	return &_ValueFactoy;
}

IImporter *BuildingSDK::GetImporter()
{
	return &_Importer;
}

IBuildingSDK *GetBuildingSDK()
{
	static BuildingSDK sdk;
	return  &sdk;
}

extern "C" _declspec(dllexport) void * LoadSDKFunction()
{
	return GetBuildingSDK();
}

