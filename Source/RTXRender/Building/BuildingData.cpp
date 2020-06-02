
#include "BuildingData.h"
#include "IClass.h"
#include "Math/kBox.h"
#include "DRGameMode.h"
#include "IrayGameMode.h"
#include "Building/BuildingSystem.h"

UBuildingData::UBuildingData(const FObjectInitializer &ObjectIntializer)
	: Super(ObjectIntializer)
	, ObjID(INVALID_OBJID)
	, TenmpID(INVALID_OBJID)
{

}

void UBuildingData::SetRawObj(int32 InObjID)
{
	ObjID = InObjID;
}

IValueFactory *UBuildingData::GetValueFactory()
{
	UBuildingSystem *NewBuildingSystem = GetBuildingSystem();
	if (NewBuildingSystem)
	{
		return NewBuildingSystem->GetValueFactory();
	}
	return nullptr;
}

UBuildingSystem *UBuildingData::GetBuildingSystem()
{ 
	UBuildingSystem *Suite = Cast<UBuildingSystem>(GetOuter());
	return Suite; 
}

class ISuite *UBuildingData::GetSuite()
{
	UBuildingSystem *Suite = GetBuildingSystem();
	if (Suite)
	{
		return Suite->GetSuite(); 
	}
	return nullptr;
}

IObject *UBuildingData::GetRawObj()
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		return Suite->GetObject(ObjID);
	}
	return nullptr;
}

int32 UBuildingData::GetObjectType()
{
	IObject *pObj = GetRawObj();
	if (pObj)
	{
		return pObj->GetType();
	}
	return 0;
}

void UBuildingData::Update()
{
	if (GetRawObj())
	{
		GetRawObj()->MarkNeedUpdate(EChannelUserData);
	}
}

float UBuildingData::GetFloat(const FString &Name)
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*Name);
		return Suite->GetProperty(ObjID, AnsiName).FloatValue();
	}
	return 0;
}

bool  UBuildingData::GetBool(const FString &Name)
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*Name);
		return Suite->GetProperty(ObjID, AnsiName).BoolValue();
	}
	return false;
}

int32  UBuildingData::GetInt(const FString &Name)
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*Name);
		return Suite->GetProperty(ObjID, AnsiName).IntValue();
	}
	return 0;
}

FString UBuildingData::GetString(const FString &Name)
{
	FString RetStr;
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*Name);
		const char *AnsiValue = Suite->GetProperty(ObjID, AnsiName).StrValue();
		RetStr = ANSI_TO_TCHAR(AnsiValue);
	}
	return RetStr;
}

FVector  UBuildingData::GetVector(const FString &Name)
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*Name);
		IValue &Value = Suite->GetProperty(ObjID, AnsiName);
		
		if (Value.GetType()==kV_Vec2D)
		{
			return ToUE4Position(kVector3D(Value.Vec2Value()));
		}
		else if(Value.GetType()==kV_Vec3D)
		{
			return ToUE4Position(Value.Vec3Value());
		}
	}
	return FVector::ZeroVector;
}

FRotator  UBuildingData::GetRotation(const FString &Name)
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*Name);
		return ToUE4Rotation(Suite->GetProperty(ObjID, AnsiName).RotationValue());
	}
	return FRotator::ZeroRotator;
}

FBox  UBuildingData::GetBounds(const FString &Name)
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*Name);
		return ToUE4Bounds(Suite->GetProperty(ObjID, AnsiName).Bounds());
	}
	return FBox();
}

FPlane UBuildingData::GetPlane(const FString &Name)
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*Name);
		return ToUE4Plane(Suite->GetProperty(ObjID, AnsiName).PlaneValue());
	}
	return FPlane::ZeroVector;
}

FVector2D UBuildingData::GetVector2D(const FString &Name)
{
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*Name);
		return ToVector2D(Suite->GetProperty(ObjID, AnsiName).Vec2Value());
	}
	return FVector2D::ZeroVector;
}

bool UBuildingData::IsRoot()
{
	if (GetRawObj())
	{
		UE_LOG(LogTemp, Log, TEXT("IsRoot(float[%d])"), (int32)GetRawObj()->IsRoot());
		return GetRawObj()->IsRoot();
	}
	return false;
}

void UBuildingData::SetFloat(const FString &Name, float fValue)
{
	if (BuildingSystem)
	{
		UE_LOG(LogTemp, Log, TEXT("%s::SetFloat(float[%f])"),*Name,fValue);
		BuildingSystem->OnPropertyChanged(this, Name, GetValueFactory()->Create(fValue));
	}
}

void UBuildingData::SetInt(const FString & Name, int32 iValue)
{
	if (BuildingSystem)
	{
		UE_LOG(LogTemp, Log, TEXT("%s::SetInt(int32[%d])"), *Name, iValue);
		BuildingSystem->OnPropertyChanged(this, Name, GetValueFactory()->Create(iValue));
	}
}

void UBuildingData::SetBool(const FString &Name, bool bValue)
{
	if (BuildingSystem)
	{
		UE_LOG(LogTemp, Log, TEXT("%s::SetBool(bool[%d])"), *Name, bValue);
		BuildingSystem->OnPropertyChanged(this, Name, GetValueFactory()->Create(bValue));
	}
}

void UBuildingData::SetVector(const FString &Name, const FVector &Value)
{
	if (BuildingSystem)
	{
		kVector3D V(Value.X, Value.Y, Value.Z);
		UE_LOG(LogTemp, Log, TEXT("%s::SetBool(FVector[%s])"), *Name, *Value.ToString());
		BuildingSystem->OnPropertyChanged(this, Name, GetValueFactory()->Create(&V));
	}
}

void UBuildingData::SetVector2D(const FString &Name, const FVector2D &Value)
{
	if (BuildingSystem)
	{
		kPoint V(Value.X, Value.Y);
		UE_LOG(LogTemp, Log, TEXT("%s::SetVector2D(FVector2D[%s])"), *Name, *Value.ToString());
		BuildingSystem->OnPropertyChanged(this, Name, GetValueFactory()->Create(&V));
	}
}

void UBuildingData::SetRotator(const FString &Name, const FRotator &Value)
{
	if (BuildingSystem)
	{
		kRotation V(Value.Pitch, Value.Yaw, Value.Roll);
		UE_LOG(LogTemp, Log, TEXT("%s::SetRotator(FRotator[%s])"), *Name, *Value.ToString());
		BuildingSystem->OnPropertyChanged(this, Name, GetValueFactory()->Create(&V));
	}
}

void UBuildingData::SetString(const FString &Name, const FString &Value)
{
	if (BuildingSystem)
	{
		const char *ansiValue = TCHAR_TO_ANSI(*Value);
		UE_LOG(LogTemp, Log, TEXT("%s::SetString(FString[%s])"), *Name, *Value);
		BuildingSystem->OnPropertyChanged(this, Name, GetValueFactory()->Create(ansiValue));
	}
}

void UBuildingData::SetBounds(const FString &Name, const FBox &Box)
{
	if (BuildingSystem)
	{
		kBox3D Bounds(ToBuildingPosition(Box.Min), ToBuildingPosition(Box.Max));
		BuildingSystem->OnPropertyChanged(this, Name, GetValueFactory()->Create(&Bounds));
	}
}

IObjectDesc *UBuildingData::GetRawDesc()
{
	IObjectDesc *Desc = nullptr;
	ISuite *Suite = GetSuite();
	if (Suite)
	{
		IObject *pObj = Suite->GetObject(ObjID);
		if (pObj)
		{
			Desc = pObj->GetDesc();
		}
	}
	return Desc;
}

void UBuildingData::GetAllDesc(TArray<IObjectDesc*> &Descs, TArray<int32> &DescCounts)
{
	IObjectDesc *Desc = GetRawDesc();
	while (Desc)
	{
		Descs.Add(Desc);
		DescCounts.Add(Desc->GetNumberOfProperty());
		Desc = Desc->GetSuperDesc();
	}
}

int32 UBuildingData::GetNumberOfProperties()
{
	int32 NumProps = 0;

	IObject *Obj = GetRawObj();
	if (Obj)
	{
		NumProps = Obj->GetNumberOfProperties();
	}

	return NumProps;
}

IProperty *UBuildingData::GetProperty(int32 Index)
{
	IProperty *Prop = nullptr;
	IObject *Obj = GetRawObj();
	if (Obj)
	{
		Prop = Obj->GetProperty(Index);
	}
	return Prop;
}

IProperty *UBuildingData::GetProperty(const FString &Name)
{
	IProperty *Prop = nullptr;
	IObject *Obj = GetRawObj();
	if (Obj)
	{
		const char *AnsiName = TCHAR_TO_ANSI(*Name);
		Prop = Obj->GetProperty(AnsiName);
	}
	return Prop;
}

int32 UBuildingData::GetPropertyType(int32 Index)
{
	IProperty *Prop = GetProperty(Index);
	if (Prop)
	{
		return (int32)Prop->GetValueType();
	}
	return 0;
}

FString UBuildingData::GetPropertyName(int32 Index)
{
	FString PropName;
	IProperty *Prop = GetProperty(Index);
	if (Prop)
	{
	 	PropName = ANSI_TO_TCHAR(Prop->GetName());
	}
	return PropName;
}

bool UBuildingData::IsPropertyReadOnly(int32 Index)
{
	IProperty *Prop = GetProperty(Index);
	if (Prop)
	{
		return Prop->IsReadOnly();
	}
	return true;
}

FPropertyInfo UBuildingData::GetPropertyInfo(int32 Index)
{
	FPropertyInfo Info;
	IProperty *Prop = GetProperty(Index);
	if (Prop)
	{
		Info.Type = Prop->GetValueType();
		Info.bReadOnly = Prop->IsReadOnly();
		Info.PropName = ANSI_TO_TCHAR(Prop->GetName());
		Info.ClassName = ANSI_TO_TCHAR(Prop->GetDesc()->GetObjectClassName());
	}
	return Info;
}

FBuildingConfig *UBuildingData::GetConfig()
{
	ADRGameMode *MyGame = Cast<ADRGameMode>(GetWorld()->GetAuthGameMode());
	if (MyGame)
	{
		return MyGame->GetConfig();
	}
	AIrayGameMode *MyIrayGame = Cast<AIrayGameMode>(GetWorld()->GetAuthGameMode());
	if (MyIrayGame)
	{
		return MyIrayGame->GetConfig();
	}
	return nullptr;
}


